/*vim: set expandtab ts=2 sw=2 tw=80 et :*/
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
#include <utility>  // std::forward
#include <celaeno/graph/views/depth.hpp>
#include <range/v3/all.hpp>

namespace celaeno::graph::balance
{

template<typename T1, typename F1, typename F2, typename F3, typename F4>
void balance(
  T1&& root,
  F1&& pred,
  F2&& succ,
  F3&& link,
  F4&& unlink,
  int64_t counter = -1
)
{
  namespace depth = celaeno::graph::views::depth;
  namespace rg = ranges;
  namespace rv = ranges::views;
  namespace ra = ranges::actions;

  //
  // Build depth-map
  //
  auto [depth_vertex,vertex_depth] =
    depth::depth(
        std::forward<T1>(root),std::forward<F1>(pred),std::forward<F2>(succ)
    );

  //
  // Get the levels indexes
  //
  auto levels { depth_vertex | rv::keys | rv::unique | rg::to<std::vector> };

  //
  // Algorithm
  //
  for (auto const& i : levels)
  {
    auto range {depth_vertex.equal_range(i)};
    for( auto it{range.first}; it!=range.second; ++it )
    {
      auto current{it->second};
      // For each predecessor current
      auto predecessors{pred(current)};
      for( auto predecessor : predecessors )
      {
        auto d1{vertex_depth[predecessor]};
        auto d2{vertex_depth[current]};
        auto distance{d2-d1};
        while( distance > 1 )
        {
          //
          // ↓         ↓
          // A         B
          // * ------> *
          //
          // Insert pseudo vertex in-between
          //
          // A    C    B
          // * -> * -> *
          //  \_______/
          //
          link(std::make_pair(predecessor,counter));
          link(std::make_pair(counter,current));
          //
          // ↓         ↓
          // A    C    B
          // * -> * -> *
          //  \_______/
          //
          // Remove old connection
          //
          // A    C    B
          // * -> * -> *
          //
          unlink(std::make_pair(predecessor,current));
          //
          // ↓
          // A    C    B
          // * -> * -> *
          //
          // Update predecessor
          //      ↓
          // A    C    B
          // * -> * -> *
          //
          predecessor = counter--;
          //
          // If the distance 'd' is still greater than one, more pseudo
          // vertices need to be inserted in-between C and B.
          // A    C    B
          // * -> * -> *
          //      |____|
          //        d
          //
          --distance;
        } // while
      } // for
    } // for
  } // for: i

} // balance

} // namespace celaeno::graph::balance
