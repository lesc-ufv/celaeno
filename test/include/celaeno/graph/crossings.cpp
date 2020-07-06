//
// @author      : Ruan E. Formigoni (ruanformigoni@gmail.com)
// @file        : crossings
// @created     : segunda jun 22, 2020 18:48:04 -03
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
#include <celaeno/graph/crossings.hpp>

//
// Aliases
//

namespace celaeno::graph::crossings::test
{

//
// Tests
//

TEST_CASE("celaeno::graph::crossings")
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


    auto c0 {crossings::run(m0)};
    auto c1 {crossings::run(m1)};
    auto c2 {crossings::run(m2)};
    auto c3 {crossings::run(m3)};
    auto c4 {crossings::run(m4)};
    auto c5 {crossings::run(m5)};

    REQUIRE(c0 == 14);
    REQUIRE(c1 == 10);
    REQUIRE(c2 == 9);
    REQUIRE(c3 == 9);
    REQUIRE(c4 == 8);
    REQUIRE(c5 == 7);

  } // SUBCASE: "Edge crossings count"
} // TEST_CASE: "celaeno::graph::crossings"

} // namespace celaeno::graph::crossings::test
