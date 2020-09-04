// vim: set expandtab fdm=marker ts=2 sw=2 tw=100 et :
//
// @author      : Ruan E. Formigoni (ruanformigoni@gmail.com)
// @file        : kahn
// @created     : Wednesday Apr 08, 2020 13:22:58 -03
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
#include <taygete/graph/reader/verilog.hpp>
#include <celaeno/aliases.hpp>
#include <celaeno/concepts.hpp>
#include <celaeno/test/test.hpp>
#include <celaeno/test/graph/test.hpp>
#include <celaeno/graph/search/kahn.hpp>

// namespace celaeno::graph::search::kahn::test {{{

namespace celaeno::graph::search::kahn::test
{

// Macros {{{
#define assertm(exp, msg) assert(((void)msg, exp))
// }}}

// using namespaces {{{
using namespace celaeno::concepts;
// }}}

// namespaces {{{
namespace fw = fplus::fwd;
namespace graph = taygete::graph;
namespace reader = taygete::graph::reader::verilog;
namespace topo = celaeno::graph::search::kahn;
// }}}

// Test Case celaeno::graph::search::kahn {{{
TEST_CASE("celaeno::graph::search::kahn"
  * doctest::timeout(10.0f)
)
{
  // Logger {{{
  celaeno::test::logger_new({
      .name="celaeno::graph::search::Kahn",
      .path="logs/celaeno/graph/search/kahn.csv",
      .info="date,time,vertices,edges,runtime",
      .pattern="%d/%m/%Y,%T,%v"
  });
  // }}}

  // test lambda {{{
  auto test = [&]<String S>(S&& str)
  {
    // Read Graph {{{
    graph::Graph<int64_t> g;
    auto emplace = [&g](auto&& pair){ g.emplace(pair); };
    reader::Reader{str,emplace};
    // }}}

    // vertices_count > 0 {{{
    assertm(g.vertices_count() > 0, "Empty input graph");
    // }}}

    // Test Kahn {{{
    auto pred = [&g](auto&& v){ return g.predecessors(v); };
    auto succ = [&g](auto&& v){ return g.successors(v); };
    auto [result,runtime] = celaeno::test::runtime([&](){ return kahn::run(0,pred,succ); });
    // }}}

    // Perform checks {{{
    celaeno::test::check(
      // Check vertices count with bfs size
      (g.vertices_count() == result.size()),
      // Check if there are no duplicates
      fw::apply(result,fw::unique()).size() == result.size()
    );
    // }}}

    // Log results {{{
    celaeno::test::logger_write("{},{},{}", g.vertices_count(), g.edges_count(), runtime);
    // }}}

  }; // lamb: test }}}

  // Perform tests {{{
  celaeno::graph::test::run(test);
  // }}}

} // TEST_CASE: celaeno::graph::search::kahn }}}

} // namespace celaeno::graph::search::kahn::test }}}
