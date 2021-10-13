// vim: set expandtab fdm=marker ts=2 sw=2 tw=80 et :
//
// @author      : Ruan E. Formigoni (ruanformigoni@gmail.com)
// @file        : outgoing
// @created     : sunday dec 13, 2020 21:30:37 -03
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

#include <fplus/fplus.hpp>
#include <range/v3/all.hpp>
#ifndef NDEBUG
  #include <spdlog/spdlog.h>
#endif

#include <celaeno/concepts.hpp>
#include <celaeno/graph/graph.hpp>
#include <celaeno/graph/search/bfs.hpp>
#include <celaeno/graph/views/depth.hpp>

// namespace celaeno::graph::operations::balance::outgoing {{{
namespace celaeno::graph::operations::balance::outgoing
{

// Using declarations {{{
using Ops = celaeno::graph::Ops;
// }}}

// namespaces {{{
namespace fp = fplus;
namespace rg = ranges;

namespace ns_views = celaeno::graph::views;
namespace ns_search = celaeno::graph::search;
// }}}

// Using namespaces {{{
using namespace celaeno::concepts;
using namespace celaeno::aliases;
// }}}

// fn: run {{{
template<SignedIntegral T, typename P, typename S, typename L, typename U>
void run(T root, P&& f_pred, S&& f_succ, L&& f_link, U&& f_unlink )
  requires CallableWith<P,T>
  && CallableWith<S,T>
  && CallableWith<L,T,T>
  && CallableWith<U,T,T>
{

#if ! defined(NDEBUG) && defined(DEBUG_SHOW_ALG)
  spdlog::set_level(spdlog::level::debug);
  spdlog::debug("Algorithm: celaeno::graph::operations::balance::outgoing");
#endif

  auto depth_view {ns_views::depth::run(root,
    std::forward<P>(f_pred),
    std::forward<S>(f_succ)
    ).ln
  };

  // Dummy vertex with lowest value
  T idx{};

  // Save a list of all nodes
  std::vector<T> nodes;

  // Define function to compare values lt 0
  auto f_lowest = [&idx,&nodes](auto&& e)
  {
    if(e < idx){ idx=e; }
    nodes.emplace_back(e);
    return false;
  };

  // Get the dummy vertex with the lowest value
  ns_search::bfs::run(root,f_pred,f_succ,f_lowest);

  // Set counter to empty node id
  --idx;

  for (auto n : nodes)
  {
    // Get successors of current node
    auto successors{f_succ(n)};

    // If successors < 2, node is balanced
    if (successors.size() <= 2) { continue; }

    // Unlink all successors from node
    rg::for_each(successors,[&](auto s){ f_unlink(n,s); });

    // Add novel successors to n, until the size is of a maximum of 2 nodes
    while (successors.size() > 2)
    {
      // Split successors into groups
      auto chunks {fp::split_every(2,successors)};

      // Keep currently inserted nodes
      std::vector<T> novel_successors;

      // Create novel layer of nodes successors of n
      rg::for_each(chunks,[&](auto c)
      {
        // Link current chunk to novel node
        rg::for_each(c, [&](auto s) { f_link(idx,s); });
        // Push current novel node id
        novel_successors.emplace_back(idx);
        // Create novel next node id
        --idx;
      });

      // Clear current successors
      successors.clear();

      // Update current successors
      rg::for_each(novel_successors,[&](auto s){ successors.emplace_back(s); });
    } // while

    // Update current successors of n
    rg::for_each(successors, [&](auto s) { f_link(n,s); });
  } // for

} // }}}

// fn: run {{{
template<SignedIntegral T>
void run(T root, Ops ops)
{
  run(root, ops.preds, ops.succs, ops.link, ops.unlink);
}
// }}}

} // namespace celaeno::graph::operations::balance::outgoing }}}
