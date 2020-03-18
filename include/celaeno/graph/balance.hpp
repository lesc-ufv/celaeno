//
// @author      : Ruan E. Formigoni (ruanformigoni@gmail.com)
// @file        : balance
// @created     : Sunday Mar 01, 2020 21:20:48 -03
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

template<typename M, typename T>
void insert_in_between(M&& graph, T&& e1, T&& e2, T&& e3)
{
  graph.emplace( std::make_pair(std::forward<T>(e1), std::forward<T>(e2)) );
  graph.emplace( std::make_pair(std::forward<T>(e2), std::forward<T>(e3)) );
}


template<typename T1, typename T2, typename T3, typename F1>
void balance(T1&& depth_map, T2 &&graph, T3&& depth, F1&& get_predecessors)
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
      for( auto const& input : inputs )
      {
        if( auto input_level = find(
              std::forward<T1>(depth_map),
              std::forward<decltype(input)>(input),
              std::forward<T3>(depth)
            )
          )
        {
          auto current{input};
          while( *input_level != i-1 )
          {
            // Insert new intermediate connection
            insert_in_between(
              std::forward<T2>(graph),
              std::forward<decltype(current)>(current),
              std::forward<decltype(current)>(pseudo_counter),
              std::forward<decltype(current)>(it->second)
            );
            // Remove old connection from current to node
            graph.erase_edge(
              {
                std::forward<decltype(current)>(current),
                std::forward<decltype(current)>(it->second)
              }
            );
            // Update current
            current = pseudo_counter;
            // Decrease counter
            --pseudo_counter;
            // Update the input level
            ++(*input_level);
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
