//
// @author      : Ruan E. Formigoni (ruanformigoni@gmail.com)
// @file        : balance
// @created     : Wednesday Apr 01, 2020 21:24:23 -03
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
#include <cstdlib>
#include <concepts>
#include <range/v3/all.hpp>
#include <celaeno/graph/balance.hpp>
#include <celaeno/graph/views/depth.hpp>
#include <taygete/graph/graph.hpp>
#include <taygete/graph/reader.hpp>
#include <maia/circuits/iscas.hpp>
#include <maia/circuits/synth-91.hpp>

//
// Aliases
//
namespace graph = taygete::graph;
namespace cir = maia::circuits;
namespace balance = celaeno::graph::balance;
namespace depth = celaeno::graph::views::depth;
namespace rg = ranges;
namespace rv = ranges::views;
namespace ra = ranges::actions;

//
// Concepts
//

template<typename T>
concept String = requires(T t){ std::string{t}; };

//
// Test Wrapper
//
template<String T>
void TEST(T&& str)
{
  graph::Graph<int64_t> g;
  auto emplace = [&g](auto&& pair){ g.emplace(pair); };
  taygete::graph::reader::Reader reader{str,emplace};

  //
  // Helpers
  //

  auto pred = [&g](auto&& v){ return g.get_predecessors(v); };
  auto succ = [&g](auto&& v){ return g.get_successors(v); };
  auto link = [&g](auto&& pair){ g.emplace(pair); };
  auto unlink = [&g](auto&& pair){ g.erase(pair); };

  // Execution
  balance::balance(0,pred,succ,link,unlink);

  //
  // Verification
  //

  // * Given a depth-view, each vertex must have a distance of one
  // * to its successor or predecessor
  auto dview {depth::depth(0,pred,succ)};
  auto const& level_vert {dview.first};
  auto const& vert_level {dview.second};

  // Get the levels
  auto levels { level_vert | rv::keys | rv::unique };

    // Get the vertices on level l
  for(auto const& l : levels)
  {
    auto rng{level_vert.equal_range(l)};
    // For each vertex on level l
    for(auto it{rng.first}; it!=rng.second; ++it)
    {
      // Current vertex
      auto const& curr {it->second};
      // The adjacent vertices
      auto adj {g.get_adjacent(curr)};
      // Verify if distance is one to each
      auto is_dist_one = [&vert_level,&curr](auto&& a) -> void
        { REQUIRE(std::abs(vert_level.at(a) - vert_level.at(curr)) == 1); };
      // Execute tests
      rg::for_each(adj, is_dist_one);
    }
  }

} // function: TEST

//
// Test Cases
//

TEST_CASE("celaeno::graph::balance"
  * doctest::description("Depth-First Search test")
  * doctest::timeout(100.0f)
)
{
  //
  // Iscas
  //

  TEST(cir::iscas::s27);
  TEST(cir::iscas::s298);
  TEST(cir::iscas::s349);
  TEST(cir::iscas::s208);
  TEST(cir::iscas::s420);
  TEST(cir::iscas::s838);
  TEST(cir::iscas::s386);
  TEST(cir::iscas::s510);
  TEST(cir::iscas::s1494);
  TEST(cir::iscas::s832);

  //
  // LGSynth 91
  //
  TEST(cir::synth_91::alu2);
  TEST(cir::synth_91::alu4);
  TEST(cir::synth_91::dalu);
  TEST(cir::synth_91::apex6);
  TEST(cir::synth_91::apex7);
  TEST(cir::synth_91::b1);
  TEST(cir::synth_91::c8);
  TEST(cir::synth_91::cc);
  TEST(cir::synth_91::cht);
  TEST(cir::synth_91::cm138a);
  TEST(cir::synth_91::cm150a);
  TEST(cir::synth_91::cm151a);
  TEST(cir::synth_91::cm162a);
  TEST(cir::synth_91::cm163a);
  TEST(cir::synth_91::cm42a);
  TEST(cir::synth_91::cm82a);
  TEST(cir::synth_91::cm85a);
  TEST(cir::synth_91::cmb);
  TEST(cir::synth_91::comp);
  TEST(cir::synth_91::cordic);
  TEST(cir::synth_91::cu);
  TEST(cir::synth_91::count);
  TEST(cir::synth_91::decod);
  TEST(cir::synth_91::my_adder);
} // TEST_CASE: celaeno::graph::balance
