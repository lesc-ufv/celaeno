//
// @author      : Ruan E. Formigoni (ruanformigoni@gmail.com)
// @file        : orientation
// @created     : Sunday Mar 15, 2020 11:25:08 -03
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

#include <map>
#include <deque>
#include <unordered_map>
#include <type_traits>
#include <range/v3/all.hpp>

namespace celaeno::graph::views::breadth
{

template<typename T1, typename T2, typename F1, typename F2>
auto breadth(
  T1&& root,
  T2&& depth_map,
  F1&& get_predecessors,
  F2&& get_successors
)
{
  using Vertex = std::remove_reference_t<T1>;
  namespace rv = ranges::views;
  namespace ra = ranges::actions;
  namespace rg = ranges;

  std::deque<Vertex> deque;

  // Current columns
  int64_t column{0};

  // Hash vertex -> column
  std::map<Vertex,int64_t> hash;

  // Insert initial vertex in deque
  deque.push_front(root);

  // Visited nodes
  std::unordered_map<Vertex,bool> visited;

  // Detect all subtrees
  while (! deque.empty())
  {
    auto current {deque.front()}; deque.pop_front();
    if( visited.contains(current) ) continue;
    else visited.insert({current,true});
    auto pred {get_predecessors(current)};
    auto succ {get_successors(current)};

    // If there is another node in this column & depth
    auto same_depth = [&depth_map,&current](auto&& e)
      { return depth_map.at(e.first) == depth_map.at(current); };
    auto same_column = [&column](auto&& e)
      { return e.second == column; };
    auto same_depth_column = [&same_depth,&same_column](auto&& e)
      { return same_depth(e) && same_column(e); };

    if( rg::find_if(hash, same_depth_column) != rg::end(hash) ) { ++column; }

    hash.insert({current,column});

    rg::for_each(pred, [&deque](auto&& e){deque.push_front(e);});
    rg::for_each(succ, [&deque](auto&& e){deque.push_back(e);});
  } // while: ! deque.empty()
  return hash;
} // breadth

} // namespace celaeno::graph::views::breadth
