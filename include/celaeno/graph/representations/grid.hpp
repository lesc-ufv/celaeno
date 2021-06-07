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

// struct: Tile {{{
struct Tile
{
  i32 x, y;
  Tile() = default;
  Tile(i32 x,i32 y) : x(x), y(y) {}
}; // }}}

// Using declarations {{{
using Ops = celaeno::graph::Ops;
using MapVertexTile = std::map<i64,Tile>;
using VertexPoint = std::pair<i64,Tile>;
// }}}

// Using namespaces {{{
using namespace celaeno::aliases;
using namespace celaeno::concepts;
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

// function: root_reachable {{{
template<SignedIntegral T>
auto filter_not_reachable(T root, Ops const& ops)
{
  auto f_nop = [](auto){ return std::vector<T>{}; };

  // Collect all nodes reachable from root
  auto successors{search::bfs::run(root, f_nop, ops.succs)};

  // Get layers
  // auto layers {ns_minimize::crossings::run(root,f_nop,f_succ,f_adj,f_link,f_unlink)};
  auto depth_view{ns_views::depth::run(root, ops.preds, ops.succs).first};
  auto layers = depth_view
    | rv::transform([](auto&& e){ return e.second; })
    | rg::to<std::vector<std::vector<i64>>>;

  // Remove non-reachable from root of each layer
  for (size_t i{}; i < layers.size(); ++i)
  {
    layers[i] = fw::apply(layers[i], fw::keep_if([&](auto e){ return fp::is_elem_of(e,successors); }));
  } // for

  return std::make_pair(layers,successors);
} // }}}

// fn: tree_like {{{

//
// Tree like placement, given a root node
//
template<SignedIntegral T>
decltype(auto) tree_like(T root, size_t slots, Ops const& ops)
{
  auto [layers,root_reachable] {filter_not_reachable(root,ops)};

  // Save vertices positions
  MapVertexTile m_vertex_tile;

  // Find level with highest number of vertices
  auto const it_base {rg::max_element(layers, {}, [](auto e){ return e.size(); })};

  // Place vertex u in coordinate {x,y}
  auto f_place = [&](auto x, auto y, auto u) { m_vertex_tile.emplace(u,Tile(x,y)); };

  // Index of it_base in layers container
  u64 idx_base{static_cast<u64>(std::distance(layers.begin(),it_base))};

  // Extra y-spacing between layers
  i64 y_extra{1};

  // Place level with higher number of vertices
  rg::for_each(*it_base, [&,x=0,y=idx_base](auto u) mutable { f_place(x*slots,y*(slots+y_extra),u); ++x; });

  // Save occupation of x positions for each layer
  std::set<i64> occupation;

  // Calculate the mean of the predecessors/successors positions
  auto leftmost_pos = [&]<Range R>(R const& vs, auto node) -> std::optional<u64>
  {
    if (vs.empty())
    {
      spdlog::warn("Dangling node {} will be ignored!", node);
      return std::nullopt;
    } // if

    auto it_min_element{rg::min_element(vs,{},[&](auto u){return m_vertex_tile.at(u).x;})};

    if( it_min_element == rg::end(vs) )
    {
      spdlog::warn("Unknown error in leftmost_pos");
      return std::nullopt;
    } // if

    return m_vertex_tile[*it_min_element].x;
  };

  //
  // Place subsequent layers with respect to first positioned layers, e.g, given
  // [1..5] layers, if first positioned layer was 3, then subsequent layers
  // [3,4],[4,5] must be positioned sequentially in this order; and layers
  // [3,2],[2,1] must be positioned sequentially in this order.
  //

  // First half of positions for placement
  // Reverse container and elements
  auto first_half {ra::reverse(fp::numbers(u64{},idx_base))};

  // Second half of positions for placement
  auto second_half {fp::numbers(idx_base+1,layers.size())};

  for (auto y : fp::append(second_half,first_half))
  {
    for (auto u : layers.at(y))
    {
      // Get preds and succs and Keep only root reachable elements
      auto preds {fp::keep_if([&,root_reachable=root_reachable](auto e){ return fp::is_elem_of(e,root_reachable); },ops.preds(u))};
      auto succs {fp::keep_if([&,root_reachable=root_reachable](auto e){ return fp::is_elem_of(e,root_reachable); },ops.succs(u))};

      // Set the x position to leftmost the predecessors positions
      auto x { (y > idx_base)? leftmost_pos(preds,u) : leftmost_pos(succs,u) };

      // Check if position is valid
      if( !x ){ continue; }

      // Mark position as used
      occupation.emplace(*x);

      // Position the vertex
      f_place(*x,y*(slots+y_extra),u);
    } // for
    occupation.clear();
  } // for

  return m_vertex_tile;
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

  MapVertexTile solution;

  for (auto it{root_nodes.begin()}; it != root_nodes.end(); ++it)
  {
    if( it == root_nodes.begin() ){ solution = tree_like(*it, slots, ops); continue; }

    // Get iterator to previous solution
    auto curr_solution{tree_like(*it, slots, ops)};

    // Find intersecting nodes between current and previous solution
    std::vector<VertexPoint> intersection;
    rg::set_intersection(solution,curr_solution,std::back_inserter(intersection),[](auto u, auto v){ return u.first < v.first; });

    // Transform intersection in absolute x-pos differences
    auto differences{ fp::transform([&](auto u){ return std::abs(solution[u.first].x-curr_solution[u.first].x); },intersection) };

    // Get the most repeated difference value
    auto difference{fp::maximum_by([](auto u, auto v){ return u.second < v.second; }, fp::count_occurrences(differences)).first};

    // Get index of curr_slot, which is the index of the current root element
    auto curr_slot{std::distance(root_nodes.begin(),it)};

    // Sum the diff with x-val of all elements of current solution
    rg::for_each(curr_solution, [&](auto& e){ e.second.x += difference+curr_slot; });

    // Merge curr_solution with solution
    for (auto [vertex,tile] : curr_solution)
    {
      if( ! solution.contains(vertex) ){ solution.emplace(vertex,tile); }
    } // for
  } // for

  return solution;
} // }}}

// function: route {{{
template<SignedIntegral T>
decltype(auto) route(T root, Ops const& ops, MapVertexTile const& m_vertex_tile)
{
  // // Find root nodes
  // auto root_nodes{fp::keep_if([&](auto e){ return ops.preds(e).empty(); },ns_search::bfs::run(root,ops))};
  //
  // auto [layers,root_reachable] {filter_not_reachable(root,ops)};
} // }}}

} // namespace celaeno::graph::representations::grid }}}
