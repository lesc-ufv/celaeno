// vim: set expandtab fdm=marker ts=2 sw=2 tw=80 et :
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
#include <doctest/doctest.h>

#include <celaeno/aliases.hpp>
#include <celaeno/concepts.hpp>
#include <celaeno/functional.hpp>
#include <celaeno/test/test.hpp>
#include <celaeno/test/graph/test.hpp>
#include <celaeno/graph/operations/balance/paths.hpp>
#include <celaeno/graph/operations/count/crossings.hpp>
#include <celaeno/graph/operations/minimize/crossings.hpp>
#include <celaeno/graph/operations/minimize/crossings/impl.hpp>

#include <taygete/graph/graph.hpp>
#include <taygete/graph/reader/verilog.hpp>

#include "crossings/data.hpp"

// namespace celaeno::graph::operations::minimize::crossings::test {{{
namespace celaeno::graph::operations::minimize::crossings::test
{

// Macros {{{
#define assertm(exp, msg) assert(((void)msg, exp))
// }}}

// Using namespaces {{{
using namespace celaeno::concepts;
// }}}

// namespaces {{{
namespace graph = taygete::graph;
namespace reader = taygete::graph::reader::verilog;
namespace fun = celaeno::functional;
namespace test = celaeno::test;
namespace balance = celaeno::graph::operations::balance::paths;
namespace depth = celaeno::graph::views::depth;
namespace ccrossings = celaeno::graph::operations::count::crossings;
namespace mcrossings = celaeno::graph::operations::minimize::crossings;
namespace impl = celaeno::graph::operations::minimize::crossings::impl;
// }}}

TEST_CASE("celaeno::graph::operations::minimize::crossings"
  * doctest::description("Graph crossing minimization test")
)
{
  // subcase: barycenter reordering {{{
  SUBCASE("Barycenter reordering")
  {
    // lamb: test {{{
    auto test = []<Matrix M>(M&& m, M&& s) -> decltype(auto)
    {
      return [&m,&s]<typename F>(F&& f) -> void
      {
        for (auto&& [result,expected] : fun::zip(f(m),s))
        {
          test::check(result == expected);
        } // for
      };
    }; // }}}

    // Test row barycenter ordering {{{
    auto bor = []<Matrix M>(M&& m){ return impl::bor(std::forward<M>(m)); };
    test(data::brs1,data::brss1)(bor);
    test(data::brs2,data::brss2)(bor);
    test(data::brs3,data::brss3)(bor);
    test(data::brs4,data::brss4)(bor);
    test(data::brs5,data::brss5)(bor);
    // }}}

    // Test barycenter ordering {{{
    auto boc = []<Matrix M>(M&& m){ return impl::boc(std::forward<M>(m)); };
    test(data::bro1,data::brso1)(boc);
    test(data::bro2,data::brso2)(boc);
    test(data::brs3,data::brss3)(boc);
    test(data::brs4,data::brss4)(boc);
    test(data::brs5,data::brss5)(boc);
    // }}}
  } // subcase: "Barycenter reordering" }}}

  // subcase: "Phase 1" {{{
  SUBCASE("Phase 1")
  {
    auto test_row = []<Matrix M>(M&& sample, M&& result)
    {
      celaeno::test::check(impl::phase_1(sample,true) == result);
    };

    auto test_col = []<Matrix M>(M&& sample, M&& result)
    {
      celaeno::test::check(impl::phase_1(sample,false) == result);
    };

    test_row(data::brs1,data::brss1);
    test_row(data::brs2,data::brss2);
    test_row(data::brs3,data::brss3);
    test_row(data::brs4,data::brss4);
    test_row(data::brs5,data::brss5);

    test_col(data::bro1,data::brso1);
    test_col(data::bro2,data::brso2);
    test_col(data::bro3,data::brso3);
    test_col(data::bro4,data::brso4);
    test_col(data::bro5,data::brso5);
  } // subcase: "Phase 1" }}}

  // subcase: "Phase 2" {{{
  SUBCASE("Phase 2")
  {
    // Logger {{{
    celaeno::test::logger_new({
        .name="celaeno::graph::operations::minimize::crossings",
        .path="logs/celaeno/graph/operations/minimize/crossings.csv",
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

      // Test minimize crossings {{{
      auto f_p = [&g](auto&& v){ return g.predecessors(v); };
      auto f_s = [&g](auto&& v){ return g.successors(v); };
      auto f_l = [&g](auto&& e){ g.emplace(e); };
      auto f_u = [&g](auto&& e){ g.erase(e); };

      auto [result,runtime]
      {
        celaeno::test::runtime([&]{return mcrossings::run(0,f_p,f_s,f_l,f_u);})
      };
      // }}}

      auto dp{depth::run(0,f_p,f_s).first};

      fmt::print("Layers before\n");
      for (auto&& [l,v] : dp)
      {
        fmt::print("{}\n",v);
      } // for

      fmt::print("Layers after\n");
      for (auto&& l : result)
      {
        fmt::print("{}\n",l);
      } // for

      CHECK( result.size() == dp.size() );

      // if (result.cbegin() != result.cend())
      // {
      //   for (auto it{result.cbegin()}; it != std::prev(result.cend()); ++it)
      //   {
      //     if ( std::next(it) != result.cend() )
      //     {
      //       fmt::print("Crossings: {} | {}", ccrossings::single());
      //     } // if
      //   } // for
      // } // if

      // Log results {{{
      celaeno::test::logger_write("{},{},{}", g.vertices_count(), g.edges_count(), runtime);
      // }}}

    }; // lamb: test }}}

    // Perform tests {{{
    celaeno::graph::test::run(test);
    // }}}
  } // SUBCASE: "Phase 2" }}}


} // TEST_CASE: "celaeno::graph::operations::minimize::crossings"

} // namespace celaeno::graph::operations::minimize::crossings::test }}}
