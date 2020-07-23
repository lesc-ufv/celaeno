//
// @author      : Ruan E. Formigoni (ruanformigoni@gmail.com)
// @file        : minimize-crossings
// @created     : quarta jun 24, 2020 11:56:38 -03
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
#include <random>
#include <doctest/doctest.h>
#include <celaeno/graph/minimize-crossings.hpp>
#include <celaeno/graph/matrix-realization.hpp>
#include <celaeno/graph/views/depth.hpp>
#include <taygete/graph/graph.hpp>
#include <range/v3/all.hpp>
#include <fplus/fplus.hpp>

namespace celaeno::graph::minimize_crossings::test
{
  //
  // Aliases
  //
  namespace minimize_crossings = celaeno::graph::minimize_crossings;
  namespace graph = taygete::graph;
  namespace depth = celaeno::graph::views::depth;
  namespace fp = fplus;
  namespace fw = fplus::fwd;
  namespace rv = ranges::views;

  TEST_CASE("celaeno::graph::minimize_crossings")
  {

    graph::Graph<int64_t> g
    {
      {1,5},{1,6},{2,5},{2,8},{2,9},{3,6},{3,8},
      {3,9},{4,5},{4,7},{4,9}
    };

    // Create hierarchical graph
    auto pred = [&g](auto&& v){ return g.predecessors(v); };
    auto succ = [&g](auto&& v){ return g.successors(v); };
    auto [h,_] = depth::run(1,pred,succ);

    // Get the key type
    using node_t = decltype(h)::key_type;

    // Lambda to obtain a layer by index
    auto get_layer = [&h](node_t idx)
    {
      return fw::apply(h
        , fw::drop_if([&idx](auto&& e){ return e.first != idx; })
        , fw::get_map_values()
        , fw::sort()
      );
    };

    // Lambda to verify if an edge between v → u exists
    auto has_edge = [&g](node_t v, node_t u){ return g.adjacent(v,u); };

    // Lambda to get the depth of the graph
    auto depth {fw::apply(h,fw::get_map_keys(),fw::unique(),fw::size_of_cont())};

    // Matrix realization of the graph
    auto matrices {matrix_realization::run(get_layer, has_edge, depth)};

    SUBCASE("Test vertices as matrices labels")
    {
      auto ordering{minimize_crossings::run(matrices, get_layer, depth)};
      for (auto const& i : ordering)
      {
        std::cout << rv::all(i) << std::endl;
      } // for i : ordering

    } // SUBCASE: "Test vertices as matrices labels"

  } // TEST_CASE: "celaeno::graph::minimize_crossings"

} // namespace celaeno::graph::minimize_crossings::test
