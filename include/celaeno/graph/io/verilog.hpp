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

#include <fmt/ranges.h>
#include <spdlog/spdlog.h>
#include <range/v3/all.hpp>

#include <celaeno/aliases.hpp>
#include <celaeno/concepts.hpp>
#include <celaeno/err/err.hpp>
#include <celaeno/fun/fun.hpp>

#include <celaeno/graph/search/bfs.hpp>

// celaeno::graph::io::verilog {{{
namespace celaeno::graph::io::verilog
{

// Namespaces {{{
namespace err = celaeno::err;

namespace rg = ranges;
namespace rv = ranges::views;
namespace ra = ranges::actions;

namespace ns_search = celaeno::graph::search;
// }}}

// Using namespaces {{{
using namespace celaeno::fun::fn;
using namespace celaeno::concepts;
using namespace celaeno::aliases;
// }}}

// Enum GateType {{{
enum class GateType{INPUT,NOT,AND,NAND,OR,NOR,XOR,XNOR,MAJ3,DUMMY,};
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

    mutable T f_callback;
    mutable i64 id_counter;
    mutable std::map<std::string,i64> m_str_id;
    mutable std::map<i64,GateType> m_id_type;
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
  : f_callback(_callback)
  , id_counter()
  , m_str_id()
  , m_id_type()
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

  // Set non-gate m_str_id as inputs
  rg::for_each(this->m_str_id, [this](auto e)
  {
    auto id {e.second};

    if( ! this->m_id_type.contains(id) )
    {
      this->m_id_type.emplace(id,GateType::INPUT);
    } // if
  });
}
// }}}

// Element Access {{{
template<typename T>
std::map<i64,GateType> const& Reader<T>::data() const noexcept
{
  return this->m_id_type;
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
      if( ! this->m_str_id.contains(arg) )
      {
        this->m_str_id[arg] = this->id_counter++;
      }
    };

    (inc(std::forward<Args>(args)), ...);

  }(std::forward<decltype(_ops)>(_ops)...);

  // Insert gate _type for _lhs
  if( ! this->m_str_id.contains(_lhs) )
  {
    this->m_str_id[_lhs] = this->id_counter;
    this->m_id_type[this->id_counter] = _type;
    ++this->id_counter;
  }

  // Perform insertion f_callback
  [&_lhs,this]<typename... Args>(Args&&... args) -> void
  {
    auto link = [&_lhs,this](auto&& arg)
    {
      this->f_callback(std::make_pair(this->m_str_id[arg], this->m_str_id[_lhs]));
    };

    (link(std::forward<Args>(args)),...);

  }(std::forward<decltype(_ops)>(_ops)...);

} // fn: update }}}

// Modifiers }}}

// class Reader }}}

class Writer
{
  private:
    static constexpr std::string_view template_header
    {
      "module {}({},{});\n\n"
    };
    static constexpr std::string_view template_inputs
    {
      "  input {};\n"
    };
    static constexpr std::string_view template_outputs
    {
      "  output {};\n\n"
    };
    static constexpr std::string_view template_wires
    {
      "  wire {};\n\n"
    };
    static constexpr std::string_view template_assign
    {
      "  assign {} = {} {} {};\n"
    };
    static constexpr std::string_view template_footer
    {
      "\nendmodule"
    };
  public:
    Writer(std::map<i64,GateType> m_id_type
      , auto&& f_pred
      , auto&& f_succ
      , String auto&& out);
}; // class: Writer

Writer::Writer( std::map<i64,GateType> m_id_type
  , auto&& f_pred
  , auto&& f_succ
  , String auto&& out)
{
  // Read file
  std::ofstream ofile{out};

  // Check for erros
  err::err({ ofile.good() })("Error to open file {}", out);

  // Get all nodes through bfs
  auto bfs{ns_search::bfs::run(0,f_pred,f_succ)};

  // Save inputs/outputs
  std::set<std::string> inputs;
  std::set<std::string> outputs;
  std::set<std::string> wires;

  // Include a prefix in the node id
  auto f_prefix = [](i64 u)
  {
    return (u < 0)? fmt::format("dummy_{}",std::abs(u))
      : fmt::format("node_{}",u);
  };

  // Remove inputs from processing, save io to create input/output statements
  // Transform each node into a range of assignments
  auto assignments{fn(bfs)
    .keep([&](i64 node)
    {
      if(f_pred(node).size() == 0)
      {
        inputs.insert(f_prefix(node));
        // Ignore inputs
        return false;
      } // if
      else if(f_succ(node).size() == 0)
      {
        outputs.insert(f_prefix(node));
      } // else if
      else
      {
        wires.insert(f_prefix(node));
      } // else

      return true;
    })
    .as([&,this](i64 node)
    {
      std::string op;

      if( ! m_id_type.contains(node) )
      {
        m_id_type[node] = GateType::DUMMY;
      } // if

      switch(m_id_type.at(node))
      {
        case GateType::AND: op = "&"; break;
        case GateType::OR: op = "|"; break;
        case GateType::NAND: op = "~&"; break;
        case GateType::NOR: op = "~|"; break;
        case GateType::XOR: op = "^"; break;
        case GateType::XNOR: op = "~^"; break;
        case GateType::DUMMY: op = "&"; break;
        case GateType::NOT: op = "~"; break;
        default:
          err::err()("Operation not supported for node {}", node);
      } // switch

      auto preds{f_pred(node)};

      if( preds.size() == 2)
      {
        return fmt::format(this->template_assign
          , f_prefix(node)
          , f_prefix(preds.at(0))
          , op
          , f_prefix(preds.at(1))
        );
      } // if
      else
      {
        return fmt::format(this->template_assign
          , f_prefix(node)
          , f_prefix(preds.at(0))
          , ""
          , ""
        );
      } // else


      // return fmt::format(this->assign,);
    })
    .vec()
  };

  // I/O in module
  ofile << fmt::format(template_header
    , out
    , fmt::join(inputs, ",")
    , fmt::join(outputs, ",")
  );

  // I/O declarations
  ofile << fmt::format(template_inputs, fmt::join(inputs, ","));
  ofile << fmt::format(template_outputs, fmt::join(outputs, ","));
  ofile << fmt::format(template_wires, fmt::join(wires, ","));

  // Assignments
  for (auto&& e : assignments)
  {
    ofile << fmt::format(e);
  } // for

  // Footer
  ofile << fmt::format(template_footer);

  ofile.close();
}

} // namespace celaeno::graph::io::verilog }}}
