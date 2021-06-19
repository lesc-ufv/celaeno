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
#include <mutex>
#include <range/v3/all.hpp>
#include <fplus/fplus.hpp>
#include <compare>

#include <spdlog/spdlog.h>
#include <spdlog/sinks/basic_file_sink.h>

#include <celaeno/aliases.hpp>
#include <celaeno/concepts.hpp>
#include <celaeno/heuristics/manhattan.hpp>
#include <celaeno/graph/graph.hpp>
#include <celaeno/graph/search/bfs.hpp>
#include <celaeno/graph/search/a-star.hpp>
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
  i64 x, y;
  Tile() = default;
  Tile(i32 x,i32 y) : x(x), y(y) {}

  auto operator<=>(Tile const& rhs) const = default;
  std::pair<i64,i64> to_pair() const
  {
    return std::pair<i64,i64>(x,y);
  }
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
namespace ns_heuristics = celaeno::heuristics;
// }}}

// function: root_reachable {{{
template<SignedIntegral T, typename L>
auto filter_not_reachable(T root, Ops const& ops, L layers)
{
  auto f_nop = [](auto){ return std::vector<T>{}; };

  // Collect all nodes reachable from root
  auto successors{search::bfs::run(root, f_nop, ops.succs)};

  // Remove non-reachable from root of each layer
  for (size_t i{}; i < layers.size(); ++i)
  {
    layers[i] = fw::apply(layers[i], fw::keep_if([&](auto e){ return fp::is_elem_of(e,successors); }));
  } // for

  return std::make_pair(layers,successors);
} // }}}

// function: area {{{
template<Map M>
decltype(auto) area(M const& m)
{
  return std::make_pair(
    rg::max_element(m, {}, [](auto e){ return e.second.x; })->second.x
    , rg::max_element(m, {},[](auto e){ return e.second.y; })->second.y
  );
} // }}}

// fn: subgraph {{{
template<SignedIntegral T, typename L>
decltype(auto) subgraph(T root, size_t slots, Ops const& ops, L layers, u64 idx_base)
{
  auto [filtered_layers,root_reachable] {filter_not_reachable(root,ops,layers)};

  // Save vertices positions
  MapVertexTile m_vertex_tile;

  // Find level with highest number of vertices
  // auto const it_base {rg::max_element(layers, {}, [](auto e){ return e.size(); })};
  auto const it_base {filtered_layers.begin()+idx_base};

  auto calc_new_y = [&](u64 y)
  {
    if( y == 0 ){ return u64{}; }

    u64 new_y{};
    rg::for_each(fp::numbers(u64{},y), [&](auto _y)
    {
      // new_y += layers.at(_y).size();
      new_y += fp::reduce([&](auto acc, auto u){ return acc+ops.succs(u).size(); },0,layers.at(_y));
    });
    return new_y+1;
  };


  // Place vertex u in coordinate {x,y}
  auto f_place = [&](auto x, auto y, auto u) { m_vertex_tile.emplace(u,Tile(x,y)); };

  // Place level with higher number of vertices
  rg::for_each(*it_base, [&,x=0,y=idx_base](auto u) mutable { f_place(x*slots,calc_new_y(y),u); ++x; });

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
  auto first_half {ra::reverse(fp::numbers(u64{},idx_base))};
  // Second half of positions for placement
  auto second_half {fp::numbers(idx_base+1,filtered_layers.size())};

  for (auto y : fp::append(second_half,first_half))
  {
    auto new_y{calc_new_y(y)};

    for (auto u : filtered_layers.at(y))
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
      f_place(*x,new_y,u);

    } // for
    occupation.clear();
  } // for

  return m_vertex_tile;
} // function: subgraph }}}

// fn: overlap_nodes {{{


