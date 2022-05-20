// vim: set expandtab fdm=marker ts=2 sw=2 tw=80 et :
//
// @author      : Ruan E. Formigoni (ruanformigoni@gmail.com)
// @file        : bfs
// @created     : Wednesday Aug 14, 2019 13:59:44 -03
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

#include <range/v3/all.hpp>
#include <fplus/fplus.hpp>
#ifndef NDEBUG
  #include <spdlog/spdlog.h>
#endif

#include <celaeno/graph/graph.hpp>
#include <celaeno/aliases.hpp>
#include <celaeno/concepts.hpp>

// namespace celaeno::graph::search::bfs {{{
namespace celaeno::graph::search::bfs
{

// Using declarations {{{
using Ops = celaeno::graph::Ops;
// }}}

// Namespaces {{{
namespace fp = fplus;
namespace ra = ranges::actions;
// }}}

// Using namespaces {{{
using namespace celaeno::concepts;
using namespace celaeno::aliases;
// }}}

// Algorithm {{{
template<typename T, typename P, typename S, typename C = std::function<bool(i64)>>
std::vector<T> run(T root, P&& f_pred, S&& f_succ, C&& f_cb = [](auto&&){return false;})
  requires CallableWith<P,T>
  && CallableWith<S,T>
  && CallableWith<C,T>
{

#if ! defined(NDEBUG) && defined(DEBUG_SHOW_ALG)
  spdlog::set_level(spdlog::level::debug);
  spdlog::debug("Algorithm: celaeno::graph::search::bfs");
#endif

  // Create adjacent helper
  auto f_nb = [&](auto&& u){ return fp::append(f_pred(u),f_succ(u)); };

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
    auto f_is_visited = [&visited](auto&& v){return visited.contains(v);};
    auto f_not_visited {f_nb(vertex) | ra::drop_while(f_is_visited)};

    // Insert non-visited into the queue
    std::ranges::for_each(f_not_visited, [&queue](auto&& v){ queue.push(v); });

    // Execute callback on current vertex
    if ( f_cb(vertex) ) return result;
  }
  return result;
} // function: run }}}

// function: run {{{
template<SignedIntegral T, typename C = std::function<bool(i64)>>
std::vector<T> run(T root, Ops const& ops, C&& f_cb = [](auto&&){return false;})
{
  return run(root, ops.preds, ops.succs, f_cb);
} // }}}

} // namespace celaeno::graph::search::bfs }}}
