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

#include <celaeno/concepts.hpp>
#include <celaeno/test/test.hpp>
#include <celaeno/graph/operations/minimize/crossings/impl.hpp>

#include "crossings/data.hpp"

// namespace celaeno::graph::operations::minimize::crossings::test {{{
namespace celaeno::graph::operations::minimize::crossings::test
{

// Using namespaces {{{
using namespace celaeno::concepts;
// }}}

// namespaces {{{
namespace test = celaeno::test;
namespace impl = celaeno::graph::operations::minimize::crossings::impl;
// }}}

TEST_CASE("celaeno::graph::operations::minimize::crossings"
  * doctest::description("Graph crossing minimization test")
)
{

  // lamb: zip {{{
  auto zip = []<Iterable C1, Iterable C2>(C1 const& c1, C2 const& c2)
    requires
       requires(C1){ typename std::decay_t<C1>::value_type; }
    && requires(C2){ typename std::decay_t<C2>::value_type; }
  {

    using T1 = typename C1::value_type;
    using T2 = typename C2::value_type;

    std::vector<std::pair<T1,T2>> zipped;

    for (auto it1{c1.begin()}, it2{c2.begin()}; it1 != c1.end() && it2 != c2.end(); ++it1, ++it2)
    {
      zipped.emplace_back(*it1,*it2);
    } // for

    return zipped;
  }; // }}}

  // subcase: barycenter reordering {{{
  SUBCASE("Barycenter reordering")
  {
    // lamb: test {{{
    auto test = [&zip]<Matrix M>(M&& m, M&& s) -> decltype(auto)
    {
      return [&m,&s,&zip]<typename F>(F&& f) -> void
      {
        for (auto&& [result,expected] : zip(f(m),s))
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
  } // SUBCASE: "Barycenter reordering" }}}

  // subcase: equal row ordering {{{
  SUBCASE("Equal row reordering")
  {
  } // SUBCASE: "Barycenter reordering" }}}

} // TEST_CASE: "celaeno::graph::operations::minimize::crossings"

} // namespace celaeno::graph::operations::minimize::crossings::test }}}
