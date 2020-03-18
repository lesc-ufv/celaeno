// vim: set ts=2 sw=2 tw=0 et :
//
// @company     : Universidade Federal de Viçosa - Florestal
// @author      : Ruan E. Formigoni (ruanformigoni@gmail.com)
// @file        : bfs
// @created     : Wednesday Aug 14, 2019 17:21:01 -03
// @license     : MIT
// @description : C++ Algorithms Collection
//

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
