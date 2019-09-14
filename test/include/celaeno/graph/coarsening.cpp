// vim: set ts=2 sw=2 tw=0 et :
//
// @company     : Universidade Federal de Viçosa - Florestal
// @author      : Ruan E. Formigoni (ruanformigoni@gmail.com)
// @file        : coarsening
// @created     : Friday Sep 13, 2019 18:34:05 -03
// @license     : MIT
// @description : C++ Algorithms Collection
//

#include <range/v3/all.hpp>
#include <celaeno/graph/coarsening.hpp>
#include <celaeno/graph/hem.hpp>
#include <taygete/graph/example.hpp>


namespace celaeno::graph::coarsening::test
{

using namespace celaeno::graph;

template<typename T>
void print(T&& t)
{
  for( auto const& [k,v] : t )
  {
    std::cout << "K: " << k << " V: " << std::endl;
  }
}

TEST_CASE( "Multiplexer" )
{
  using namespace ranges;

  auto const& mux = taygete::graph::example::mux_2_1;

  auto neigh = [&mux](auto const& vx) // Neighboring vertices
    {
      return mux.get_adjacent(vx);
    };

  auto card = [&mux](auto const& vx) // Cardinality
    {
      return mux.get_adjacent(vx).size();
    };

  auto umap = coarsening::coarsening<int32_t>(hem::hem(7, neigh, card));

  for( auto const& e : umap )
  {
    std::cout << ": " <<  e.first << " →";

    std::cout << " " << e.second.first << "," << e.second.second;

    std::cout << std::endl;

  }

  auto umap2 = coarsening::coarsening<int32_t>(umap);

  // for( auto const& e : umap2 )
  // {
  //   std::cout << ": " <<  e.first << " →";
  //
  //   std::cout << " " << e.second.first << "," << e.second.second;
  //
  //   std::cout << std::endl;
  //
  // }

  // auto umap3 = coarsening::coarsening<int32_t>(umap2);

}

} // namespace celaeno::graph::coarsening::test
