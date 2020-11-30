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

#include <fplus/fplus.hpp>
#include <range/v3/all.hpp>

#include <taygete/graph/graph.hpp>
#include <taygete/graph/reader.hpp>

#include <celaeno/aliases.hpp>
#include <celaeno/test/test.hpp>
#include <celaeno/test/graph/test.hpp>
#include <celaeno/concepts.hpp>
#include <celaeno/graph/operations/balance/paths.hpp>
#include <celaeno/graph/operations/count/crossings.hpp>
#include <celaeno/graph/operations/minimize/crossings.hpp>
#include <celaeno/graph/operations/minimize/crossings/impl.hpp>


#include "crossings/data.hpp"

// namespace celaeno::graph::operations::minimize::crossings::test {{{
namespace celaeno::graph::operations::minimize::crossings::test
{

// Macros {{{
#define assertm(exp, msg) assert(((void)msg, exp))
// }}}

// Namespaces {{{
namespace rg = ranges;
namespace fp = fplus;

namespace graph = taygete::graph;
namespace reader = taygete::graph::reader;

namespace test = celaeno::test;
namespace ccrossings = celaeno::graph::operations::count::crossings;
namespace balance = celaeno::graph::operations::balance::paths;
namespace incidence = celaeno::graph::representations::incidence;
namespace mc = celaeno::graph::operations::minimize::crossings;
namespace mci = celaeno::graph::operations::minimize::crossings::impl;
// }}}

// Using namespaces {{{
using namespace celaeno::concepts;
// }}}

// TEST_CASE: "celaeno::graph::operations::minimize::crossings" {{{
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
        for (auto&& [result,expected] : fp::zip(f(m),s))
        {
          test::check(result == expected);
        } // for
      };
    }; // }}}

    // lamb: bor {{{
    auto bor = []<Matrix M>(M&& m)
    {
      return impl::bor(std::forward<M>(m),std::vector<i64>(m.size(),0)).first;
    };
    // }}}

    // Test row barycenter ordering {{{
    test(data::brs1,data::brss1)(bor);
    test(data::brs2,data::brss2)(bor);
    test(data::brs3,data::brss3)(bor);
    test(data::brs4,data::brss4)(bor);
    test(data::brs5,data::brss5)(bor);
    // }}}

    // lamb: boc {{{
    auto boc = []<Matrix M>(M&& m)
    {
      return impl::reverse(
        impl::bor(
          impl::reverse(std::forward<M>(m)),std::vector<i64>(m.at(0).size(),0)
        ).first
      );
    };
    // }}}

    // Test barycenter ordering {{{
    // test(data::bro1,data::brso1)(boc);
    // test(data::bro2,data::brso2)(boc);
    // test(data::brs3,data::brss3)(boc);
    // test(data::brs4,data::brss4)(boc);
    // test(data::brs5,data::brss5)(boc);
    // }}}

  } // subcase: "Barycenter reordering" }}}

  // subcase: "Minimize crossings" {{{
  SUBCASE("Minimize crossings")
  {
    // test lambda {{{
    auto test = [&]()
    {
      // Read graph {{{
      graph::Graph<i64> g
      {
        {6,8},{2,4},{3,4},{1,5},
        {4,7},{5,7},{5,8},{5,9},
        {7,12},{8,10},{8,11},
      };
      // }}}

      // vertices_count > 0 {{{
      assertm(g.vertices_count() > 0, "Empty input graph");
      // }}}

      // Test minimize crossings {{{
      auto f_p = [&g](auto&& v){ return g.predecessors(v); };
      auto f_a = [&g](auto&& u, auto&& v){ return g.adjacent(u,v); };
      auto f_s = [&g](auto&& v){ return g.successors(v); };
      auto f_l = [&g](auto&& e){ g.emplace(e); };
      auto f_u = [&g](auto&& e){ g.erase(e); };


      // TODO erase {{{

      // Print ordered
      mc::run(1,f_p,f_s,f_a,f_l,f_u);

      // TODO erase }}}

      // Test minimize crossings }}}

    }; // lamb: test }}}

    // Perform tests {{{
    test();
    // }}}

  } // }}}

  // subcase: "Batch tests" {{{
  SUBCASE("Batch tests")
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
      auto f_a = [&g](auto&& u, auto&& v){ return g.adjacent(u,v); };
      auto f_l = [&g](auto&& e){ g.emplace(e); };
      auto f_u = [&g](auto&& e){ g.erase(e); };

      // Balance graph
      auto start {std::chrono::system_clock::now()};
      balance::run(0,f_p,f_s,f_l,f_u);
      auto end {std::chrono::system_clock::now()};
      std::chrono::duration<f64> dur {end-start};
      std::stringstream ss; ss << dur.count();

      auto [result,runtime]
      {
        celaeno::test::runtime([&]{return mc::run(0,f_p,f_s,f_a,f_l,f_u);})
      };

      // }}}

      // Log results {{{
      celaeno::test::logger_write("{},{},{}", g.vertices_count(), g.edges_count(), runtime);
      // }}}

    }; // lamb: test }}}

    // Perform tests {{{
    celaeno::graph::test::run(test);
    // }}}
  } // SUBCASE: "Batch tests" }}}


} // }}}

} // namespace celaeno::graph::operations::minimize::crossings::test }}}
