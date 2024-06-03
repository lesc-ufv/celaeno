// vim: set expandtab fdm=marker ts=2 sw=2 tw=80 et :
//
// @author      : Ruan E. Formigoni (ruanformigoni@gmail.com)
// @file        : minimal-basis
// @created     : Saturday Oct 23, 2021 14:20:16 -03
//
// BSD 2-Clause License

// Copyright (c) 2021, Ruan Evangelista Formigoni
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

using namespace celaeno::aliases;
using namespace celaeno::concepts;

template<typename T>
using Graph = celaeno::graph::Graph<T>;

Graph<i64> g
{
  std::make_pair(-36, 13),
  std::make_pair(-36, 14),
  std::make_pair(-35, 9),
  std::make_pair(-35, 7),
  std::make_pair(-34, 7),
  std::make_pair(-34, 4),
  std::make_pair(-33, -34),
  std::make_pair(-33, -35),
  std::make_pair(-32, 4),
  std::make_pair(-32, -34),
  std::make_pair(-31, -32),
  std::make_pair(-31, -33),
  std::make_pair(-30, 11),
  std::make_pair(-30, 10),
  std::make_pair(-29, 10),
  std::make_pair(-29, 8),
  std::make_pair(-28, -29),
  std::make_pair(-28, -30),
  std::make_pair(-27, 8),
  std::make_pair(-27, 6),
  std::make_pair(-26, -27),
  std::make_pair(-26, -29),
  std::make_pair(-25, -26),
  std::make_pair(-25, -28),
  std::make_pair(-24, 3),
  std::make_pair(-24, 2),
  std::make_pair(-23, 2),
  std::make_pair(-23, -31),
  std::make_pair(-22, -23),
  std::make_pair(-22, -24),
  std::make_pair(-21, -23),
  std::make_pair(-21, -32),
  std::make_pair(-20, -21),
  std::make_pair(-20, -22),
  std::make_pair(-11, -8),
  std::make_pair(-11, -9),
  std::make_pair(-10, 15),
  std::make_pair(-10, 16),
  std::make_pair(-9, 11),
  std::make_pair(-9, -30),
  std::make_pair(-8, -25),
  std::make_pair(-8, -28),
  std::make_pair(-6, -3),
  std::make_pair(-6, -4),
  std::make_pair(-5, -25),
  std::make_pair(-5, -26),
  std::make_pair(-4, 6),
  std::make_pair(-4, -27),
  std::make_pair(-3, 3),
  std::make_pair(-3, -24),
  std::make_pair(-2, -20),
  std::make_pair(-2, -22),
  std::make_pair(-1, -21),
  std::make_pair(-1, -20),
  std::make_pair(0, -1),
  std::make_pair(0, -2),
  std::make_pair(1, -6),
  std::make_pair(1, -5),
  std::make_pair(2, -31),
  std::make_pair(3, -33),
  std::make_pair(5, -11),
  std::make_pair(5, -10),
  std::make_pair(6, -35),
  std::make_pair(7, -36),
  std::make_pair(8, 9),
  std::make_pair(9, -36),
  std::make_pair(10, 12),
  std::make_pair(11, 12),
  std::make_pair(12, 13),
  std::make_pair(13, 14),
};

TEST_CASE("celaeno::graph::search::minimal_basis"
  * doctest::description("Get outer cycle")
  * doctest::timeout(10.0f) )
{

} // Test case
