// vim: set ts=2 sw=2 tw=0 et :
//
// @company     : Universidade Federal de Viçosa - Florestal
// @author      : Ruan E. Formigoni (ruanformigoni@gmail.com)
// @file        : bfs
// @created     : Wednesday Aug 14, 2019 13:59:44 -03
// @license     : MIT
// @description : C++ Algorithms Collection
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

#include <vector>
#include <queue>
#include <set>
#include <tuple>
#include <range/v3/all.hpp>

namespace celaeno::graph::bfs
{


template<typename T1, typename T2>
auto operator|=(T1 const& lhs, T2 const& rhs)
{
  return lhs | rhs | ranges::to<T1>;
}

//
// Implementation
//
template<typename T, typename F1, typename F2>
std::vector<T> bfs(T&& root, F1&& get_adjacent, F2&& callback)
{
  namespace rg = ranges;
  namespace rv = ranges::views;
  namespace ra = ranges::actions;

  // Static Assertions
  static_assert( std::is_integral<T>(),
    "\n\n\033[91;1m * \033[mThe type T is not an integral type.\n" );

  std::queue<T> queue;
  std::set<T> visited; // Using std::set for log(n) query
  std::vector<T> result;

  // Push initial vertex into the queue
  queue.push(root);
  result.push_back(root);

  while( ! queue.empty() )
  {
    // Get next vertex
    auto vertex {queue.front()}; queue.pop();

    if( visited.contains(vertex) ) continue;

    // Mark as visited
    visited.insert(vertex);

      // Get the adjacent vertices
    get_adjacent(vertex)
      // Remove visited vertices
    |= rv::filter([&visited](auto&& v){return ! visited.contains(v);})
      // Insert non-visited into the queue
    | ra::transform([&queue](auto&& v){ queue.push(v); return v; })
      // Insert into the vertex in the result vector
    | ra::transform([&result](auto&& v){ result.push_back(v); return v; });

    // Execute callback on current vertex
    if ( callback(vertex) ) result;
  }
  return result;
}

//
// Pipe Closure
//
template<typename T, typename F1, typename F2>
decltype(auto) operator|(
  std::tuple<T,F1,F2> args,
  decltype(bfs<T,F1,F2>) const& bfs
)
{
  static auto const& ret = bfs(
    std::forward<T>(std::get<0>(args)),
    std::forward<F1>(std::get<1>(args)),
    std::forward<F2>(std::get<2>(args))
  );

  return ret;
} // function: operator|

} // namespace celaeno::graph::bfs
