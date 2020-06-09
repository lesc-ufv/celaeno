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
#include <concepts>
#include <fplus/fplus.hpp>
#include <range/v3/all.hpp>

namespace celaeno::graph::bfs
{

//
// Aliases
//
namespace rg = ranges;
namespace fw = fplus::fwd;


//
// Concepts
//
template<typename T>
concept Iterable = requires{ std::input_iterator<T> && std::incrementable<T>; };

template<typename T>
concept SignedIntegral = std::signed_integral<T>;

template<typename T>
concept Fn = requires(T t){ {t(int64_t{})} -> Iterable; };

template<typename T>
concept Fc = requires(T t){ {t(int64_t{})} -> std::same_as<bool>; };

//
// Algorithm
//
template< SignedIntegral T, Fn F1, Fc F2 = std::function<bool(int64_t)> >
std::vector<T> bfs(T root, F1&& adj, F2&& cb = [](auto&&){return false;})
{
  // Queue of vertices
  std::queue<T> queue;

  // Visited vertices
  std::set<T> visited; // Using std::set for log(n) query

  // Result that contains all the visited vertices
  // until callback returns true
  std::vector<T> result;

  // Push initial vertex into the queue
  queue.push(root);

  while( ! queue.empty() )
  {
    // Get next vertex
    auto vertex {queue.front()}; queue.pop();

    // Skip visited vertices
    if( visited.contains(vertex) ) continue;

    // Insert the vertex in the result
    result.push_back(vertex);

    // Mark as visited
    visited.insert(vertex);

    // Get the adjacent vertices
    // Remove the visited ones
    auto is_visited = [&visited](auto&& v){return visited.contains(v);};
    auto not_visited {fw::apply(adj(vertex), fw::drop_if(is_visited))};

    // Insert non-visited into the queue
    rg::for_each(not_visited, [&queue](auto&& v){ queue.push(v); });

    // Execute callback on current vertex
    if ( cb(vertex) ) return result;
  }
  return result;
}

} // namespace celaeno::graph::bfs
