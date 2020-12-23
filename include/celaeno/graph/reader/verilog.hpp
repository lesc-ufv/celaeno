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

#include <spdlog/spdlog.h>
#include <range/v3/all.hpp>

#include <celaeno/aliases.hpp>
#include <celaeno/concepts.hpp>

// celaeno::graph::reader::verilog {{{
namespace celaeno::graph::reader::verilog
{

// Namespaces {{{
namespace rv = ranges::views;
namespace ra = ranges::actions;
// }}}

// Using namespaces {{{
using namespace celaeno::concepts;
using namespace celaeno::aliases;
// }}}

// Enum Type {{{
enum class GateType{AND,NAND,OR,NOR,XOR,XNOR,MAJ3,};
// }}}

// Parsing error exception {{{
class ParseError : std::exception
{
  private:
    std::string const error_msg;

  public:
    template<typename T>
    ParseError(T&& error_msg)
      : error_msg(std::forward<T>(error_msg))
    {}
    virtual const char* what() const noexcept override
    {
      return this->error_msg.c_str();
    };
}; // class: ParseError : std::exception }}}

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
    Reader(S&& input, T callback);
  // }}}

  // Element Access {{{
    std::map<i64,GateType> const& data() const noexcept;
  // }}}

  // Private Methods {{{
  private:
    // Modifiers {{{
    bool on_assign( std::string const& line ) const;
    bool on_and( std::string const& line ) const;
    bool on_or( std::string const& line ) const;
    void update(auto&& lhs, GateType const& type, auto&&... ops) const;
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
Reader<T>::Reader(S&& input, T callback)
  : callback(callback)
  , id_counter(0)
{

#ifndef NDEBUG
  spdlog::set_level(spdlog::level::debug);
#endif

  auto eval_regex = [&](auto&& line, auto&& pattern, auto&& msg)
  {
    std::regex r{pattern};

    std::smatch m;

    if( std::regex_match(line,m,r) )
    {
      spdlog::debug(msg);
      return true;
    }
    return false;
  };

  for(auto rng : input
    | rv::filter([](auto c){ return c != ' ' && c != '\n' && c != '\t';  })
    | ra::split(';')
  )
  {
    std::string line(rng.begin(),rng.end());

    // Newline
    if (not (eval_regex(line,"^$","Newline")
    // Comment
    || eval_regex(line,"^//.*$",fmt::format("Comment: {}",line))
    // Module
    || eval_regex(line,
        fmt::format("module{}.*",expr_id),
        fmt::format("Module: {}",line))
    // Endmodule
    || eval_regex(line,"endmodule","Endmodule")
    // Inputs/Outputs
    || eval_regex(line,
        fmt::format("((input)|(output))({}[,;]?)+",expr_id),
        fmt::format("IO: {}",line))
    // Wire declarations
    || eval_regex(line,
            fmt::format("(wire)?({}[,;]?)+",expr_id),
            fmt::format("Wire: {}",line))
    // Assigns
    || this->on_assign(line)
    // And gates
    || this->on_and(line)
    // Or gates
    || this->on_or(line))
    // Error on parsing
    ){
      spdlog::error(fmt::format("Did not match: {}\n",line));
      throw ParseError(fmt::format("Error on parsing expression: {}", line));
    }
  }
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
template<typename T>
bool Reader<T>::on_assign( std::string const& line ) const
{
  std::regex r{fmt::format(expr_assign, expr_id)};

  std::smatch m;
  if( std::regex_match(line,m,r) )
  {
    spdlog::debug(fmt::format("Assign: ({}) Input: ({})",
      std::string(m[1]), std::string(m[2])));
    return true;
  }
  return false;
}

template<typename T>
bool Reader<T>::on_and( std::string const& line ) const
{
  std::regex r{fmt::format(expr_and, expr_id, "&")};

  std::smatch m;
  if( std::regex_match(line,m,r) )
  {
    spdlog::debug(fmt::format("Gate: ({}) Inputs: ({}) & ({})",
      std::string(m[1]), std::string(m[2]), std::string(m[3])));
    this->update(m[1], GateType::AND, m[2], m[3]);
    return true;
  }
  return false;
}

template<typename T>
bool Reader<T>::on_or( std::string const& line ) const
{
  std::regex r{fmt::format(expr_or, expr_id, "\\|")};

  std::smatch m;
  if( std::regex_match(line,m,r) )
  {
    spdlog::debug(fmt::format("Gate: ({}) Inputs: ({}) | ({})",
      std::string(m[1]), std::string(m[2]), std::string(m[3])));
    this->update(m[1], GateType::OR, m[2], m[3]);
    return true;
  }
  return false;
}

template<typename T>
void Reader<T>::update(auto&& lhs, GateType const& type, auto&&... ops) const
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

  }(std::forward<decltype(ops)>(ops)...);

  // Insert gate type for lhs
  if( ! this->ids.contains(lhs) )
  {
    this->ids[lhs] = this->id_counter++;
    this->gate_type[this->id_counter] = type;
  }

  // Perform insertion callback
  [&lhs,this]<typename... Args>(Args&&... args) -> void
  {
    auto link = [&lhs,this](auto&& arg)
    {
      this->callback(std::make_pair(this->ids[arg], this->ids[lhs]));
    };

    (link(std::forward<Args>(args)),...);

  }(std::forward<decltype(ops)>(ops)...);

} // }}}

} // namespace celaeno::graph::reader::verilog }}}
