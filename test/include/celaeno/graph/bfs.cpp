// vim: set ts=2 sw=2 tw=0 et :
//
// @author      : Ruan E. Formigoni (ruanformigoni@gmail.com)
// @file        : bfs
// @created     : Wednesday Aug 14, 2019 17:21:01 -03
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

#include <tuple>
#include <catch2/catch.hpp>
#include <array>
#include <celaeno/graph/bfs.hpp>
#include <taygete/graph.hpp>
#include <taygete/graph-reader.hpp>
#include <range/v3/all.hpp>
#include "circuits.hpp"

namespace celaeno::graph::bfs::test
{

TEST_CASE("Breadth-First Search", "[bfs]")
{
  namespace rg = ranges;
  namespace rv = ranges::views;
  namespace ra = ranges::actions;

  // Use taygete data structure
  taygete::graph::Graph<int64_t> g;
  // Read circuit into a stream
  std::stringstream ss; ss << clpl;
  // Callback for populating the graph
  auto insert = [&g]<typename T>(T&& a, T&& b) -> void
    { g.emplace(std::make_pair(a,b)); };

  // Read the graph into g
  taygete::graph::reader::Reader reader(ss, insert);

  // Perform a BFS
  auto adj = [&g](auto&& v) -> auto { return g.get_adjacent(v); };
  auto cont =  [&g](auto&& v) -> bool { return false; };

  // std::cout <<
  //   (
  //     std::make_tuple(0,adj,cont)
  //     | bfs
  //     | rv::all
  //   )
  // << std::endl;
}

} // namespace celaeno::graph::bfs::test
