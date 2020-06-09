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
#include <fplus/fplus.hpp>
#include <celaeno/graph/views/depth.hpp>
#include <celaeno/graph/bfs.hpp>
#include <range/v3/all.hpp>

namespace celaeno::graph::balance
{

//
// Aliases
//

namespace depth = celaeno::graph::views::depth;
namespace bfs = celaeno::graph::bfs;
namespace rg = ranges;
namespace rv = ranges::views;
namespace fw = fplus::fwd;

template<typename T, typename F1, typename F2, typename F3, typename F4>
void balance(T root, F1&& pred, F2&& succ, F3&& link, F4&& unlink )
{
  //
  // Get the pseudo vertex with the lowest value
  //
  auto adj = [&pred,&succ](auto&& v) { return fplus::append(pred(v),succ(v)); };
  auto counter { fw::apply(bfs::bfs(root,adj), fw::sort(), fw::minimum()) };

  //
  // Build depth-map
  //
  auto [depth_vertex,vertex_depth] =
    depth::depth(root,std::forward<F1>(pred),std::forward<F2>(succ));

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
      auto const& current{it->second};
      // For each predecessor current
      for( auto p : pred(current) )
      {
        auto distance{vertex_depth.at(current)-vertex_depth.at(p)};
        while( distance > 1 )
        {
          --counter;
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
          link(std::make_pair(p,counter));
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
          unlink(std::make_pair(p,current));
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
          p = counter;
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
