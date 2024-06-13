// vim: set expandtab fdm=marker ts=2 sw=2 tw=100 et :
//
// @author      : Ruan E. Formigoni (ruanformigoni@gmail.com)
// @file        : dfs
// @created     : Wednesday Mar 18, 2020 14:52:57 -03
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

#include <stack>
#include <unordered_map>
#include <type_traits>
#include <concepts>

#include <fplus/fplus.hpp>
#include <range/v3/all.hpp>
#ifndef NDEBUG
  #include <spdlog/spdlog.h>
#endif

#include <celaeno/concepts.hpp>
#include <celaeno/log/log.hpp>

// namespace celaeno::graph::search::dfs
namespace celaeno::graph::search::dfs
{

namespace
{

// Namespaces
namespace rg = ranges;
namespace fp = fplus;
namespace fw = fplus::fwd;
namespace ns_log = celaeno::log;

// Using namespaces
using namespace celaeno::concepts;
using namespace celaeno::aliases;

}

// fn: run {{{
template<SignedIntegral T, typename P, typename S, typename C = std::function<bool(int64_t)>>
std::vector<T> run(T root, P&& pred, S&& succ, C&& cb = [](auto&&){return false;})
  requires CallableWith<P,i64>
  && CallableWith<S,i64>
  && CallableWith<C,i64>
{
  [[maybe_unused]] ns_log::Timer timer("celaeno::graph::search::dfs");

  // Create adjacent helper
  auto nb = [&](auto&& u){ return fp::append(pred(u),succ(u)); };

  // Stack of vertices
  std::stack<T> stack;

  // Map of visited vertices
  std::unordered_map<T,bool> visited;

  // Result that contains all visited vertices
  // until callback returns true
  std::vector<T> result;

  // Push root vertex into the stack
  stack.push(root);

  while ( ! stack.empty() )
  {
    // Get the vertex at the top of the stack
    auto vertex {stack.top()}; stack.pop();

    // Check if it has been visited
    if ( visited.contains(vertex) ) continue;

    // Insert the vertex in the result
    result.push_back(vertex);

    // Mark the vertex as visited
    visited.emplace(vertex,true);

    // Get the adjacent vertices
    // Remove the visited ones
    auto is_visited = [&visited](auto&& v){ return visited.contains(v); };
    auto not_visited {fw::apply(nb(vertex), fw::drop_if(is_visited))};

    // Insert the unvisited vertices into the stack
    rg::for_each(not_visited, [&stack](auto&& v){ stack.push(v); });

    // Execute callback on current vertex
    if( cb(vertex) ) return result;
  } // while
  return result;
} // function: run }}}

} // namespace celaeno::graph::search::dfs
