// vim: set expandtab fdm=marker ts=2 sw=2 tw=80 et :
//
// @author      : Ruan E. Formigoni (ruanformigoni@gmail.com)
// @file        : paths
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

#include <utility>  // std::forward

#ifndef NDEBUG
  #include <spdlog/spdlog.h>
#endif

#include <celaeno/graph/graph.hpp>
#include <celaeno/aliases.hpp>
#include <celaeno/concepts.hpp>
#include <celaeno/graph/views/depth.hpp>
#include <celaeno/graph/search/bfs.hpp>

// namespace celaeno::graph::operations::balance::paths {{{
namespace celaeno::graph::operations::balance::paths
{

// Namespaces {{{
namespace depth = celaeno::graph::views::depth;
namespace bfs = celaeno::graph::search::bfs;
// }}}

// Using namespaces {{{
using namespace celaeno::concepts;
using namespace celaeno::aliases;
// }}}

// Using declarations {{{
using std::ranges::transform;
using Ops = celaeno::graph::Ops;
// }}}

// fn: run {{{
template<SignedIntegral T, typename P, typename S, typename L, typename U>
void run(T root, P&& f_pred, S&& f_succ, L&& f_link, U&& f_unlink )
  requires CallableWith<P,i64>
  && CallableWith<S,i64>
  && CallableWith<L,T,T>
  && CallableWith<U,T,T>
{

#if ! defined(NDEBUG) && defined(DEBUG_SHOW_ALG)
  spdlog::set_level(spdlog::level::debug);
  spdlog::debug("Algorithm: celaeno::graph::operations::balance::paths");
#endif

  // Dummy vertex with lowest value
  i64 idx{};

  // Define function to compare values lt 0
  auto f_lowest = [&idx](auto&& e) { if(e < idx){ idx=e; } return false; };

  // Get the dummy vertex with the lowest value
  bfs::run(root,f_pred,f_succ,f_lowest);

  // vl = Vertex → Level; lv = Level  → Vertex
  auto [lv,vl] = depth::run(root,std::forward<P>(f_pred),std::forward<S>(f_succ));

  // Create the vector with levels indexes
  std::vector<T> levels {};

  // Populate level indexes
  transform(lv, std::back_inserter(levels), [](auto&& e) { return e.first; });

  // Algorithm
  for (auto&& current : levels)
  {
    for( auto it{lv.at(current).begin()}; it!=lv.at(current).end(); ++it )
    {
      // For each predecessor current
      for( auto p : f_pred(*it) )
      {
        auto distance{vl.at(*it)-vl.at(p)};
        while( distance > 1 )
        {
          --idx;
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
          f_link(p,idx);
          f_link(idx,*it);
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
          f_unlink(p,*it);
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
          p = idx;
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

} // }}}

// fn: run {{{
template<SignedIntegral T>
void run(T root, Ops ops)
{
  run(root, ops.preds, ops.succs, ops.link, ops.unlink);
}
// }}}

} // namespace celaeno::graph::operations::balance::paths }}}
