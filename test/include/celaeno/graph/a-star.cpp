// vim: set ts=2 sw=2 tw=0 et :
//
// @author      : Ruan E. Formigoni (ruanformigoni@gmail.com)
// @file        : a-star
// @created     : Thursday Feb 20, 2020 15:25:14 -03
//

#pragma once

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
