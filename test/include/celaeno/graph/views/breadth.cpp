//
// @author      : Ruan E. Formigoni (ruanformigoni@gmail.com)
// @file        : breadth
// @created     : Tuesday Mar 24, 2020 07:47:56 -03
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

#include <catch2/catch.hpp>
#include <taygete/graph.hpp>
#include <taygete/graph-reader.hpp>
#include <asterope/graph/to-dot.hpp>
#include <asterope/graph/to-png.hpp>
#include <celaeno/graph/views/breadth.hpp>
#include "../circuits.hpp"
#include <iostream> // TODO remove
#include <celaeno/graph/views/depth.hpp> // TODO Remove
#include <celaeno/graph/balance.hpp>    // TODO Remove

namespace celaeno::graph::views::breadth::test
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
  // Balance the graph
  //
  auto g_views
  {
    celaeno::graph::views::depth::depth(
      0,
      [&g](auto&& e){ return g.get_successors(std::forward<decltype(e)>(e)); },
      [&g](auto&& e){ return g.get_predecessors(std::forward<decltype(e)>(e)); }
    )
  };
  auto& d_view{g_views.first};

  int64_t depth{};
  if( ! d_view.empty() )
  {
    depth = ( d_view.rbegin()->first ) + 1;
  }
  else
  {
    std::cerr << " * Empty map" << std::endl;
    exit(1);
  }

  // Balace the graph
  celaeno::graph::balance::balance(
    d_view,
    g, depth,
    [&g]<typename T>(T&& e){return g.get_predecessors(std::forward<T>(e));}
  );

  //
  // Create breadth view
  //
  auto pred = [&g]<typename T>(T&& v)
    {return g.get_predecessors(std::forward<T>(v));};
  auto succ = [&g]<typename T>(T&& v)
    {return g.get_successors(std::forward<T>(v));};

  auto view {celaeno::graph::views::breadth::breadth(
      14,
      celaeno::graph::views::depth::depth(
          0,
          [&g](auto&& e){ return g.get_successors(std::forward<decltype(e)>(e)); },
          [&g](auto&& e){ return g.get_predecessors(std::forward<decltype(e)>(e)); }
        ).second,
      pred,
      succ
    )
  };

  asterope::graph::to_png::to_png(
    asterope::graph::to_dot::to_dot(g.data(),
      [&view](auto&& os, auto&& k, auto& v)
        {
          if ( view.contains(k) )
          {
            os << k << " [label=\"Node: " << k << " Column: " << view[k] << "\"]" << std::endl;
          } // if
          if ( view.contains(v) )
          {
            os << v << " [label=\"Node: " << v << " Column: " << view[v] << "\"]" << std::endl;
          } // if
          os << k << " -> " << v << std::endl;
        }
    ),
    "graph"
  );

} // run

TEST_CASE("breadth","[breadth]")
{
  SECTION("xor5_r")
  {
    run(newtag);
  } // TEST_CASE: xor5_r
} // TEST_CASE: breadth

} // namespace celaeno::graph::views::breadth::test
