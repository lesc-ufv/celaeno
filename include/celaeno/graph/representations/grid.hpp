// vim: set expandtab fdm=marker ts=2 sw=2 tw=80 et :
//
// @author      : Ruan E. Formigoni (ruanformigoni@gmail.com)
// @file        : grid
// @created     : thrusday feb 25, 2021 16:06:04 -03
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
#include <queue>
#include <set>
#include <range/v3/all.hpp>
#include <fplus/fplus.hpp>

#include <spdlog/spdlog.h>

#include <celaeno/aliases.hpp>
#include <celaeno/concepts.hpp>
#include <celaeno/graph/search/bfs.hpp>
#include <celaeno/graph/search/kahn.hpp>
#include <celaeno/graph/views/depth.hpp>
#include <celaeno/graph/operations/minimize/edge-length.hpp>
#include <celaeno/graph/operations/minimize/crossings.hpp>

// namespace celaeno::graph::representations::grid {{{
namespace celaeno::graph::representations::grid
{

// Using declarations {{{
using namespace celaeno::aliases;
using namespace celaeno::concepts;
// }}}

// namespaces {{{
namespace rg = ranges;
namespace ra = ranges::actions;
namespace fp = fplus;
namespace ns_minimize = celaeno::graph::operations::minimize;
namespace ns_search = celaeno::graph::search;
// }}}

// fn: place {{{

//
// Crossing minimization oriented coordinates
//
template<SignedIntegral T, typename P, typename S, typename A, typename L, typename U>
decltype(auto) place(T&& root, P&& f_pred, S&& f_succ, A&& f_adj, L&& f_link, U&& f_unlink)
{
  // Get layers
  auto layers {ns_minimize::crossings::run(root,f_pred,f_succ,f_adj,f_link,f_unlink)};

  // Save vertices positions
  std::map<i64,std::pair<i64,i64>> vertex_xy;

  // Find level with highest number of vertices
  auto const it_base {rg::max_element(layers, {}, [](auto e){ return e.size(); })};

  // Place vertex u in coordinate {x,y}
  auto f_place = [&](auto x, auto y, auto u) { vertex_xy.insert({u, {x,y}}); };

  // Index of it_base in layers container
  u64 idx_base{static_cast<u64>(std::distance(layers.begin(),it_base))};

  // Place level with higher number of vertices
  rg::for_each(*it_base, [&,x=0,y=idx_base](auto u) mutable { f_place(x++,y,u); });

  // Save occupation of x positions for each layer
  std::set<i64> occupation;

  // Calculate the mean of the predecessors/successors positions
  auto mean_of_pos = [&]<Range R>(R const& vs, auto node) -> std::optional<u64>
  {
    if (vs.empty())
    {
      spdlog::warn("Dangling node {} will be ignored!", node);
      return std::nullopt;
    } // if
    auto pos {rg::accumulate(vs,0,{},[&](auto u){return vertex_xy[u].first;}) / vs.size()};
    while( occupation.contains(pos) ) { ++pos; }
    return pos;
  };

  //
  // Place subsequent layers with respect to first positioned layers, e.g, given
  // [1..5] layers, if first positioned layer was 3, then subsequent layers
  // [3,4],[4,5] must be positioned sequentially in this order; and layers
  // [3,2],[2,1] must be positioned sequentially in this order.
  //

  // First half of positions for placement
  auto first_half {fp::numbers(u64{},idx_base)};

  // Reverse container and elements
  first_half = ra::reverse(first_half);

  // Second half of positions for placement
  auto second_half {fp::numbers(idx_base+1,layers.size())};

  for (auto y : fp::append(first_half,second_half))
  {
    for (auto u : layers.at(y))
    {
      // Set the x position to a mean of the predecessors positions
      auto x { (y > idx_base)? mean_of_pos(f_pred(u),u) : mean_of_pos(f_succ(u),u) };
      // Check if position is valid
      if( !x ){ continue; }
      // Save the position
      vertex_xy.insert({u,{*x,y}});
      // Mark position as used
      occupation.emplace(*x);
      // Position the vertex
      f_place(*x,y,u);
    } // for
    occupation.clear();
  } // for

  return std::make_pair(layers,vertex_xy);
} // function: place }}}

// fn: minimize_edge_distance {{{

//
// Shifts nodes in layer to minimize long edges
//
template<SignedIntegral T, typename P, typename S, typename A, typename L, typename U>
decltype(auto) minimize_edge_distance(T&& root, P&& f_pred, S&& f_succ, A&& f_adj, L&& f_link, U&& f_unlink)
{
  auto grid {place(root, f_pred, f_succ, f_adj, f_link, f_unlink)};
  auto& layers{grid.first};
  auto& vertex_xy{grid.second};

  auto f_dist = [&](auto u, auto v)
  {
    return std::abs(vertex_xy[u].first-vertex_xy[v].first);
  };

  //
  // Queue to keep elements with nodes that have incoming or outgoing edges with
  // distance greater than 1
  //

  std::queue<std::decay_t<T>> q;

  //
  // Use a topo search to find edges with dist greater than 1, and populate the queue
  //

  ns_search::kahn::run(root, f_pred, f_succ,
  [&](auto u)
  {
    for (auto v : f_succ(u))
    {
      auto ux{vertex_xy.at(u).first};
      auto& vx{vertex_xy.at(v).first};
      if( f_dist(u,v) > 1 )
      {
        q.push(v);
        vx = (vx > ux)? ux+1 : ux-1;
      }
    } // for
    return false;
  });

  //
  // Keep processing until all nodes of the graph have an edge distance of 1
  //

  while( ! q.empty() )
  {
    auto u{q.front()}; q.pop();

    for (auto v : f_succ(u))
    {
      auto ux{vertex_xy.at(u).first};
      auto& vx{vertex_xy.at(v).first};
      if ( f_dist(u,v) > 1 )
      {
        vx = ( vx > ux )? ( q.push(v), ux+1) : ( q.push(v), ux-1);
      } // if
    } // for

    for (auto v : f_pred(u))
    {
      auto ux{vertex_xy.at(u).first};
      auto& vx{vertex_xy.at(v).first};
      if ( f_dist(u,v) > 1 )
      {
        vx = ( vx > ux )? (q.push(v), ux+1) : (q.push(v), ux-1);
      } // if
    } // for

  } // while

  return grid;

} // function: minimize_edge_distance }}}

// fn: fill_paths_with_nodes {{{

//
// Inserts intra layer pseudo nodes for binary tree-like drawing
//
template<SignedIntegral T, typename P, typename S, typename A, typename L, typename U>
void fill_paths_with_nodes(T&& root, P&& f_pred, S&& f_succ, A&& f_adj, L&& f_link, U&& f_unlink)
{
  auto grid {place(root, f_pred, f_succ, f_adj, f_link, f_unlink)};
  auto layers{grid.first};
  auto vertex_xy{grid.second};

  auto f_dist = [&](auto u, auto v)
  {
    return std::abs(vertex_xy[u].first-vertex_xy[v].first);
  };

  // Pseudo number index
  i64 counter{};

  // Define function to compare values lt 0
  auto f_lowest = [&](auto e)
  {
    if(e < counter){ counter=e; } return false;
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
        Pseudo(i64 c) : c(c) {}
        i64 next(){ return --c; }
        i64 curr(){ return c; }
    };
    return Pseudo{counter};
  }();

