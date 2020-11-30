// vim: set expandtab fdm=marker ts=2 sw=2 tw=100 et :
//
// @author      : Ruan E. Formigoni (ruanformigoni@gmail.com)
// @file        : bfs
// @created     : Monday Apr 06, 2020 09:06:59 -03
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

#include <taygete/graph/graph.hpp>
#include <taygete/graph/reader.hpp>
#include <celaeno/aliases.hpp>
#include <celaeno/concepts.hpp>
#include <celaeno/test/test.hpp>
#include <celaeno/test/graph/test.hpp>
#include <celaeno/graph/search/bfs.hpp>

// namespace celaeno::graph::search::bfs::test {{{

namespace celaeno::graph::search::bfs::test
{

// Macros {{{
#define assertm(exp, msg) assert(((void)msg, exp))
// }}}

// using namespaces {{{
using namespace celaeno::concepts;
// }}}

// Namespaces {{{
namespace graph = taygete::graph;
namespace reader = taygete::graph::reader;
namespace bfs = celaeno::graph::search::bfs;
// }}}

// Test Case: celaeno::graph::search::bfs {{{
TEST_CASE("celaeno::graph::search::bfs"
  * doctest::timeout(10.0f)
)
{
  // Logger {{{
  celaeno::test::logger_new({
      .name="celaeno::graph::search::bfs",
      .path="logs/celaeno/graph/search/bfs.csv",
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

    // Test bfs {{{
    auto f_p = [&g](auto&& u){ return g.predecessors(u); };
    auto f_s = [&g](auto&& u){ return g.successors(u); };
    auto [result,runtime] = celaeno::test::runtime([&]{ return bfs::run(0,f_p,f_s); });
    // }}}

    // Perform checks {{{
    auto f_unique = [](auto&& c){ return std::set(c.begin(),c.end()); };
    celaeno::test::check(
      // Check vertices count with bfs size
      ( g.vertices_count() == result.size() ),
      // Check if there are no duplicates
      ( f_unique(result).size() == result.size() )
    );
    // }}}

    // Log results {{{
    celaeno::test::logger_write("{},{},{}", g.vertices_count(), g.edges_count(), runtime);
    // }}}

  }; // lamb: test }}}

  // Perform tests {{{
  celaeno::graph::test::run(test);
  // }}}

} // TEST_CASE: celaeno::graph::search::bfs }}}

} // namespace celaeno::graph::search::bfs::test }}}
