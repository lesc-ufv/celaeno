// vim: set expandtab fdm=marker ts=2 sw=2 tw=100 et :
//
// @author      : Ruan E. Formigoni (ruanformigoni@gmail.com)
// @file        : matrix-realization
// @created     : sunday jun 14, 2020 15:43:43 -03
//

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest/doctest.h>
#include <array>

// Helper libraries
#include <range/v3/all.hpp>
#include <fplus/fplus.hpp>

// Taygete
#include <taygete/graph/graph.hpp>

// Celaeno
#include <celaeno/aliases.hpp>
#include <celaeno/graph/representations/incidence.hpp>
#include <celaeno/graph/views/depth.hpp>


// namespace: celaeno::graph::representations::incidence::test {{{

namespace celaeno::graph::representations::incidence::test
{

// Namespaces {{{
namespace graph = taygete::graph;
namespace incidence = celaeno::graph::representations::incidence;
// }}}

// Helpers {{{

template<typename C1, typename C2>
void compare(C1&& c1, C2&& c2)
{
  REQUIRE(c1.size() == c2.size());
  for (size_t i{0}; i < c1.size(); ++i)
  {
    REQUIRE(c1.at(i).size() == c2.at(i).size());
    for (size_t j{0}; j < c2.at(i).size(); ++j)
    {
      REQUIRE(c1.at(i).at(j) == c2.at(i).at(j));
    } // for: j
  } // for: i
} // function: compare

// }}}

// Test case celaeno::graph::representations::incidence {{{

TEST_CASE("celaeno::graph::representations::incidence"
  * doctest::timeout(100.0f))
{
  // Used graph available at:
  // https://gitlab.com/formigoni/celaeno/-/blob/development/doc/celaeno/graph/matrix-realization-test.png
  std::array<std::array<i8,4>,2> m1
  {{
    {1,1,1,1},
    {1,0,0,1},
  }};

  std::array<std::array<i8,4>,4> m2
  {{
    {1,0,0,0},
    {0,1,1,1},
    {1,0,0,1},
    {0,0,0,0},
  }};

  std::array<std::array<i8,3>,4> m3
  {{
    {1,0,0},
    {0,0,0},
    {1,0,1},
    {1,1,0},
  }};

  // Subcase: Even number of layers {{{
  SUBCASE("Even number of layers")
  {
    graph::Graph<i32> g
    {
      // Layer 1 → 2
      { 1,3 },{ 1,4 },{ 1,5 },{ 1,6 },{ 2,3 },{ 2,6 },
      // Layer 2 → 3
      { 3,7 },{ 5,7 },{ 5,10 },{ 4,8 },{ 4,9 },{ 4,10 },
      // Layer 3 → 4
      { 7,11 },{ 9,11 },{ 9,13 },{ 10,11 },{ 10,12 }
    };


    // Helpers
    auto f_pred = [&g](auto u){ return g.predecessors(u); };
    auto f_succ = [&g](auto u){ return g.successors(u); };
    auto f_adj = [&g](auto u, auto v){ return g.adjacent(u,v); };
    // Create matrix realization
    auto matrices {incidence::run(1,f_pred,f_succ,f_adj)};

    // Test against expected result
    REQUIRE(matrices.size() == 3);
    compare(matrices.at(0),m1);
    compare(matrices.at(1),m2);
    compare(matrices.at(2),m3);

  } // SUBCASE: "Even number of layers" }}}

  // Subcase: Odd number of layers {{{

  SUBCASE("Odd number of layers")
  {
    graph::Graph<i64> g
    {
      // Layer 1 → 2
      { 1,3 },{ 1,4 },{ 1,5 },{ 1,6 },{ 2,3 },{ 2,6 },
      // Layer 2 → 3
      { 3,7 },{ 5,7 },{ 5,10 },{ 4,8 },{ 4,9 },{ 4,10 }
    };

    // Helpers
    auto f_pred = [&g](auto u){ return g.predecessors(u); };
    auto f_succ = [&g](auto u){ return g.successors(u); };
    auto f_adj = [&g](auto u, auto v){ return g.adjacent(u,v); };
    // Create matrix realization
    auto matrices {incidence::run(1, f_pred, f_succ, f_adj)};

    // TESTS
    REQUIRE(matrices.size() == 2);
    compare(matrices.at(0),m1);
    compare(matrices.at(1),m2);

  } // SUBCASE: "Odd number of layers" }}}

} // TEST_CASE: celaeno::graph::representations::incidence }}}

} // namespace celaeno::graph::representations::incidence::test }}}
