//
// @author      : Ruan E. Formigoni (ruanformigoni@gmail.com)
// @file        : pseudo
// @created     : tuesday mar 09, 2021 19:00:25 -03
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

#ifndef NDEBUG
  #include <spdlog/spdlog.h>
#endif

#include <celaeno/concepts.hpp>
#include <celaeno/aliases.hpp>
#include <celaeno/graph/graph.hpp>
#include <celaeno/graph/search/bfs.hpp>
#include <celaeno/graph/views/depth.hpp>

namespace celaeno::graph::operations::minimize::pseudo
{

// Using declarations {{{
using Ops = celaeno::graph::Ops;
// }}}

// Namespaces {{{
namespace rg = ranges;
namespace ns_views = celaeno::graph::views;
namespace ns_search = celaeno::graph::search;
// }}}

// Using namespaces {{{
using namespace celaeno::aliases;
using namespace celaeno::concepts;
// }}}

// fn: run {{{
template<SignedIntegral T, typename P, typename S, typename L, typename U>
void run(T root, P&& f_pred, S&& f_succ, L&& f_link, U&& f_unlink)
  requires CallableWith<P,i64>
  && CallableWith<S,i64>
  && CallableWith<L,std::pair<i64,i64>>
  && CallableWith<U,std::pair<i64,i64>>
{

#if ! defined(NDEBUG) && defined(DEBUG_SHOW_ALG)
  spdlog::set_level(spdlog::level::debug);
  spdlog::debug("Algorithm: celaeno::graph::operations::minimize::pseudo");
#endif

  // Node count as stop condition
  i64 begin_node_count{};

  // Pseudo number index
  i64 counter{};

  // Define function to compare values lt 0
  auto f_lowest = [&](auto e)
  {
    if(e < counter){ counter=e; } ++begin_node_count; return false;
  };

  // Get the dummy vertex with the lowest value
  ns_search::bfs::run(root,f_pred,f_succ,f_lowest);

  // A struct to control the value of pseudo nodes
  auto make_pseudo = [counter]
  {
    struct Pseudo
    {
      private:
        i64 c;
      public:
        Pseudo(i64 _c) : c(_c) {}
        i64 next(){ return --c; }
        i64 curr(){ return c; }
    };
    return Pseudo{counter};
  }();

  // Get topologic layers
  auto view_depth { ns_views::depth::run(root,f_pred,f_succ).ln };

  // Constant to control merge of nodes
  constexpr i8 const c{2};

  // Chech if successors of each layer are prone to merging
  for (size_t i{}; i < view_depth.size(); ++i)
  {
    // Current topologic layer
    auto layer{view_depth.at(i)};

    for (auto node : layer)
    {
      auto succs{f_succ(node)};
      // All nodes are pseudo nodes
      bool all_pseudo{rg::all_of(succs,[](auto s){ return s < 0; })};
      // All nodes have lte C combined successors
      auto succ_count
      {
        rg::accumulate(succs,0,[&](auto ac,auto e){ return ac+f_succ(e).size(); })
      };
      // Check if conditions are met, than merge, otherwise go to next node
      if ( (succs.size() > 1) && all_pseudo && (succ_count <= c))
      {
        // Create new node
        auto new_node{make_pseudo.next()};

        // Link to common predecessor
        f_link(std::make_pair(node,new_node));

        // Connect new node to successors of successors
        for (auto s : succs)
        {
          for( auto _s : f_succ(s) ){ f_link(std::make_pair(new_node,_s)); }
        } // for

        // Remove old successors, leaving only new_node
        for (auto s : succs)
        {
          for( auto _s : f_succ(s) ){ f_unlink(std::make_pair(s,_s)); }
          f_unlink(std::make_pair(node,s));
        } // for

      } // if
    } // for

    // Update layers
    view_depth = ns_views::depth::run(root,f_pred,f_succ).ln;

    // Check if another pass is required
    if (i+1 == view_depth.size())
    {
      i64 end_node_count{};
      ns_search::bfs::run(root,f_pred,f_succ,
        [&](auto){ ++end_node_count; return false;}
      );
      if( begin_node_count != end_node_count )
      {
        begin_node_count = end_node_count;
        i=0;
      }
    } // if

  } // for

} // }}}

// fn: run {{{
template<SignedIntegral T>
void run(T root, Ops ops)
{
  run(root, ops.preds, ops.succs, ops.link, ops.unlink);
}
// }}}

} // namespace celaeno::graph::operations::minimize::pseudo