// fn: run {{{
template<SignedIntegral T, typename L>
auto run(T root, Ops const& ops, L const& layers)
{
#ifndef NDEBUG
  spdlog::set_level(spdlog::level::debug);
  spdlog::debug("Algorithm: celaeno::graph::representations::grid");
#endif

  for (auto const& layer : layers)
  {
    fmt::print("{}\n", layer);
  } // for

  //
  // Find root nodes
  //
  auto root_nodes{fp::keep_if([&](auto e){ return ops.preds(e).empty(); },ns_search::bfs::run(root,ops))};

  auto const it_base {rg::max_element(layers, {}, [](auto e){ return e.size(); })};

  auto i_base{std::distance(layers.begin(),it_base)};

  //
  // Determined number of slots on each node
  //
  auto slots{root_nodes.size()};
  fmt::print("root_nodes: {}\n", root_nodes);

  //
  // Process each subgraph concurrently
  //
  std::vector<MapVertexTile> solutions;

  // Keep a mutex to control insertions in solutions vector
  std::mutex mutex_solutions;

  // Solution insertion lambda
  auto add_to_solutions = [&]<typename S>(S&& solution)
  {
    std::lock_guard<std::mutex> guard(mutex_solutions);
    solutions.emplace_back(std::forward<S>(solution));
  };

  std::map<u64,Tile> map_root_pos;
  for (u64 slot_offset{}; auto root_node : root_nodes)
  {
    auto solution{subgraph(root_node, slots, ops, layers, i_base)};
    add_to_solutions(solution);
    map_root_pos[root_node] = solution.at(root_node);
    ++slot_offset;
  } // for

  // std::vector<std::thread> jobs;
  //
  // for (u64 slot_offset{}; auto root_node : root_nodes)
  // {
  //   jobs.emplace_back([=] { add_to_solutions(subgraph(root_node, slots, slot_offset, ops, layers, i_base)); } );
  //   ++slot_offset;
  // } // for
  //
  // for (auto& job : jobs) { job.join(); } // for

  //
  // Merge solutions
  //

  rg::sort(root_nodes,{},[&](auto u) { return map_root_pos.at(u).x; });

  // Final merged solution
  MapVertexTile solution;

  for (auto it{solutions.begin()}; it != solutions.end(); ++it)
  {
    if( it == solutions.begin()) { solution = *it; continue; }

    // Get iterator to previous solution
    auto curr_solution{*it};

    // Find intersecting nodes between current and previous solution
    std::vector<VertexPoint> intersection;
    rg::set_intersection(solution,curr_solution,std::back_inserter(intersection),[](auto u, auto v){ return u.first < v.first; });

    // Transform intersection in absolute x-pos differences
    auto differences{ fp::transform([&](auto u){ return std::abs(solution[u.first].x-curr_solution[u.first].x); },intersection) };

    // Get the most repeated difference value
    auto difference{fp::maximum_by([](auto u, auto v){ return u.second < v.second; }, fp::count_occurrences(differences)).first};

    // Sum the diff with x-val of all elements of current solution
    rg::for_each(curr_solution, [&](auto& e)
    {
      // Approximate intersection of subgraphs
      e.second.x += difference;

      // Adjust subgraph x-values to avoid node overlaps
      auto offset = -(e.second.x % root_nodes.size()) + std::distance(solutions.begin(),it);

      if ( rg::find(root_nodes, e.first) != rg::end(root_nodes) )
      {
        fmt::print("Node: {} pos: {} i: {} offset:{} \n", e.first, e.second.x, std::distance(solutions.begin(),it), offset);
      } // if

      e.second.x += offset;
      if ( rg::find(root_nodes, e.first) != rg::end(root_nodes) )
      {
        fmt::print("Node: {} pos: {} i: {}\n", e.first, e.second.x, std::distance(solutions.begin(),it));
      } // if
    });

    // Merge curr_solution with solution
    for (auto const& [vertex,tile] : curr_solution)
    {
      // Get the solution with leftmost x-value
      if( ! solution.contains(vertex) ){ solution.emplace(vertex,tile); }
      else if (solution.at(vertex).x < tile.x)
      {
        solution.at(vertex) = tile;
      } // else if
      // solution.emplace(vertex,tile);
    } // for
  } // for

  // //
  // // Include outgoing edge offset
  // //
  // for (i64 i{}; auto layer : layers)
  // {
  //   layer = fp::sort_by([&](auto u, auto v){ return solution.at(u).x < solution.at(v).x; },layer);
  //
  //   fmt::print("Layer {}: {}\n", i++, layer);
  //
  //   // fmt::print("Layer: {}\n", layer);
  //   // fmt::print("Offsets: ");
  //   for (i64 x_offset{}; auto u : layer)
  //   {
  //     solution.at(u).x += x_offset;
  //     // fmt::print("{} ", x_offset);
  //
  //     x_offset += (ops.succs(u).size() > 1)? 1 : 0;
  //   } // for
  //
  //   // fmt::print("\n");
  // } // for

  return solution;
} // }}}

