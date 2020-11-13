// vim: set expandtab fdm=marker ts=2 sw=2 tw=100 et :
//
// @author      : Ruan E. Formigoni (ruanformigoni@gmail.com)
// @file        : paths
// @created     : Wednesday Apr 01, 2020 21:24:23 -03
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

#include <range/v3/all.hpp>
#include <celaeno/aliases.hpp>
#include <celaeno/concepts.hpp>
#include <celaeno/test/test.hpp>
#include <celaeno/test/graph/test.hpp>
#include <celaeno/graph/views/depth.hpp>
#include <celaeno/graph/operations/balance/paths.hpp>
#include <taygete/graph/graph.hpp>
#include <taygete/graph/reader/verilog.hpp>

// namespace celaeno::graph::operations::balance::paths::test {{{

namespace celaeno::graph::operations::balance::paths::test
{

// Macros {{{
#define assertm(exp, msg) assert(((void)msg, exp))
// }}}

// using namespaces {{{
using namespace celaeno::concepts;
// }}}


// namespaces {{{
namespace rg = ranges;
namespace rv = ranges::views;
namespace graph = taygete::graph;
namespace reader = taygete::graph::reader::verilog;
namespace cir = maia::circuits;
namespace balance = celaeno::graph::operations::balance::paths;
namespace depth = celaeno::graph::views::depth;
// }}}

// Test Cases {{{

TEST_CASE("celaeno::graph::operations::balance::paths"
  * doctest::timeout(1000.0f)
)
{
  // Logger {{{
  celaeno::test::logger_new({
      .name="celaeno::graph::operations::balance::paths",
      .path="logs/celaeno/graph/operations/balance/paths.csv",
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

    // Test Balance {{{
    auto p = [&g](auto&& v){ return g.predecessors(v); };
    auto s = [&g](auto&& v){ return g.successors(v); };
    auto l = [&g](auto&& e){ g.emplace(e); };
    auto u = [&g](auto&& e){ g.erase(e); };
    auto runtime {celaeno::test::runtime_vt([&]{ balance::run(0,p,s,l,u); })};
    // }}}

    // More tests {{{
    // * Given a depth-view, each vertex must have a distance of one
    // * to its successor or predecessor
    auto [lv,vl] {depth::run(0,p,s)};

    // Get the levels
    auto levels { rv::transform(lv,[](auto&& _u){ return _u.first; }) };

    // Get the vertices on level
    for(auto&& level : levels)
    {
      // For each vertex on level
      for(auto it{lv.at(level).begin()}; it!=lv.at(level).end(); ++it)
      {
        // The adjacent vertices
        auto adj {g.neighbors(*it)};
        // Verify if distance is one to each
        auto is_dist_one = [&,vl = std::ref(vl)](auto&& a) -> void
        {
          celaeno::test::check(std::abs(vl.get().at(a) - vl.get().at(*it)) == 1);
        };
        // Execute tests
        rg::for_each(adj, is_dist_one);
      } // for
    } // for
    // }}}

    // Log results {{{
    celaeno::test::logger_write("{},{},{}", g.vertices_count(), g.edges_count(), runtime);
    // }}}

  }; // lamb: test }}}

  // Perform tests {{{
  celaeno::graph::test::run(test);
  // }}}

} // TEST_CASE: celaeno::graph::operations::balance::paths }}}

} // namespace celaeno::graph::operations::balance::paths::test }}}
