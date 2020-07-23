/* vim: set expandtab fdm=marker ts=2 sw=2 tw=80 et :*/
//
// @author      : Ruan E. Formigoni (ruanformigoni@gmail.com)
// @file        : matrix-realization
// @created     : sunday jun 14, 2020 15:43:43 -03
//

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest/doctest.h>
#include <array>

// Graph Library
#include <taygete/graph/graph.hpp>
// Tested algorithm
#include <celaeno/graph/matrix-realization.hpp>
#include <celaeno/graph/views/depth.hpp>

#include <range/v3/all.hpp>
#include <fplus/fplus.hpp>

namespace celaeno::graph::matrix_realization::test
{

// Namespaces {{{
namespace graph = taygete::graph;
namespace matrix_realization = celaeno::graph::matrix_realization;
namespace fw = fplus::fwd;
// }}}

// Aliases {{{

// }}}

//
// Helpers
//
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

//
// Tests
//

TEST_CASE("celaeno::graph::matrix_realization"
  * doctest::description("Matrix Realization Test")
  * doctest::timeout(100.0f))
{
  //
  // Create graph available at
  // https://gitlab.com/formigoni/celaeno/-/blob/development/doc/celaeno/graph/matrix-realization-test.png
  //

  //
  // Verification
  //
  std::array<std::array<bool,4>,2> m1
  {{
    {1,1,1,1},
    {1,0,0,1},
  }};

  std::array<std::array<bool,4>,4> m2
  {{
    {1,0,0,0},
    {0,1,1,1},
    {1,0,0,1},
    {0,0,0,0},
  }};

  std::array<std::array<bool,3>,4> m3
  {{
    {1,0,0},
    {0,0,0},
    {1,0,1},
    {1,1,0},
  }};

  SUBCASE("Even number of layers")
  {
    graph::Graph<int32_t> g;
    g.emplace(
      // Layer 1 → 2
      std::make_pair(1,3),
      std::make_pair(1,4),
      std::make_pair(1,5),
      std::make_pair(1,6),
      std::make_pair(2,3),
      std::make_pair(2,6),
      // Layer 2 → 3
      std::make_pair(3,7),
      std::make_pair(5,7),
      std::make_pair(5,10),
      std::make_pair(4,8),
      std::make_pair(4,9),
      std::make_pair(4,10),
      // Layer 3 → 4
      std::make_pair(7,11),
      std::make_pair(9,11),
      std::make_pair(9,13),
      std::make_pair(10,11),
      std::make_pair(10,12)
  );


    //
    // Create hierarchical graph
    //
    auto pred = [&g](auto&& v){ return g.predecessors(v); };
    auto succ = [&g](auto&& v){ return g.successors(v); };
    auto [h,_] = celaeno::graph::views::depth::depth(1,pred,succ);

    //
    // Get the key type
    //
    using node_t = decltype(h)::key_type;

    //
    // Lambda to obtain a layer by index
    //
    auto get_layer = [&h](node_t idx)
    {
      return fw::apply(
        h
        , fw::drop_if([&idx](auto&& e){ return e.first != idx; })
        , fw::get_map_values()
        , fw::sort()
      );
    };

    //
    // Lambda to verify if an edge between v → u exists
    //
    auto has_edge = [&g](node_t v, node_t u){ return g.adjacent(v,u); };

    //
    // Lambda to get the height of the graph
    //
    auto height
    {
      fw::apply(
        h
        , fw::get_map_keys()
        , fw::unique()
        , fw::size_of_cont()
      )
    };

    auto matrices
      {matrix_realization::matrix_realization(get_layer, has_edge, height)};

    //
    // TESTS
    //
    REQUIRE(matrices.size() == 3);
    compare(matrices.at(0),m1);
    compare(matrices.at(1),m2);
    compare(matrices.at(2),m3);

  } // SUBCASE: "Even number of layers"

  SUBCASE("Odd number of layers")
  {
    graph::Graph<int64_t> g;
    g.emplace(
      // Layer 1 → 2
      std::make_pair(1,3),
      std::make_pair(1,4),
      std::make_pair(1,5),
      std::make_pair(1,6),
      std::make_pair(2,3),
      std::make_pair(2,6),
      // Layer 2 → 3
      std::make_pair(3,7),
      std::make_pair(5,7),
      std::make_pair(5,10),
      std::make_pair(4,8),
      std::make_pair(4,9),
      std::make_pair(4,10)
    );

    //
    // Create hierarchical graph
    //
    auto pred = [&g](auto&& v){ return g.predecessors(v); };
    auto succ = [&g](auto&& v){ return g.successors(v); };
    auto [h,_] = celaeno::graph::views::depth::depth(1,pred,succ);

    //
    // Get the key type
    //
    using node_t = decltype(h)::key_type;

    //
    // Lambda to obtain a layer by index
    //
    auto get_layer = [&h](node_t idx)
    {
      return fw::apply(
        h
        , fw::drop_if([&idx](auto&& e){ return e.first != idx; })
        , fw::get_map_values()
        , fw::sort()
      );
    };

    //
    // Lambda to verify if an edge between v → u exists
    //
    auto has_edge = [&g](node_t v, node_t u){ return g.adjacent(v,u); };

    //
    // Lambda to get the height of the graph
    //
    auto height
    {
      fw::apply(
        h
        , fw::get_map_keys()
        , fw::unique()
        , fw::size_of_cont()
      )
    };

    auto matrices
      {matrix_realization::matrix_realization(get_layer, has_edge, height)};

    //
    // TESTS
    //
    REQUIRE(matrices.size() == 2);
    compare(matrices.at(0),m1);
    compare(matrices.at(1),m2);

  } // SUBCASE: "Odd number of layers"

} // TEST_CASE: celaeno::graph::matrix_realization

} // namespace celaeno::graph::matrix_realization::test
