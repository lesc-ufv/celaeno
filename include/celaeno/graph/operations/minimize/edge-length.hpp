//
// @author      : Ruan E. Formigoni (ruanformigoni@gmail.com)
// @file        : edge-length
// @created     : tuesday mar 09, 2021 19:10:18 -03
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

#include <range/v3/all.hpp>

#ifndef NDEBUG
  #include <spdlog/spdlog.h>
#endif

#include <celaeno/concepts.hpp>
#include <celaeno/aliases.hpp>
#include <celaeno/graph/search/bfs.hpp>
#include <celaeno/graph/views/depth.hpp>

namespace celaeno::graph::operations::minimize::edge_length
{

// Namespaces {{{
namespace ns_search = celaeno::graph::search;
namespace ns_views = celaeno::graph::views;
// }}}

// Using declarations {{{
using namespace celaeno::aliases;
using namespace celaeno::concepts;
// }}}

// fn: run {{{
template<SignedIntegral T, typename P,typename S,typename L, typename U, typename D>
void run(T root, P&& f_pred, S&& f_succ, L&& f_link, U&& f_unlink, D&& f_dist)
  requires CallableWith<P,i64>
  && CallableWith<S,i64>
  && CallableWith<L,std::pair<i64,i64>>
  && CallableWith<U,std::pair<i64,i64>>
{

#if ! defined(NDEBUG) && defined(DEBUG_SHOW_ALG)
  spdlog::set_level(spdlog::level::debug);
  spdlog::debug("Algorithm: celaeno::graph::operations::minimize::edge_length");
#endif

  // Pseudo number index
  i64 counter{};

  // Define function to compare values lt 0
  auto f_lowest = [&counter](auto&& e) { if(e < counter){ counter=e; } return false; };

  // Get the dummy vertex with the lowest value
  ns_search::bfs::run(root,f_pred,f_succ,f_lowest);

  // Create depth view
  auto [view_id_layer,view_vertex_depth] = ns_views::depth::run(root, f_pred, f_succ);

  // For the current level, check horizontal distances of each vertex against
  // the ones in its outgoing edges
  for (auto const& [_,nodes] : view_id_layer)
  {
    for (auto n : nodes)
    {
      for (auto s : f_succ(n))
      {
        // Distance between nodes n,s
        auto dist = f_dist(n,s);

        // Predecessors of n
        auto preds{f_pred(n)};

        if( dist > 1 && (! preds.empty()) )
        {
          // Find predecessor of n, which is not a pseudo vertex
          // TODO improve this
          auto pred{n};

          while( ! preds.empty() )
          {
            if( pred < 0 )
            {
              preds = f_pred(pred);
              // There will always be a predecessor if current node is negative
              assertm("Negative node as root", ! preds.empty() );
              pred = preds.at(0);
            } // if
            else
            {
              break;
            } // else
          }

          // Find a pseudo vertex successor of pred that is closest to s
          // with a greedy heuristic
          auto closest{n};

          for ( auto _n : f_succ(pred) )
          {
            // Make sure it preceeds s
            if( view_vertex_depth[_n] >= view_vertex_depth[s] ){ break; }
            // Calculate distances
            if ( f_dist(_n,s) < f_dist(n,s) )
            {
              closest = _n;
            } // if
          } // for

          // Update graph
          if (closest != n)
          {
            f_unlink(std::make_pair(n,s));
            f_link(std::make_pair(closest,s));
          } // if
        } // if
      } // for
    } // for
  } // for
} // function: run }}}

} // namespace celaeno::graph::operations::minimize::edge_length
