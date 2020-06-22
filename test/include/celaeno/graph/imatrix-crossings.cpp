//
// @author      : Ruan E. Formigoni (ruanformigoni@gmail.com)
// @file        : imatrix-crossings
// @created     : segunda jun 22, 2020 18:48:04 -03
//

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest/doctest.h>
#include <celaeno/graph/imatrix-crossings.hpp>

//
// Aliases
//
namespace crossings = celaeno::graph::imatrix_crossings;

//
// Tests
//

TEST_CASE("celaeno::graph::imatrix_crossings")
{
  SUBCASE("Edge crossings count")
  {
    std::array<std::array<int32_t,5>,4> m0
    {{
      {1,1,0,0,0},
      {1,0,0,1,1},
      {0,1,0,1,1},
      {1,0,1,0,1},
    }};

    std::array<std::array<int32_t,5>,4> m1
    {{
      {1,1,0,0,0},
      {1,0,1,0,1},
      {1,0,0,1,1},
      {0,0,1,1,1},
    }};

    std::array<std::array<int32_t,5>,4> m2
    {{
      {1,0,1,0,0},
      {1,1,0,1,0},
      {1,0,0,1,1},
      {0,0,1,1,1},
    }};

    std::array<std::array<int32_t,5>,4> m3
    {{
      {0,1,1,0,0},
      {1,1,0,1,0},
      {0,1,0,1,1},
      {0,0,1,1,1},
    }};

    std::array<std::array<int32_t,5>,4> m4
    {{
      {1,1,0,1,0},
      {0,1,1,0,0},
      {0,1,0,1,1},
      {0,0,1,1,1},
    }};

    std::array<std::array<int32_t,5>,4> m5
    {{
      {1,1,1,0,0},
      {0,1,0,1,0},
      {0,1,1,0,1},
      {0,0,1,1,1},
    }};


    auto c0 {crossings::imatrix_crossings(m0)};
    auto c1 {crossings::imatrix_crossings(m1)};
    auto c2 {crossings::imatrix_crossings(m2)};
    auto c3 {crossings::imatrix_crossings(m3)};
    auto c4 {crossings::imatrix_crossings(m4)};
    auto c5 {crossings::imatrix_crossings(m5)};

    REQUIRE(c0 == 14);
    REQUIRE(c1 == 10);
    REQUIRE(c2 == 9);
    REQUIRE(c3 == 9);
    REQUIRE(c4 == 8);
    REQUIRE(c5 == 7);

  } // SUBCASE: "Edge crossings count"
} // TEST_CASE: "celaeno::graph::imatrix_crossings"
