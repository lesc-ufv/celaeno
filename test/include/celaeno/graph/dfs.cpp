//
// @author      : Ruan E. Formigoni (ruanformigoni@gmail.com)
// @file        : dfs
// @created     : Wednesday Mar 18, 2020 16:02:20 -03
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

#define CATCH_CONFIG_MAIN

#include <sstream>
#include <catch2/catch.hpp>
#include <taygete/graph.hpp>
#include <taygete/graph-reader.hpp>
#include <asterope/graph/dot-parser.hpp>
#include <asterope/graph/to-png.hpp>
#include <celaeno/graph/dfs.hpp>
#include "circuits.hpp"
#include <iostream> // TODO remove

namespace celaeno::graph::dfs::test
{

template<typename Str>
void run(Str&& graph)
{
  //
  // Read the graph
  //
  taygete::graph::Graph<int64_t> g;
  auto callback = [&g]<typename T>(T&& v1, T&& v2) -> void
    { g.emplace(std::make_pair(std::forward<T>(v1),std::forward<T>(v2))); };
  std::stringstream ss; ss << graph;
  taygete::graph::reader::Reader reader(ss, callback);

  //
  // Perform DFS
  //
  auto get_neighbors = [&g]<typename T>(T&& v)
  {
    auto&& pred {g.get_predecessors(std::forward<T>(v))};
    auto&& succ {g.get_successors(std::forward<T>(v))};
    pred.insert(pred.end(), succ.cbegin(), succ.cend());
    return pred;
  };

  // asterope::graph::to_png::to_png(
  //   asterope::graph::dot_parser::dot_parser(g.data()),
  //   "graph"
  // );

  celaeno::graph::dfs::dfs(
    0,
    get_neighbors,
    [](auto&& v){ return false; }
  );
}

TEST_CASE("Depth-First Search","[dfs.hpp]")
{
  SECTION("newtag Graph")
  {
    run(newtag);
  } // SECTION: newtag Graph

} // TEST_CASE: Depth-First Search

} // namespace celaeno::graph::dfs::test
