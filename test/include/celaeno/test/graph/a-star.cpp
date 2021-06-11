// vim: set expandtab fdm=marker ts=2 sw=2 tw=100 et :
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
#include <celaeno/aliases.hpp>
#include <celaeno/concepts.hpp>
#include <celaeno/graph/graph.hpp>
#include <celaeno/graph/search/a-star.hpp>
#include <celaeno/graph/reader/verilog.hpp>

#include <vector>
#include <utility>
#include <range/v3/all.hpp>
#include <fplus/fplus.hpp>

// TODO REMOVE
#include <fmt/core.h>
#include <fmt/ranges.h>

//  namespace celaeno::graph::a_star::test {{{

namespace celaeno::graph::a_star::test
{

// Namespaces {{{
namespace ns_search = celaeno::graph::search;
namespace rg = ranges;
namespace fp = fplus;
// }}}

// Using namespaces {{{
using namespace celaeno::aliases;
using namespace celaeno::concepts;
// }}}

// // struct: Tile {{{
// struct Tile
// {
//   i32 x, y;
//   Tile() = default;
//   Tile(i32 x,i32 y) : x(x), y(y) {}
// }; // }}}


TEST_CASE("celaeno::graph::a_star")
{
  auto [x,y] = std::make_pair(std::pair<i64,i64>(0,0), std::pair<i64,i64>(10,0));
  auto result = ns_search::a_star::run(
    x,
    y,
    [](auto k)
    {
      return std::vector<std::pair<i64,i64>>
      {
          std::make_pair(k.first+1,k.second)
        , std::make_pair(k.first,k.second+1)
        , std::make_pair(k.first,k.second-1)
      };
    },
    [](auto k){ return (k.second == 0) && (k.first > 0) && (k.first < 10) && (k != std::pair<i64,i64>(0,4)) && (k != std::pair<i64,i64>(0,0)); }
  );

  fmt::print("Path : ");
  for (auto&& i : result)
  {
    fmt::print("{},", i);
  } // for
  fmt::print("\n");
} // testcase

} // namespace celaeno::graph::bfs::test }}}
