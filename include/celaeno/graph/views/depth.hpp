//
// @author      : Ruan E. Formigoni (ruanformigoni@gmail.com)
// @file        : depth
// @created     : Sunday Feb 23, 2020 18:35:30 -03
//

#pragma once

#include <set>
#include <map>
#include <unordered_map>
#include <range/v3/all.hpp>
#include <celaeno/graph/bfs.hpp>

namespace celaeno::graph::views::depth
{

// Inputs
//  → root node
//  → get_successors nodes lambda
//  → get_predecessors nodes lambda
// Output
//  ← Map: Level → Nodes
//  ← Map: Node → Level
template<typename T, typename F1, typename F2>
std::pair<std::multimap<T,T>,std::unordered_map<T,T>>
  depth(
    T&& root,
    F1&& get_successors,
    F2&& get_predecessors
  )
{

  static_assert(std::is_integral_v<T>, " * T must be of an integral type");

  // level -> nodes
  std::multimap<T,T> m;
  // node -> level
  std::unordered_map<T,T> m_rev;

  auto curr_level{0};
  size_t size{};
  std::set<T> visited;

  // Initial BFS
  celaeno::graph::bfs::bfs(
    std::forward<T>(root),
    [&get_predecessors,get_successors](auto&& n)
      {
        auto vp { get_predecessors(std::forward<decltype(n)>(n)) };
        auto vs { get_successors(std::forward<decltype(n)>(n)) };
        std::copy(vs.begin(), vs.end(), std::back_inserter(vp));
        return vp;
      },
    [&get_predecessors,&m,&curr_level,&size,&visited,&m_rev](auto&& n)
      {
        // A node is at level 0 if it has 0 predecessors
        if( get_predecessors(n).size() == 0 )
        {
          // Update level -> node map
          m.insert({curr_level,n});
          m_rev.emplace(n,curr_level);
          // Mark initial nodes as visited
          visited.insert(n);
        }
        // Compute size for future comparison
        ++size;
        return false;
      }
  );

  // Main loop build all levels
  while( visited.size() < size )
  {
    // Skip computed level 0
    ++curr_level;
    // Keep m_rev intact until next iteration
    auto new_m_rev {m_rev};
    // Perform BFS
    celaeno::graph::bfs::bfs(
      std::forward<T>(root),
      [&get_predecessors,&get_successors](auto&& n)
        {
          auto vp { get_predecessors(std::forward<decltype(n)>(n)) };
          auto vs { get_successors(std::forward<decltype(n)>(n)) };
          std::copy(vs.begin(), vs.end(), std::back_inserter(vp));
          return vp;
        },
      [&get_predecessors,&m,&curr_level,&m_rev,&new_m_rev,&visited](auto&& n)
        {
          namespace views = ranges::views;
          // Predecessor nodes
          auto preds { get_predecessors(n) };
          // Are the predecessors in the table (m_rev)?
          auto in_preds{true};
          // Skip inputs
          if( preds.size() == 0 ) { in_preds = false; }
          // Check if any predecessor is not in the table
          auto is_not_in_table =
            [&m_rev](auto&& e) { return ! m_rev.contains(e); };
          auto pred_rng { preds | views::filter(is_not_in_table) };
          // If any is not in the table, then skip the node
          if( ! ranges::empty(pred_rng) ) return false;
          // All the predecessors are in the table
          // If node is already in the table, no need
          // for further action
          if( ! new_m_rev.contains(n) )
          {
            m.insert({curr_level,n});
            new_m_rev.emplace(n,curr_level);
            // Mark node as visited
            visited.insert(n);
          }
          // Perform full BFS
          return false;
        }
    );
    m_rev = new_m_rev;
  }
  return std::make_pair(m,m_rev);
} // depth_view

} // namespace celaeno::graph::view::depth