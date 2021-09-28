// vim: set expandtab fdm=marker ts=2 sw=2 tw=80 et :
//
// @author      : Ruan E. Formigoni (ruanformigoni@gmail.com)
// @file        : verilog
// @created     : domingo dez 20, 2020 14:00:25 -03
//
// BSD 2-Clause License

// Copyright (c) 2020, Ruan Evangelista Formigoni
// All rights reserved.

// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:

// * Redistributions of source code must retain the above copyright notice, this
//   list of conditions and the following disclaimer.

// * Redistributions in binary form must reproduce the above copyright notice,
//   this list of conditions and the following disclaimer in the documentation
//   and/or other materials provided with the distribution.

// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
// FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
// DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
// SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
// CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
// OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

#pragma once

#include <optional>
#include <map>
#include <sstream>
#include <type_traits>
#include <regex>
#include <fstream>

#include <spdlog/spdlog.h>
#include <range/v3/all.hpp>

#include <celaeno/aliases.hpp>
#include <celaeno/concepts.hpp>
#include <celaeno/err/err.hpp>

// celaeno::graph::io::verilog {{{
namespace celaeno::graph::io::verilog
{

// Namespaces {{{
namespace err = celaeno::err;

namespace rg = ranges;
namespace rv = ranges::views;
namespace ra = ranges::actions;
// }}}

// Using namespaces {{{
using namespace celaeno::concepts;
using namespace celaeno::aliases;
// }}}

// Enum Type {{{
enum class GateType{INPUT,NOT,AND,NAND,OR,NOR,XOR,XNOR,MAJ3,};
// }}}

// class Reader {{{
template<typename T>
class Reader
{
  // Private Members {{{
  private:
    static std::string_view const expr_id;
    static std::string_view const expr_assign;
    static std::string_view const expr_and;
    static std::string_view const expr_or;

    mutable T callback;
    mutable i64 id_counter;
    mutable std::map<std::string,i64> ids;
    mutable std::map<i64,GateType> gate_type;
  // }}}

  // Constructors {{{
  public:
    template<typename S>
    Reader(S&& _filename, T _callback);
  // }}}

  // Element Access {{{
    std::map<i64,GateType> const& data() const noexcept;
  // }}}

