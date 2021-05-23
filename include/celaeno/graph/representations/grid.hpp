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
#include <celaeno/graph/graph.hpp>
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
using Ops = celaeno::graph::Ops;
// }}}

// namespaces {{{
namespace rg = ranges;
namespace rv = ranges::views;
namespace ra = ranges::actions;
namespace fp = fplus;
namespace fw = fplus::fwd;
namespace ns_minimize = celaeno::graph::operations::minimize;
namespace ns_search = celaeno::graph::search;
namespace ns_views = celaeno::graph::views;
// }}}

// fn: tree_like {{{

//
// Tree like placement, given a root node
//
template<SignedIntegral T>
decltype(auto) tree_like(T root, size_t slots, i64 curr_slot, Ops const& ops)
{
  auto f_nop = [](auto){ return std::vector<T>{}; };

  // Collect all nodes reachable from root
  auto root_reachable{search::bfs::run(root, f_nop, ops.succs)};

  // Get layers
  // auto layers {ns_minimize::crossings::run(root,f_nop,f_succ,f_adj,f_link,f_unlink)};
  auto depth_view{ns_views::depth::run(root, ops.preds, ops.succs).first};
  auto layers = depth_view
    | rv::transform([](auto&& e){ return e.second; })
    | rg::to<std::vector<std::vector<i64>>>;

  // Remove non-reachable from root of each layer
  for (size_t i{}; i < layers.size(); ++i)
  {
    layers[i] = fw::apply(layers[i], fw::keep_if([&](auto e){ return fp::is_elem_of(e,root_reachable); }));
  } // for

  // Save vertices positions
  std::map<i64,std::pair<i64,i64>> vertex_xy;

  // Find level with highest number of vertices
  auto const it_base {rg::max_element(layers, {}, [](auto e){ return e.size(); })};

  // Place vertex u in coordinate {x,y}
  auto f_place = [&](auto x, auto y, auto u) { vertex_xy[u] = std::make_pair(x,y); };

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
      // Get preds and succs and Keep only root reachable elements
      auto preds {fp::keep_if([&](auto e){ return fp::is_elem_of(e,root_reachable); },ops.preds(u))};
      auto succs {fp::keep_if([&](auto e){ return fp::is_elem_of(e,root_reachable); },ops.succs(u))};

      // Set the x position to a mean of the predecessors positions
      auto x { (y > idx_base)? mean_of_pos(preds,u) : mean_of_pos(succs,u) };

      // Check if position is valid
      if( !x ){ continue; }

      // Mark position as used
      occupation.emplace(*x);

      // Position the vertex
      f_place(*x,y,u);
    } // for
    occupation.clear();
  } // for

  return std::make_pair(layers,vertex_xy);
} // function: tree_like }}}

// fn: overlap_nodes {{{


// fn: run {{{
template<SignedIntegral T>
auto run(T root, Ops const& ops)
{
#ifndef NDEBUG
  spdlog::set_level(spdlog::level::debug);
  spdlog::debug("Algorithm: celaeno::graph::representations::grid");
#endif

  // Find root nodes
  auto root_nodes{fp::keep_if([&](auto e){ return ops.preds(e).empty(); },ns_search::bfs::run(root,ops))};

  auto slots{root_nodes.size()};

  auto sol_0{tree_like(root_nodes.at(0), slots, 0, ops)};

  std::vector<decltype(sol_0)> solutions;

  solutions.push_back(std::move(sol_0));

  root_nodes.erase(root_nodes.begin());

  for (i64 i{1}; auto root_node : root_nodes)
  {
    solutions.push_back(tree_like(root_node, slots, i++, ops));
  } // for

  return solutions;
}
// }}}

} // namespace celaeno::graph::representations::grid }}}
