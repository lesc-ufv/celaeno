// vim: set expandtab fdm=marker ts=2 sw=2 tw=100 et :
//
// @author      : Ruan E. Formigoni (ruanformigoni@gmail.com)
// @file        : proximity
// @created     : sexta jul 31, 2020 02:33:31 -03
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

#include <ranges>

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest/doctest.h>

#include <celaeno/aliases.hpp>
#include <celaeno/concepts.hpp>
#include <celaeno/test/test.hpp>
#include <celaeno/test/graph/test.hpp>
#include <celaeno/graph/views/proximity.hpp>
#include <celaeno/graph/operations/balance/paths.hpp>

#include <celaeno/graph/graph.hpp>
#include <celaeno/graph/reader/verilog.hpp>

// namespace celaeno::graph::view::proximity::test {{{

namespace celaeno::graph::view::proximity::test
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
namespace proximity = celaeno::graph::views::proximity;
// }}}

TEST_CASE("celaeno::graph::view::proximity"
  * doctest::description("Graph proximity view test")
  * doctest::timeout(100.0f)
)
{

  // Logger {{{
  celaeno::test::logger_new({
      .name="graph::views::proximity",
      .path="logs/celaeno/graph/views/proximity.csv",
      .info="date,time,vertices,edges,runtime",
      .pattern="%d/%m/%Y,%T,%v"
  });
  // }}}

  // test_lambda {{{
  auto test = [&]<String S>(S&& str)
  {
    //  Read graph {{{
    graph::Graph<i64> g;
    auto emplace = [&g](auto&& e) -> void { g.emplace(e); };
    reader::Reader{str,emplace};
    // }}}

    // Execute proximity view algorithm {{{
    auto f_p = [&g](auto&& v){ return g.predecessors(v); };
    auto f_s = [&g](auto&& v){ return g.successors(v); };
    auto f_l = [&g](auto&& u){ return g.emplace(u); };
    auto f_u = [&g](auto&& u){ return g.erase(u); };

    celaeno::graph::operations::balance::paths::run(0,f_p,f_s,f_l,f_u);

    auto [result,runtime] = celaeno::test::runtime([&]{ return proximity::run(0, f_p, f_s); });
    auto&& vl{result.second};
    // }}}

    // Perform tests {{{
    for (auto&& [v,l] : vl)
    {
      // If has successors get the minimum level between all of them
      if( auto successors = f_s(v); ! successors.empty() )
      {

        // Store levels
        std::vector<decltype(l)> levels;

        // Populate levels
        std::ranges::transform(successors, std::back_inserter(levels),
          [&vl](auto&& s){ return vl.at(s); }
        );

        // Sort to find minimum (only finds the first minimum)
        std::ranges::sort(levels);

        // Get minimum value
        auto minimum {std::ranges::min(levels)};

        // Check distance of v and topologically nearest successor is one
        CHECK( minimum-l == 1 );
      } // if: ! successors.empty()
    } // for [v,l] : vl
    // }}}

    // Log results {{{
    celaeno::test::logger_write("{},{},{}", g.vertices_count(), g.edges_count(), runtime);
    // }}}
  }; // lamb: test }}}

  // Perform tests {{{
  celaeno::graph::test::run(test);
  // }}}

} // TEST_CASE: "celaeno::graph::view::proximity"

} // namespace celaeno::graph::view::proximity::test

// }}}
