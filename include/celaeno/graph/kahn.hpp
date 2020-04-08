//
// @author      : Ruan E. Formigoni (ruanformigoni@gmail.com)
// @file        : kahn
// @created     : Wednesday Apr 08, 2020 11:42:02 -03
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
#include <deque>
#include <unordered_map>
#include <celaeno/graph/bfs.hpp>
#include <fplus/fplus.hpp>

namespace celaeno::graph::kahn
{

//
// Aliases
//
namespace fp = fplus;
namespace fw = fplus::fwd;
namespace bfs = celaeno::graph::bfs;

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
template< SignedIntegral T, Fn F1, Fn F2, Fc F3 = std::function<bool(int64_t)> >
std::vector<T> kahn(T&& root, F1&& pred, F2&& succ, F3&& cb = [](auto&&){return false;})
{
  auto adj = [&pred,&succ](auto&& v){ return fp::append(pred(v),succ(v)); };

  // Topologically sorted result
  std::vector<T> result;

  // Vertices with no incomming edges
  std::deque<T> deque;

  // Removed edges
  std::set<std::pair<T,T>> re;

  // Populate the deque
  auto has_pred = [&pred](auto&& v){ return ! pred(v).empty(); };
  bfs::bfs(root,adj,[&has_pred,&deque](auto&& v)
    { if( ! has_pred(v) ){ deque.push_back(v); } return false; });

  while (! deque.empty() )
  {
    // Get the current vertex
    auto c{deque.front()}; deque.pop_front();

    // Include in the result
    result.push_back(c);

    // Perform the callback
    if( cb(c) ) return result;

    // Get successors
    for( auto s : succ(c) )
    {
      // remove edge c -> s
      re.insert({c,s});
      // If s has no more predecessors
      auto is_rm = [&s,&re](auto&& v){ return re.contains({v,s}); };
      auto preds_s {fp::drop_if(is_rm, pred(s))};
      // Insert s into the queue
      if( preds_s.empty() ) { deque.push_back(s); }
    }
  } // while: ! initial.empty()
  return result;
}

} // namespace celaeno::graph::kahn
