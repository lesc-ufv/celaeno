// vim: set expandtab fdm=marker ts=2 sw=2 tw=80 et :
//
// @author      : Ruan E. Formigoni (ruanformigoni@gmail.com)
// @file        : dot
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
#include <fstream>

#include <fmt/ranges.h>
#include <spdlog/spdlog.h>
#include <range/v3/all.hpp>

#include <celaeno/aliases.hpp>
#include <celaeno/concepts.hpp>
#include <celaeno/log/log.hpp>
#include <celaeno/fun/fun.hpp>

#include <celaeno/graph/graph.hpp>

// celaeno::graph::io::dot
namespace celaeno::graph::io::dot
{

namespace
{

// Namespaces
namespace ns_log = celaeno::log;

// Using namespaces
using namespace celaeno::fun::fn;
using namespace celaeno::concepts;
using namespace celaeno::aliases;

}

// class Writer {{{
class Writer
{
  public:
    Writer(auto&& view
      , celaeno::graph::Ops const& ops
      , String auto&& out
      , std::optional<std::map<i64,std::pair<i64,i64>>> map_node_position = std::nullopt);
}; // class: Writer }}}

// Writer::Writer {{{
Writer::Writer(auto&& view
  , celaeno::graph::Ops const& ops
  , String auto&& out
  , std::optional<std::map<i64,std::pair<i64,i64>>> map_node_position)
{
  [[maybe_unused]] ns_log::Timer timer("celaeno::graph::io::dimacs::Writer");

  // Read file
  std::ofstream ofile{out};

  // Check for errors
  log::err({ ofile.good() })("Error to open file {}", out);

  // Write header
  ofile << "digraph {\n";
  ofile << "rankdir = BT;\n";

  if ( map_node_position )
  {
    // Write positions
    for(auto&& [node,position] : *map_node_position)
    {
      ofile << fmt::format("{} [pos=\"{},{}!\"]\n", node, position.first*100, position.second*100);
      ofile << fmt::format("{} [label=\"{}\\n{}x{}\"]\n", node, node, position.first, position.second);
      ofile << fmt::format("{} [color=red]\n", node);
    } // for
    // Write nodes
    for (auto&& [layer,nodes] : view.ln)
    {
      for(auto node : nodes)
      {
        for(auto succ : ops.succs(node))
        {
          if ( map_node_position->contains(node) and map_node_position->contains(succ) )
          {
            ofile << fmt::format("{} -> {}\n", node, succ);
          } // if
        }
      } // for
    } // for
  } // if
  else
  {
    // Write edges
    for (auto&& [layer,nodes] : view.ln)
    {
      ofile << "{\nrank = same;\n";
      ofile << "edge[ style=invis];\n";
      for(auto node : nodes)
      {
        if ( node == nodes.back() )
        {
          ofile << fmt::format("{}", node);
        } // if
        else
        {
          ofile << fmt::format("{} -> ", node);
        } // else
      } // for
      ofile << "\n}\n";
    } // for

    for (auto&& [layer,nodes] : view.ln)
    {
      for(auto node : nodes)
      {
        for(auto succ : ops.succs(node))
        {
          ofile << fmt::format("{} -> {}\n", node, succ);
        }
      } // for
    } // for
  } // if

  // Write footer
  ofile << "}\n";

  ofile.close();
} // }}}

} // namespace celaeno::graph::io::dot
