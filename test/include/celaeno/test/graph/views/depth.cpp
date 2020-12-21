// vim: set expandtab fdm=marker ts=2 sw=2 tw=80 et :
//
// @author      : Ruan E. Formigoni (ruanformigoni@gmail.com)
// @file        : depth
// @created     : Wednesday Apr 08, 2020 16:04:13 -03
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

#include <vector>

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest/doctest.h>

#include <celaeno/aliases.hpp>
#include <celaeno/concepts.hpp>
#include <celaeno/test/test.hpp>
#include <celaeno/test/graph/test.hpp>
#include <celaeno/graph/views/depth.hpp>
#include <celaeno/graph/graph.hpp>
#include <celaeno/graph/reader/verilog.hpp>


// namespace celaeno::graph::views::depth::test {{{
namespace celaeno::graph::views::depth::test
{

// Macros {{{
#define assertm(exp, msg) assert(((void)msg, exp))
// }}}

// using namespaces {{{
using namespace celaeno::concepts;
// }}}

// Namespaces {{{
namespace graph = celaeno::graph;
namespace reader = celaeno::graph::reader::verilog;
namespace depth = celaeno::graph::views::depth;
// }}}

// test case celaeno::graph::views::depth  {{{

TEST_CASE("celaeno::graph::views::depth")
{
  // Logger {{{
  celaeno::test::logger_new({
      .name="graph::views::depth",
      .path="logs/celaeno/graph/views/depth.csv",
      .info="date,time,vertices,edges,runtime",
      .pattern="%d/%m/%Y,%T,%v"
  });
  // }}}

  // test lambda {{{
  auto test = [&]<String S>(S&& str)
  {
    // Read Graph {{{
    graph::Graph<i64> g;
    auto emplace = [&g](auto&& pair){ g.emplace(pair); };
    reader::Reader{str,emplace};
    // }}}

    // Create depth view {{{
    auto pred = [&g](auto&& v){ return g.predecessors(v); };
    auto succ = [&g](auto&& v){ return g.successors(v); };
    auto [result,runtime] = celaeno::test::runtime([&]{ return depth::run(0, pred, succ); });
    // }}}


    // Perform checks {{{
    // TODO
    // }}}

    // Log results {{{
    celaeno::test::logger_write("{},{},{}", g.vertices_count(), g.edges_count(), runtime);
    // }}}

  }; // lamb: test }}}

  // Perform tests {{{
  celaeno::graph::test::run(test);
  // }}}

} // TEST_CASE: celaeno::graph::views::depth }}}


} // namespace celaeno::graph::views::depth::test }}}