  // Private Methods {{{
  private:
    // Modifiers {{{
    bool on_assign( std::string const& _line ) const;
    bool on_and( std::string const& _line ) const;
    bool on_or( std::string const& _line ) const;
    void update(auto&& _lhs, GateType const& _type, auto&&... _ops) const;
    // }}}
  // }}}
};

// Static members initialization {{{
template<typename T>
std::string_view const Reader<T>::expr_id = "[A-Za-z0-9_\\(\\)~\\.]+";
template<typename T>
std::string_view const Reader<T>::expr_assign = "assign({0})=~?({0})";
template<typename T>
std::string_view const Reader<T>::expr_and = "assign({0})=~?({0}){1}~?({0})";
template<typename T>
std::string_view const Reader<T>::expr_or = "assign({0})=~?({0}){1}~?({0})";
// }}}

// Constructors {{{
template<typename T>
template<typename S>
Reader<T>::Reader(S&& _filename, T _callback)
  : callback(_callback)
  , id_counter()
  , ids()
  , gate_type()
{

#ifndef NDEBUG
  spdlog::set_level(spdlog::level::debug);
#endif

  std::ifstream ifile{_filename};

  err::err({ifile.good()})("Invalid input file {}", _filename);

  std::string input;

  { std::stringstream ss; ss << ifile.rdbuf(); input = ss.str(); }

  auto eval_regex = [&](auto&& _line, auto&& pattern, [[maybe_unused]] auto&& msg)
  {
    std::regex r{pattern};

    std::smatch m;

    if( std::regex_match(_line,m,r) )
    {
#if defined(DEBUG) && defined(DEBUG_SHOW_HDL)
      spdlog::debug(msg);
#endif
      return true;
    }
    return false;
  };

  for(auto rng : input
    | rv::filter([](auto c){ return c != ' ' && c != '\n' && c != '\t';  })
    | ra::split(';')
  )
  {
    std::string _line(rng.begin(),rng.end());

    err::err({
      // Newline
      eval_regex(_line,"^$","Newline")
      // Comment
      || eval_regex(_line,"^//.*$",fmt::format("Comment: {}",_line))
      // Module
      || eval_regex(_line,
        fmt::format("module{}.*",expr_id),
        fmt::format("Module: {}",_line))
      // Endmodule
      || eval_regex(_line,"endmodule","Endmodule")
      // Inputs/Outputs
      || eval_regex(_line,
        fmt::format("((input)|(output))({}[,;]?)+",expr_id),
        fmt::format("IO: {}",_line))
      // Wire declarations
      || eval_regex(_line,
        fmt::format("(wire)?({}[,;]?)+",expr_id),
        fmt::format("Wire: {}",_line))
      // Assigns
      || this->on_assign(_line)
      // And gates
      || this->on_and(_line)
      // Or gates
      || this->on_or(_line)
    })("Did not match: {}", _line);
  }

  // Set non-gate ids as inputs
  rg::for_each(this->ids, [this](auto e)
  {
    auto id {e.second};

    if( ! this->gate_type.contains(id) )
    {
      this->gate_type.emplace(id,GateType::INPUT);
    } // if
  });
}
// }}}

// Element Access {{{
template<typename T>
std::map<i64,GateType> const& Reader<T>::data() const noexcept
{
  return this->gate_type;
}
// }}}

// Modifiers {{{

// fn: on_assign {{{
template<typename T>
bool Reader<T>::on_assign( std::string const& _line ) const
{
  std::regex r{fmt::format(expr_assign, expr_id)};

  std::smatch m;
  if( std::regex_match(_line,m,r) )
  {
#if defined(DEBUG) && defined(DEBUG_SHOW_HDL)
    spdlog::debug(fmt::format("Assign: ({}) Input: ({})",
      std::string(m[1]), std::string(m[2])));
#endif
    this->update(m[1],GateType::NOT,m[2]);
    return true;
  }
  return false;
} // fn: on_assign }}}

// fn: on_and {{{
template<typename T>
bool Reader<T>::on_and( std::string const& _line ) const
{
  std::regex r{fmt::format(expr_and, expr_id, "&")};

  std::smatch m;
  if( std::regex_match(_line,m,r) )
  {
#if defined(DEBUG) && defined(DEBUG_SHOW_HDL)
    spdlog::debug(fmt::format("Gate: ({}) Inputs: ({}) & ({})",
      std::string(m[1]), std::string(m[2]), std::string(m[3])));
#endif
    this->update(m[1], GateType::AND, m[2], m[3]);
    return true;
  }
  return false;
} // fn: on_and }}}

// fn: on_or {{{
template<typename T>
bool Reader<T>::on_or( std::string const& _line ) const
{
  std::regex r{fmt::format(expr_or, expr_id, "\\|")};

  std::smatch m;
  if( std::regex_match(_line,m,r) )
  {
#if defined(DEBUG) && defined(DEBUG_SHOW_HDL)
    spdlog::debug(fmt::format("Gate: ({}) Inputs: ({}) | ({})",
      std::string(m[1]), std::string(m[2]), std::string(m[3])));
#endif
    this->update(m[1], GateType::OR, m[2], m[3]);
    return true;
  }
  return false;
} // fn: on_or }}}

// fn: update {{{
template<typename T>
void Reader<T>::update(auto&& _lhs, GateType const& _type, auto&&... _ops) const
{
  // Include gate id in list if it was not found before
  [this]<typename... Args>(Args&&... args) -> void
  {
    auto inc = [this](auto&& arg)
    {
      if( ! this->ids.contains(arg) )
      {
        this->ids[arg] = this->id_counter++;
      }
    };

    (inc(std::forward<Args>(args)), ...);

  }(std::forward<decltype(_ops)>(_ops)...);

  // Insert gate _type for _lhs
  if( ! this->ids.contains(_lhs) )
  {
    this->ids[_lhs] = this->id_counter;
    this->gate_type[this->id_counter] = _type;
    ++this->id_counter;
  }

  // Perform insertion callback
  [&_lhs,this]<typename... Args>(Args&&... args) -> void
  {
    auto link = [&_lhs,this](auto&& arg)
    {
      this->callback(std::make_pair(this->ids[arg], this->ids[_lhs]));
    };

    (link(std::forward<Args>(args)),...);

  }(std::forward<decltype(_ops)>(_ops)...);

} // fn: update }}}

// Modifiers }}}

} // namespace celaeno::graph::io::verilog }}}
