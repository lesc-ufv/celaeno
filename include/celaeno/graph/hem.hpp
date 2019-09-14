// vim: set ts=2 sw=2 tw=0 et :
//
// @company     : Universidade Federal de Viçosa - Florestal
// @author      : Ruan E. Formigoni (ruanformigoni@gmail.com)
// @file        : hwm
// @created     : Saturday Aug 17, 2019 17:17:23 -03
// @license     : MIT
// @description : C++ Algorithms Collection
//

#pragma once

#include <unordered_map>
#include <algorithm>
#include <random>
#include <vector>
#include <set>
#include <type_traits>

namespace celaeno::graph::hem
{

template<typename T, typename F1, typename F2>
std::unordered_map<T,T> hem(T&& size, F1&& get_adjacent, F2&& get_weight)
{
  // Helpers
  std::random_device rd;
  std::mt19937 g(rd());

  // Static Assertions
  static_assert( std::is_integral<T>(),
    "\n\n\033[91;1m * \033[mThe type T is not an integral type.\n" );

  // Algorithm
  std::set<T> saturated; // Logarithmic lookup for saturated vertices
  std::unordered_map<T,T> edges; // From vertex a to b

  std::vector<T> vxs(size); // Vertices
  std::iota(vxs.begin(), vxs.end(), T{}); // Fill from 0 to size
  std::shuffle(vxs.begin(), vxs.end(), g);


  while( ! vxs.empty() )
  {
    // Get next vertex
    auto vx { vxs.front() }; vxs.erase(vxs.begin());

    // If is saturated, continue
    if ( saturated.find(vx) != saturated.cend() ) continue;

    // Get adjacent vertices
    auto vas { get_adjacent(vx) };

    // Filter marked vertices to remove saturated ones
    vas.erase(std::remove_if(vas.begin(), vas.end(),
      [&saturated](auto const& adj)
      {
        return saturated.find(adj) != saturated.cend();
      }), vas.end());

    if( ! vas.empty() )
    {
      // Get the vertex with the maximum cardinality
      auto va_max = *(vas.begin());
      std::for_each(vas.cbegin()+1, vas.cend(),
        [&va_max, &get_weight](auto const& va)
        {
          if( get_weight(va) > va_max )
          {
            va_max = va;
          }
        }
      );

      // Saturate both vertices
      saturated.insert(va_max);
      saturated.insert(vx);

      // Insert the edge between them in the edge list
      edges.insert({vx, va_max});
    }
  }
  return edges;
}

} // namespace celaeno::graph::hwm
