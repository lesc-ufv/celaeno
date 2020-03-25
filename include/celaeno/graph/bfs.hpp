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
#include <range/v3/all.hpp>

namespace celaeno::graph::bfs
{

template<typename T, typename F1, typename F2>
T bfs(T&& root, F1&& get_adjacent, F2&& callback)
{
  // Static Assertions
  static_assert( std::is_integral<T>(),
    "\n\n\033[91;1m * \033[mThe type T is not an integral type.\n" );

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
    auto rng = adjs | ranges::views::filter(marked);

    // Update the queue and visited set
    ranges::for_each(rng, [&q,&vs](auto const& c){ q.push(c); vs.insert(c); });

    // Execute callback on current node
    if ( callback(node) ) return node;
  }

  return node;
}

} // namespace celaeno::graph::bfs