// function: route {{{
template<typename L>
decltype(auto) route(Ops const& ops, MapVertexTile const& m_vertex_tile, L layers)
{
  std::map< std::pair<Tile,Tile>, std::deque<std::pair<i64,i64>> > paths;

  for (auto layer : layers)
  {
    layer = fp::sort_by([&](auto u, auto v){ return m_vertex_tile.at(u).x < m_vertex_tile.at(v).x; },layer);

    for (i64 idx_node{1}; auto const& node : layer)
    {
      auto succs{ops.succs(node)};

      // Sort successors by closest manhattan distance
      rg::sort(succs,{},[&](auto u){ return ns_heuristics::manhattan::run(m_vertex_tile.at(node).to_pair(), m_vertex_tile.at(u).to_pair()); });

      for (i64 i_succ{}; auto const& succ : succs)
      {
        // Create begin/end pair
        auto tiles {std::make_pair(m_vertex_tile.at(node),m_vertex_tile.at(succ))};

        // Check if tiles were already processed
        if( paths.contains(tiles) ) { continue; }

        auto [t1,t2] = std::make_pair(tiles.first.to_pair(),tiles.second.to_pair());

        spdlog::info("Nodes: {},{} idx: {} Source: {} Dest: {}\n", node, succ, idx_node, t1, t2);

        // Create path
        std::deque<std::pair<i64,i64>> path;

        auto y_i{t1.second};
        auto y_m{t1.second+idx_node};
        auto y_f{t2.second};
        auto x_i{t1.first};
        auto x_f{t2.first};

        // Adjust y-col for next successor
        if(i_succ != 0)
        {
          // Include left-right adjustment
          path.emplace_back(x_i,y_i);
          // Update new x-pos
          x_i += 1;
          // Update idx_node
          ++idx_node;
          // Update y_m
          y_m = t1.second+idx_node;
        } // if

        // Vert. in source col
        for (auto y : fp::numbers(y_i,y_m+1))
        {
          path.emplace_back(x_i,y);
        } // for

        // Hor. to dest. col
        for (i64 i{}; auto x : fp::numbers(x_i,x_f+1))
        {
          if( i++ == 0 ){ continue; }

          path.emplace_back(x,y_m);
        } // for

        // Vert. in dest. col
        for (auto y : fp::numbers(y_m,y_f+1))
        {
          path.emplace_back(x_f,y);
        } // for

        path = fp::unique(path);

        paths.emplace(tiles, path);

        ++i_succ;
      } // for
      ++idx_node;
    } // for
  } // for

  // // Find root nodes
  //
  // auto root_nodes{fp::keep_if([&](auto e){ return ops.preds(e).empty(); },ns_search::bfs::run(root,ops))};
  //
  // // For each root node, route edges reachable from it
  //
  // for (T root_node : root_nodes)
  // {
  //   auto [filtered_layers,root_reachable] {filter_not_reachable(root_node,ops,layers)};
  //
  //   for (auto const& layer : filtered_layers)
  //   {
  //     for (i64 idx_node{1}; auto const& node : layer)
  //     {
  //       for (i64 i_succ{}; auto const& succ : ops.succs(node))
  //       {
  //         // Create begin/end pair
  //         auto tiles {std::make_pair(m_vertex_tile.at(node),m_vertex_tile.at(succ))};
  //
  //         // Check if tiles were already processed
  //         if( paths.contains(tiles) ) { continue; }
  //
  //         auto [t1,t2] = std::make_pair(tiles.first.to_pair(),tiles.second.to_pair());
  //
  //         spdlog::info("Nodes: {},{} idx: {} Source: {} Dest: {}\n", node, succ, idx_node, t1, t2);
  //
  //         // Create path
  //         std::deque<std::pair<i64,i64>> path;
  //
  //         auto y_i{t1.second};
  //         auto y_m{t1.second+idx_node};
  //         auto y_f{t2.second};
  //         auto x_i{t1.first};
  //         auto x_f{t2.first};
  //
  //         // Adjust y-col for next successor
  //         if(i_succ != 0)
  //         {
  //           // Include left-right adjustment
  //           path.emplace_back(x_i,y_i);
  //           // Update new x-pos
  //           x_i += 1;
  //         } // if
  //
  //         // Vert. in source col
  //         for (auto y : fp::numbers(y_i,y_m+1))
  //         {
  //           path.emplace_back(x_i,y);
  //         } // for
  //
  //         // Hor. to dest. col
  //         for (i64 i{}; auto x : fp::numbers(x_i,x_f+1))
  //         {
  //           if( i++ == 0 ){ continue; }
  //
  //           path.emplace_back(x,y_m);
  //         } // for
  //
  //         // Vert. in dest. col
  //         for (auto y : fp::numbers(y_m,y_f+1))
  //         {
  //           path.emplace_back(x_f,y);
  //         } // for
  //
  //         path = fp::unique(path);
  //
  //         paths.emplace(tiles, path);
  //
  //         ++i_succ;
  //       } // for
  //       ++idx_node;
  //     } // for
  //   } // for
  // } // for

  return paths;


} // }}}

} // namespace celaeno::graph::representations::grid }}}
