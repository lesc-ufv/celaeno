// vim: set expandtab fdm=marker ts=2 sw=2 tw=100 et :
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
#include <range/v3/all.hpp>
#include <celaeno/concepts.hpp>
#include <celaeno/graph/operations/minimize/crossings/impl.hpp>
#include "crossings/data.hpp"
#include "../../../test.hpp"

// namespace celaeno::graph::operations::minimize::crossings::test {{{
namespace celaeno::graph::operations::minimize::crossings::test
{

// Using namespaces {{{
using namespace celaeno::concepts;
// }}}

// namespaces {{{
namespace rv = ranges::views;
namespace test = celaeno::test;
namespace impl = celaeno::graph::operations::minimize::crossings::impl;
// }}}

TEST_CASE("celaeno::graph::operations::minimize::crossings"
  * doctest::description("Graph crossing minimization test")
)
{
  // lamb: test bor {{{
  auto test_bor = []<Matrix M>(M&& m, M&& solution) -> void
  {
    auto _m {impl::bor(std::forward<M>(m))};
    for (auto&& [result,expected] : rv::zip(_m,solution))
    {
      test::check(result == expected);
    } // for row : fp::zip(_m,data::mrsb1)
  }; // }}}

  // Test barycenter ordering method {{{
  test_bor(data::bs1,data::br1);
  test_bor(data::bs2,data::br2);
  test_bor(data::bs3,data::br3);
  test_bor(data::bs4,data::br4);
  test_bor(data::bs5,data::br5);
  // }}}

  // lamb: test boc {{{
  auto test_boc = []<Matrix M>(M&& m)
  {
    auto _m {impl::boc(std::forward<M>(m))};
  }; // }}}

} // TEST_CASE: "celaeno::graph::operations::minimize::crossings"

} // namespace celaeno::graph::operations::minimize::crossings::test }}}
