// vim: set expandtab fdm=marker ts=2 sw=2 tw=100 et :
//
// @author      : Ruan E. Formigoni (ruanformigoni@gmail.com)
// @file        : crossings
// @created     : segunda jun 22, 2020 18:48:04 -03
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

#include <fplus/fplus.hpp>

#include <taygete/graph/graph.hpp>
#include <taygete/graph/reader.hpp>

#include <celaeno/aliases.hpp>
#include <celaeno/concepts.hpp>
#include <celaeno/test/test.hpp>
#include <celaeno/test/graph/test.hpp>
#include <celaeno/graph/views/depth.hpp>
#include <celaeno/graph/operations/balance/paths.hpp>
#include <celaeno/graph/operations/count/crossings.hpp>

// namespace celaeno::graph::operations::count::crossings::test {{{

namespace celaeno::graph::operations::count::crossings::test
{

// Macros {{{
#define assertm(exp, msg) assert(((void)msg, exp))
// }}}

// namespaces {{{
namespace fp = fplus;
namespace fw = fplus::fwd;

namespace graph = taygete::graph;
namespace reader = taygete::graph::reader;

namespace depth = celaeno::graph::views::depth;
namespace balance = celaeno::graph::operations::balance::paths;
namespace count = celaeno::graph::operations::count::crossings;
// }}}

// Tests case celaeno::graph::crossings {{{

TEST_CASE("celaeno::graph::operations::count::crossings")
{

  // Subcase: Empiric time testing {{{
  SUBCASE("Subcase: Empiric time testing")
  {

    // Logger {{{
    celaeno::test::logger_new({
        .name="celaeno::graph::operations::count::crossings",
        .path="logs/celaeno/graph/operations/count/crossings.csv",
        .info="date,time,vertices,edges,runtime",
        .pattern="%d/%m/%Y,%T,%v"
    });
    // }}}

    // test lambda {{{
    auto test = [&]<String S>(S const& str)
    {
      // Read graph {{{
      graph::Graph<i64> g;
      auto emplace = [&g](auto&& e) -> void { g.emplace(e); };
      reader::Reader{str,emplace};
      // }}}

      // vertices_count > 0 {{{
      assertm(g.vertices_count() > 0, "Empty input graph");
      // }}}

      // Test {{{
      // Helpers
      auto f_pred = [&g](auto&& v){ return g.predecessors(v); };
      auto f_succ = [&g](auto&& v){ return g.successors(v); };
      auto f_link = [&g](auto&& e){ g.emplace(e); };
      auto f_unlink = [&g](auto&& e){ g.erase(e); };
      // Balance graph
      balance::run(0,f_pred,f_succ,f_link,f_unlink);
      // Create depth-view
      auto depth_view{depth::run(1,f_pred,f_succ).first};
      // Create overlapping pairs of layers
      auto levels_to_compute {fp::overlapping_pairs(fp::numbers({},depth_view.size()))};
      // Exec and log runtime
      auto runtime{celaeno::test::runtime_vt([&]
      {
        for (auto const& [a,b] : levels_to_compute)
        {
          count::run(depth_view.at(a),depth_view.at(b),f_succ);
        } // for
      })};
      // }}}

      // Log results {{{
      celaeno::test::logger_write("{},{},{}", g.vertices_count(), g.edges_count(), runtime);
      // }}}

    }; // lamb: test }}}

    // Perform tests {{{
    celaeno::graph::test::run(test);
    // }}}

  } // SUBCASE: "Subcase: Empiric time testing" }}}

  // Subcase: "Edge cases" {{{
  SUBCASE("Edge cases")
  {
    // Graph {{{
    graph::Graph<i64> g
    {
      {6,-1},{-1,8},{2,4},{3,4},{1,5},
      {4,7},{5,7},{5,8},{5,9},
      {7,12},{8,10},{8,11},
    };
    // }}}

    // vertices_count > 0 {{{
    assertm(g.vertices_count() > 0, "Empty input graph");
    // }}}

    // Helpers {{{
    auto f_p = [&g](auto&& v){ return g.predecessors(v); };
    auto f_s = [&g](auto&& v){ return g.successors(v); };
    // }}}

    // Depth view {{{
    auto depth_view{depth::run(1,f_p,f_s).first};
    // }}}

    // Overlapping level id pairs {{{
    auto levels_to_compute {fp::overlapping_pairs(fp::numbers({},depth_view.size()))};
    // }}}

    // No crossings
    // lamb: t1 {{{
    auto t1 = [=](auto view)
    {
      CHECK(count::run(view.at(0),view.at(1),f_s) == 0);
    }; // lamb: t1 }}}


    // Induced 1 crossing
    // lamb: t2 {{{
    auto t2 = [=](auto view)
    {
      view[0] = {2,1,3,6};
      // Test count crossings {{{
      CHECK(count::run(view.at(0),view.at(1),f_s) == 1);
      // }}}

    }; // lamb: t2 }}}

    // Induced 2 crossings
    // lamb: t3 {{{
    auto t3 = [=](auto view)
    {
      view[0] = {2,1,6,3};
      // Test count crossings {{{
      CHECK(count::run(view.at(0),view.at(1),f_s) == 2);
      // }}}

    }; // lamb: t3 }}}

    // All layers crossings
    // lamb: t4 {{{
    auto t4 = [=](auto view)
    {
      // Modify depth-view {{{
      view[0] = {2,1,6,3};
      view[3] = {10,12,11};
      // }}}

      // Test count crossings {{{
      CHECK(fw::apply(levels_to_compute
            , fw::transform([&](auto e){ return count::run(view.at(e.first),view.at(e.second),f_s); })
            , fw::sum() ) == 4 );
      // }}}

    }; // lamb: t4 }}}

    t1(depth_view);
    t2(depth_view);
    t3(depth_view);
    t4(depth_view);

  } // SUBCASE: "Edge cases" }}}

} // TEST_CASE: "celaeno::graph::operations::count::crossings" }}}

} // namespace celaeno::graph::operations::count::crossings::test }}}
