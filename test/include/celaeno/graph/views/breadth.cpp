//
// @author      : Ruan E. Formigoni (ruanformigoni@gmail.com)
// @file        : breadth
// @created     : Tuesday Apr 07, 2020 07:08:02 -03
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

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest/doctest.h>
#include <celaeno/graph/bfs.hpp>
#include <celaeno/graph/hooks.hpp>
#include <celaeno/graph/balance.hpp>
#include <celaeno/graph/views/depth.hpp>
#include <celaeno/graph/views/breadth.hpp>
#include <taygete/graph/graph.hpp>
#include <taygete/graph/reader.hpp>
#include <fplus/fplus.hpp>
#include <maia/circuits/iscas.hpp>
#include <maia/circuits/synth-91.hpp>
#include <asterope/graph/to-dot.hpp>
#include <asterope/graph/to-png.hpp>

namespace celaeno::graph::views::breadth::test
{
//
// Aliases
//
namespace synth = maia::circuits::synth_91;

TEST_CASE("Breadth")
{
  // Read Graph
  taygete::graph::Graph<int64_t> g;
  auto emplace = [&g](auto&& pair){ g.emplace(pair); };
  taygete::graph::reader::Reader reader(synth::decod,emplace);

  // Helpers
  auto pred = [&g](auto&& v){ return g.get_predecessors(v); };
  auto succ = [&g](auto&& v){ return g.get_successors(v); };
  auto adj = [&g](auto&& v){ return g.get_adjacent(v); };
  auto link = [&g](auto&& pair){ return g.emplace(pair); };
  auto unlink = [&g](auto&& pair){ return g.erase(pair); };
  std::string_view const constexpr color{"\033[94;1m"};

  // Manage Fan-ins and Fan-outs
  celaeno::graph::hooks::hooks(0,pred,succ,link,unlink);
  // Balance
  celaeno::graph::balance::balance(0,pred,succ,link,unlink);
  // Get cols
  auto cols{celaeno::graph::views::breadth::breadth(0,pred,succ)};

  // Print
  // auto format = [&cols](auto&& os, auto&& v1, auto&& v2)
  // {
  //   os << v1 << " -> " << v2 << std::endl;
  //   os << v1 << " [label=\"l: " << cols.at(v1).first
  //     << " c: " << cols.at(v1).second << "\"]" << std::endl;
  //   os << v2 << " [label=\"l: " << cols.at(v2).first
  //     << " c: " << cols.at(v2).second << "\"]" << std::endl;
  // };
  //
  // asterope::graph::to_png::to_png(
  //   asterope::graph::to_dot::to_dot(
  //     g.data(),
  //     format
  //   ),
  //   "column"
  // );

} // TEST_CASE: Breadth

} // namespace celaeno::graph::views::breadth::test
