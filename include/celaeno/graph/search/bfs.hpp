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
#include <type_traits>

#include <range/v3/all.hpp>
#include <fplus/fplus.hpp>
#ifndef NDEBUG
  #include <spdlog/spdlog.h>
#endif

#include <celaeno/aliases.hpp>
#include <celaeno/concepts.hpp>
#include <celaeno/log/log.hpp>
#include <celaeno/graph/graph.hpp>

// namespace celaeno::graph::search::bfs
namespace celaeno::graph::search::bfs
{

namespace
{

// Using declarations {{{
using Ops = celaeno::graph::Ops;
// }}}

// Namespaces {{{
namespace fp = fplus;
namespace ra = ranges::actions;
namespace ns_log = celaeno::log;
// }}}

// Using namespaces {{{
using namespace celaeno::concepts;
using namespace celaeno::aliases;
// }}}

// BaseType {{{
template<typename T, typename = void>
struct BaseType { using type = T; };

template<typename T>
struct BaseType<T, std::enable_if_t< std::is_same<T, std::vector<typename T::value_type>>::value >>
{
  using type = typename T::value_type;
};
// }}}

// CbType {{{
template<typename T>
using CbType = std::function<bool(typename BaseType<T>::type)>;
// }}}

}

// run {{{
template<typename T , typename P , typename S , typename C = CbType<T>>
decltype(auto) run(T root
    , P&& f_pred
    , S&& f_succ
    , C&& f_cb = [](auto&&){ return false; })
  requires (CallableWith<P,T> or CallableWith<P,typename T::value_type>)
  && (CallableWith<S,T> or CallableWith<S,typename T::value_type>)
  && (CallableWith<C,T> or CallableWith<C,typename T::value_type>)
{

  [[maybe_unused]] ns_log::Timer timer("celaeno::graph::search::bfs");

  // Create adjacent helper
  auto f_nb = [&](auto&& u){ return fp::append(f_pred(u),f_succ(u)); };

  // Select basetype
  using Type = typename BaseType<T>::type;

  // Queue of vertices
  std::queue<Type> queue;

  // Visited vertices
  std::set<Type> visited; // Using std::set for log(n) query

  // Result that contains all the visited vertices
  // until callback returns true
  std::vector<Type> result;

  // Push initial vertex/vertices into the queue
  if constexpr ( Range<T> )
  {
    std::ranges::for_each(root, [&](auto e){ queue.push(e); });
  } // if
  else
  {
    queue.push(root);
  } // else

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
template<typename T, typename C = CbType<T>>
decltype(auto) run(T root, Ops const& ops, C&& f_cb = [](auto&&){return false;})
{
  return run(root, ops.preds, ops.succs, f_cb);
} // }}}

} // namespace celaeno::graph::search::bfs
