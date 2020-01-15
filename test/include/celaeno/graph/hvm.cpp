// vim: set ts=2 sw=2 tw=0 et :
//
// @company     : Universidade Federal de Viçosa - Florestal
// @author      : Ruan E. Formigoni (ruanformigoni@gmail.com)
// @file        : hwm
// @created     : Saturday Aug 17, 2019 17:32:57 -03
// @license     : MIT
// @description : C++ Algorithms Collection
//

#include <celaeno/graph/bfs.hpp>
#include <celaeno/graph/hvm.hpp>
#include <taygete/graph.hpp>

namespace celaeno::graph::hvm::test
{

TEST_CASE("Heavy-vertex Matching", "[hvm]")
{
  using namespace taygete::graph;

  SECTION("Verify if for each vertex there is only one matching")
  {
    Graph<int32_t> chebyshev
    {
      {0,2},{0,3},{1,3},
      {2,4},{2,5},{3,5},
      {4,7},{4,8},{5,9},{6,9},
      {7,10},{8,11},{9,11},
      {10,12},{10,13},{11,13},
      {12,15},{13,16},{14,16},
      {15,17},{16,17},
      {17,18}
    };

    auto res = celaeno::graph::hvm::hvm(0,
      [&chebyshev]( auto const& node ) -> auto
      {
        return chebyshev.get_adjacent(node);
      }
    );
  }
}

} // namespace celaeno::graph::hwm::test
