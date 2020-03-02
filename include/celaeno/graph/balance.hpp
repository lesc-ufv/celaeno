//
// @author      : Ruan E. Formigoni (ruanformigoni@gmail.com)
// @file        : balance
// @created     : Sunday Mar 01, 2020 21:20:48 -03
//

#pragma once

#include <iostream> // std::cerr and std::endl
#include <optional> // std::optional
#include <cstdint>  // int64_t, int32_t,...

namespace celaeno::graph::balance
{

template<typename M, typename T1, typename T2>
std::optional<int64_t> find(M&& m, T1&& e, T2&& size)
{
  auto depth{ m.size() };
  for (auto i = 0; i < depth; ++i)
  {
    // For each node in level
    auto range {m.equal_range(i)};
    for( auto it{range.first}; it!=range.second; ++it )
    {
      if( it->second == e )
        return i;
    }
  }
  return std::nullopt;
}

template<typename M, typename T1, typename T2>
void erase_edge(M&& m, T1&& e1, T2&& e2)
{
  // Find the range of elements with the same key e1
  auto range{ m.equal_range(e1) };
  // Search for the element with value of e2
  auto search{ std::find_if(range.first,range.second,
    [&e2](auto&& tgt){ return tgt.second == e2; }) };
  // Erase the connection e1 -> e2
  if( search != range.second )
  {
    m.erase(search);
  }
  else
  {
    std::cerr
      << " * Could not find connection from predecessor to successor" 
      << std::endl;
    exit(1);
  }
}

template<typename M, typename T>
void insert_in_between(M&& map, T&& e1, T&& e2, T&& e3)
{
  map.emplace(std::forward<T>(e1), std::forward<T>(e2));
  map.emplace(std::forward<T>(e2), std::forward<T>(e3));
}


template<typename T1, typename T2, typename T3, typename F1>
void balance(T1&& depth_map, T2 &&map, T3&& depth, F1&& get_predecessors)
{

  int64_t pseudo_counter{-1};

  // For each level from 0 to max
  for (auto i = 0; i < depth; ++i)
  {
    // For each node in level
    auto range {depth_map.equal_range(i)};
    for( auto it{range.first}; it!=range.second; ++it )
    {
      // For each input of node
      auto inputs{ get_predecessors(it->second) };
      for( auto input : inputs )
      {
        if( auto input_level = find(
              std::forward<T1>(depth_map),
              std::forward<decltype(input)>(input),
              std::forward<T3>(depth)
            )
          )
        {
          if( *input_level != i-1 )
          {
            // Insert new intermediate connection
            insert_in_between(
              std::forward<T2>(map),
              std::forward<decltype(input)>(input),
              std::forward<decltype(input)>(pseudo_counter),
              std::forward<decltype(input)>(it->second)
            );
            // Remove old connection from input to node
            erase_edge(
              std::forward<T2>(map),
              std::forward<decltype(input)>(input),
              std::forward<decltype(it->second)>(it->second)
            );
            // Decrease counter
            --pseudo_counter;
          }
        }
        else
        {
          std::cerr << " * Error: Could not find the input." << std::endl;
        }
      } // for
    } // for
  } // for

} // balance

} // namespace celaeno::graph::balance
