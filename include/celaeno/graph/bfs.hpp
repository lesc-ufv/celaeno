// vim: set ts=2 sw=2 tw=0 et :
//
// @company     : Universidade Federal de Viçosa - Florestal
// @author      : Ruan E. Formigoni (ruanformigoni@gmail.com)
// @file        : bfs
// @created     : Wednesday Aug 14, 2019 13:59:44 -03
// @license     : MIT
// @description : C++ Algorithms Collection
//

#pragma once

#include <vector>
#include <queue>
#include <set>
#include <range/v3/all.hpp>

namespace celaeno::graph::bfs
{

template<typename T, typename F1, typename F2>
T bfs(T&& root, F1&& get_adjacent, F2&& callback)
{
  // Static Assertions
  static_assert( std::is_integral<T>(),
    "\n\n\033[91;1m * \033[mThe type T is not an integral type.\n" );

  // Using declarations
  using namespace ranges;

  std::queue<T> q;
  std::set<T> vs; // Using std::set for log(n) query

  // Push initial node into the queue
  q.push(root);

  decltype(root) node{};

  while( ! q.empty() )
  {
    // Get next node
    node = q.front(); q.pop();

    // Mark as visited
    vs.insert(node);

    // Get the adjacent nodes
    auto adjs = get_adjacent(node);

    // Remove visited nodes
    auto marked = [&vs](auto const& n){ return vs.find(n) == vs.cend(); };
    auto rng = adjs | views::filter(marked);

    // Update the queue and visited set
    for_each(rng, [&q,&vs](auto const& c){ q.push(c); vs.insert(c); });

    // Execute callback on current node
    if ( callback(node) ) return node;
  }

  return node;
}

} // namespace celaeno::graph::bfs
