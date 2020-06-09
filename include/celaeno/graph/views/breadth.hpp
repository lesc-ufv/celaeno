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
#include <set>
#include <type_traits>
#include <fplus/fplus.hpp>
#include <range/v3/all.hpp>
#include <celaeno/graph/views/depth.hpp>

namespace celaeno::graph::views::breadth
{

template<typename T, typename F1, typename F2>
auto breadth(
  T&& root,
  F1&& pred,
  F2&& succ
)
{
  using Vertex = std::decay_t<T>;
  namespace rv = ranges::views;
  namespace ra = ranges::actions;
  namespace rg = ranges;
  namespace fp = fplus;
  namespace fw = fplus::fwd;

  // Create depth map
  auto depth {celaeno::graph::views::depth::depth(root,pred,succ)};
  auto& map_lv{depth.first};
  auto& map_vl{depth.second};

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

    if( visited.contains(current) )
    {
      continue;
    }
    else
    {
      visited.insert({current,true});
    }

    auto p {pred(current)};
    auto s {succ(current)};

    // If there is another vertex in this column & depth
    auto same_depth = [&map_vl,&current](auto&& e)
      { return map_vl.at(e.first) == map_vl.at(current); };
    auto same_column = [&column](auto&& e)
      { return e.second == column; };
    auto same_depth_column = [&same_depth,&same_column](auto&& e)
      { return same_depth(e) && same_column(e); };

    if( rg::find_if(hash, same_depth_column) != rg::end(hash) )
    {
      // Get the depth of current
      auto depth_of_current { map_vl.at(current) };
      // Get all the vertices in level of current
      auto range{map_lv.equal_range(depth_of_current)};
      std::vector<T> siblings;
      for(auto it{range.first}; it!=range.second; ++it)
      {
        siblings.push_back(it->second);
      }
      auto max_column{column};
      for( auto const& s : siblings )
      {
        if( hash.contains(s) )
        {
          auto s_column{hash.at(s)};
          if( s_column > max_column )
          {
            max_column = s_column;
          }
        }
      }
      hash.insert({current,max_column+1});
    }
    else
    {
      hash.insert({current,column});
    }

    rg::for_each(p, [&deque](auto&& e){deque.push_back(e);});
    rg::for_each(s, [&deque](auto&& e){deque.push_front(e);});
    if( s.empty() ) ++column;

  } // while: ! deque.empty()
  return hash;
} // breadth

} // namespace celaeno::graph::views::breadth