  // Find distances of each level
  for (i64 i{}; auto const& layer : layers)
  {

    if( static_cast<u64>(i+1) == layers.size() ){ break; }

    // Find max x distance
    i64 x_max{};
    for (auto node : layer)
    {
      for (auto succ : f_succ(node))
      {
        if( auto dist{f_dist(node,succ)}; dist > x_max ){ x_max = dist; }
      } // for
    } // for

    if (x_max > 1)
    {
      for (auto node : layers.at(i))
      {
        for (auto succ : f_succ(node))
        {
          auto curr{node};
          for (i64 j{}; j < std::ceil(static_cast<f64>(x_max)/2); ++j)
          {
            auto new_node{make_pseudo.next()};
            f_link(std::make_pair(curr,new_node));
            f_link(std::make_pair(new_node,succ));
            f_unlink(std::make_pair(curr,succ));
            curr=new_node;
          } // for: i < x_max/2
        } // for
      } // for
    } // if x_max > 1
    ++i;
  } // for
} // function: fill_paths_with_nodes }}}

// fn: run {{{
template<SignedIntegral T, typename P, typename S, typename A, typename L, typename U>
auto run(T&& root, P&& f_pred, S&& f_succ, A&& f_adj, L&& f_link, U&& f_unlink)
{
#ifndef NDEBUG
  spdlog::set_level(spdlog::level::debug);
  spdlog::debug("Algorithm: celaeno::graph::representations::grid");
#endif

  //
  // Minimize edge length by pseudo-node relinking
  //

  auto grid {place(root, f_pred, f_succ, f_adj, f_link, f_unlink)};

  auto f_dist = [&](auto u, auto v)
  {
    grid = place(root, f_pred, f_succ, f_adj, f_link, f_unlink);
    auto layers = grid.first;
    auto vertex_xy = grid.second;
    return std::abs(vertex_xy[u].first-vertex_xy[v].first);
  };

  ns_minimize::edge_length::run(root,f_pred,f_succ,f_link,f_unlink,f_dist);

  fill_paths_with_nodes(root, f_pred, f_succ, f_adj, f_link, f_unlink);

  return minimize_edge_distance(root, f_pred, f_succ, f_adj, f_link, f_unlink);
}
// }}}

} // namespace celaeno::graph::representations::grid }}}
