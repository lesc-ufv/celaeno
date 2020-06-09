// vim: set ts=2 sw=2 tw=0 et :
//
// @author      : Ruan E. Formigoni (ruanformigoni@gmail.com)
// @file        : a-star
// @created     : Thursday Feb 20, 2020 15:25:14 -03
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
#include <celaeno/graph/a-star.hpp>
#include <taygete/graph/graph.hpp>
#include <taygete/graph/reader.hpp>

#include <vector>
#include <utility>
#include <range/v3/all.hpp>
#include <fplus/fplus.hpp>

namespace celaeno::graph::a_star::test
{

//
// Aliases
//
namespace a_star = celaeno::graph::a_star;
namespace rg = ranges;
namespace rv = ranges::views;
namespace ra = ranges::action;
namespace fp = fplus;
namespace fw = fplus::fwd;
using float64_t = double;

//
// Helpers
//
template<typename T1, typename T2>
decltype(auto) constexpr manhattan(T1&& p1, T2&& p2) noexcept
{
  return std::abs(p1.first - p2.first) + std::abs(p1.second - p2.second);
}

template<typename T1, typename T2>
decltype(auto) path(T1&& p1, T2&& p2) noexcept
{
  auto neighbors = [](auto&& pair) constexpr
    -> std::array<std::pair<int64_t,int64_t>,6>
  {
    return
      {{
        {pair.first+1,pair.second},
        {pair.first-1,pair.second},
        {pair.first,pair.second+1},
        {pair.first,pair.second-1},
        {pair.first-1,pair.second-1},
        {pair.first+1,pair.second+1},
      }};
  };

  return a_star::a_star(
      std::forward<T1>(p1),
      std::forward<T2>(p2),
      neighbors,
      [](auto&&) -> int32_t { return 0; },
      [&](auto&& p) -> float64_t
      {
        auto m {manhattan(p, p2)};
        return m;
      }
    );
}

//
// Tests
//

TEST_CASE("celaeno::graph::a_star")
{
  // Generate coordinates for tests
  // // Positive only range
  auto pxy {fp::zip(
      fp::zip(fp::numbers(0,101),fp::reverse(fp::numbers(0,101))),
      fp::zip(fp::reverse(fp::numbers(0,101)),fp::numbers(0,101))
  )};
  // // Negative only range
  auto nxy {fp::zip(
      fp::zip(fp::numbers(-100,0),fp::reverse(fp::numbers(-100,0))),
      fp::zip(fp::reverse(fp::numbers(-100,0)),fp::numbers(-100,0))
  )};
  // // Mixed range
  auto mxy {fp::zip(
      fp::zip(fp::numbers(-100,101),fp::reverse(fp::numbers(-100,101))),
      fp::zip(fp::reverse(fp::numbers(-100,101)),fp::numbers(-100,101))
  )};

  SUBCASE("Initialization with lvalues and rvalues")
  {
    // Rvalue search
    auto rsearch {path(std::make_pair(2,0),std::make_pair(5,4))};

    // Lvalue search
    std::pair<int64_t,int64_t> const constexpr p1{2,0}, p2{5,4};
    auto vsearch {path(p1,p2)};

    // Create a combined view
    auto combined_view {fp::zip(rsearch,vsearch)};

    // Check if paths are the same
    rg::for_each(combined_view,[](auto&& v) { REQUIRE(v.first == v.second); });

  } // SUB_CASE: Initialization with lvalues and rvalues

  SUBCASE("Heterogeneous pair types")
  {
    // Rvalue search
    auto rsearch {path(
        std::make_pair<int64_t,int32_t>(2,0),
        std::make_pair<int64_t,int32_t>(5,4)
    )};

    // Lvalue search
    std::pair<int32_t,int64_t> const constexpr p1{2,0};
    std::pair<int32_t,int64_t> const constexpr p2{5,4};
    auto vsearch {path(p1,p2)};

    // Create a combined view
    auto combined_view {fp::zip(rsearch,vsearch)};

    // Check if paths are the same
    rg::for_each(combined_view,[](auto&& v) { REQUIRE(v.first == v.second); });
  } // SUB_CASE: Heterogeneous pair types

  SUBCASE("Path size checks")
  {
    auto check = [](auto&& p1, auto&& p2) -> void
    {
      auto res {path(p1,p2)};
      auto sz_limit {manhattan(p1,p2)};
      // +2, excluding beg and end
      CHECK(res.size() <= sz_limit+2);
    };

    for (auto&& [x,y] : pxy) { check(x,y); } // for xy
    for (auto&& [x,y] : nxy) { check(x,y); } // for xy
    for (auto&& [x,y] : mxy) { check(x,y); } // for xy

  } // SUB_CASE: Path size checks

} // TEST_CASE: celaeno::graph::a_star

} // namespace celaeno::graph::bfs::test
