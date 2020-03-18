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


#define CATCH_CONFIG_MAIN

#include <catch2/catch.hpp>
#include <taygete/graph.hpp>
#include <taygete/graph-reader.hpp>
#include <celaeno/graph/a-star.hpp>
#include <celaeno/graph/bfs.hpp>

namespace celaeno::graph::a_star::test
{

TEST_CASE("A* Algorithm", "[a-star.hpp]")
{
  SECTION("C17 circuit")
  {
    static std::string_view constexpr c17 {
      "module c17 (N1, N2, N3, N6, N7, N22, N23);"
      "input  N1, N2, N3, N6, N7;"
      "output N22, N23;"
      "wire new_N10_, new_N11_, new_N16_, new_N19_;"
      "assign new_N10_ = ~N1 | ~N3;"
      "assign new_N11_ = ~N3 | ~N6;"
      "assign new_N16_ = ~N2 | ~new_N11_;"
      "assign new_N19_ = ~new_N11_ | ~N7;"
      "assign N22 = ~new_N10_ | ~new_N16_;"
      "assign N23 = ~new_N16_ | ~new_N19_;"
      "endmodule"
    };
    taygete::graph::Graph<int64_t> g;

    auto callback = [&g](auto const& a, auto const& b)
      { g.emplace(std::make_pair(a,b)); };

    std::stringstream ss; ss << c17;

    taygete::graph::reader::Reader reader(ss, callback);

    auto neighbors = [&g](auto& e){ return g.get_successors(e); };
    auto heuristic = [](int64_t e){ return 1; };
    auto distance = [](auto& e1, auto& e2){ return 1; };

    auto result
    {
      celaeno::graph::a_star::a_star(5,9,neighbors,distance,heuristic)
    };

    constexpr std::array<int32_t,3> res1 = { 5,6,9 };
    REQUIRE(res1[0] == result->at(0));
    REQUIRE(res1[1] == result->at(1));
    REQUIRE(res1[2] == result->at(2));

    constexpr std::array<int32_t,4> res2 = { 3,4,6,10 };
    result = celaeno::graph::a_star::a_star(3,10,neighbors,distance,heuristic);
    REQUIRE( result );
    REQUIRE(res2[0] == result->at(0));
    REQUIRE(res2[1] == result->at(1));
    REQUIRE(res2[2] == result->at(2));
    REQUIRE(res2[3] == result->at(3));

    constexpr std::array<int32_t,4> res3 = { 7,8,10 };
    result = celaeno::graph::a_star::a_star(7,10,neighbors,distance,heuristic);

    REQUIRE( result );
    REQUIRE(res3[0] == result->at(0));
    REQUIRE(res3[1] == result->at(1));
    REQUIRE(res3[2] == result->at(2));

    result = celaeno::graph::a_star::a_star(0,10,neighbors,distance,heuristic);
    REQUIRE_FALSE(result);

    result = celaeno::graph::a_star::a_star(0,6,neighbors,distance,heuristic);
    REQUIRE_FALSE(result);

    result = celaeno::graph::a_star::a_star(0,4,neighbors,distance,heuristic);
    REQUIRE_FALSE(result);

    result = celaeno::graph::a_star::a_star(7,0,neighbors,distance,heuristic);
    REQUIRE_FALSE(result);

    result = celaeno::graph::a_star::a_star(7,9,neighbors,distance,heuristic);
    REQUIRE_FALSE(result);
  } // SECTION
} // TEST_CASE

} // namespace celaeno::graph::bfs::test
