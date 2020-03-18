// vim: set ts=2 sw=2 tw=0 et :
//
// @author      : Ruan E. Formigoni (ruanformigoni@gmail.com)
// @file        : bfs
// @created     : Wednesday Aug 14, 2019 17:21:01 -03
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


#define CATCH_CONFIG_MAIN

#include <catch2/catch.hpp>
#include <array>
#include <celaeno/graph/bfs.hpp>
#include <taygete/graph.hpp>
#include <taygete/graph/example.hpp>

namespace celaeno::graph::bfs::test
{

TEST_CASE("Breadth-First Search", "[bfs]")
{
  using namespace taygete::graph::example;

  SECTION("Multiplexer")
  {
    std::array<int32_t, 7> mux_res_0 { 0, 3, 4, 5, 6, 2, 1 };

    celaeno::graph::bfs::bfs(0,
      [](auto const& node) -> auto
      {
        return mux_2_1.get_adjacent(node);
      },
      [&mux_res_0](auto const& node) -> bool
      {
        static int32_t i{0};
        REQUIRE(node == mux_res_0.at(i++));
        return false; // Perform full BFS
      }
    );
  }
}

} // namespace celaeno::graph::bfs::test
