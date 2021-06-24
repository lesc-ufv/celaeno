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

// enum class: TileType {{{
//
// @Type 1 : Node                   : ND
// @Type 2 : Tile Left → Right      : LR
// @Type 3 : Tile Left → Down       : LD
// @Type 4 : Tile Up → Down         : UD
// @Type 5 : Tile Up → Right        : UR
// @Type 6 : Tile Up → Right & Down : URD
//
enum class TileType
{
  ND,
  LR,
  LD,
  UD,
  UR,
  URD,
};
// }}}

// struct: Tile {{{
//
// Represents a unit of area in the positions grid
//
struct Tile
{
  // Public Members
  public:
  i64 x, y;
  TileType type;
  // Constructors
  public:
  Tile() = default;
  Tile(i32 x, i32 y, TileType type) : x(x), y(y), type(type) {}
  // Public Methods
  // // Operations
  std::pair<i64,i64> to_pair() const;
  // // Operators
  auto operator<=>(Tile const& rhs) const = default;
};

std::pair<i64,i64> Tile::to_pair() const { return std::pair<i64,i64>(x,y); }

// }}}

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
//
// @ Returns all nodes reachable from node t and filtered layers
//
template<SignedIntegral T, typename L>
auto filter_not_reachable(T t, Ops const& ops, L layers)
{
  // Ignore preceding nodes
  auto f_nop = [](auto){ return std::vector<T>{}; };

  // Collect all nodes reachable from t
  auto successors{search::bfs::run(t, f_nop, ops.succs)};

  // Remove non-reachable from t of each layer
  for (size_t i{}; i < layers.size(); ++i)
  {
    layers[i] = fw::apply(layers[i], fw::keep_if([&](auto e){ return fp::is_elem_of(e,successors); }));
  } // for

  return std::make_pair(layers,successors);
} // }}}

// function: area {{{
//
// Returns the total grid area as a horizontal x vertical pair
//
template<Map M>
decltype(auto) area(M const& m)
{
  return std::make_pair(
    rg::max_element(m, {}, [](auto e){ return e.second.x; })->second.x
    , rg::max_element(m, {},[](auto e){ return e.second.y; })->second.y
  );
} // }}}

// fn: subgraph {{{
//
// Positions nodes of a subgraph based on root
// @root: root base of subgraph
// @slots: number of root nodes in the graph
// @ops: Graph operations
// @layers: Layered graph view
// @idx_base: Initial layer index to start placement, must be the same for all
// subgraphs
//
template<SignedIntegral T, typename L>
decltype(auto) subgraph(T root, Ops const& ops, L layers, u64 idx_base, size_t slots)
{
  auto [filtered_layers,root_reachable] {filter_not_reachable(root,ops,layers)};

  // Save vertices positions
  MapVertexTile m_vertex_tile;

  // Find level with highest number of vertices
  auto const it_base {filtered_layers.begin()+idx_base};

  // Offset y-position, to separate layers by the distance of wires between them
  auto f_offset_y = [&](u64 y)
  {
    // Let layer 0 on y = 0
    if( y == 0 ){ return u64{}; }

    // From layer 0 to y_f
    // Get nodes
    // Transform them into successors count
    // Reduce (sum)
    return fw::apply(fp::numbers(u64{},y)
      , fw::transform([&](auto i){ return layers.at(i); })
      , fw::transform([&](auto l)
        {
          return fw::apply(l
            , fw::transform([&](auto u){ return ops.succs(u).size(); })
            , fw::reduce([&](auto acc, auto u){ return acc+u; },2)
          );
        })
      , fw::reduce([&](auto acc, auto u){ return acc+u; },0)
    );
  };

  // Place vertex u in coordinate {x,y}
  auto f_place = [&](auto x, auto y, auto u) -> void
  {
    if( u > 0 )
    {
      m_vertex_tile.emplace(u,Tile(x,y,TileType::ND));
      return;
    }

    if( ops.succs(u).size() > 1 )
    {
      m_vertex_tile.emplace(u,Tile(x,y,TileType::URD));
    }
    else
    {
      m_vertex_tile.emplace(u,Tile(x,y,TileType::UD));
    } // else
  };

  // Place level with higher number of vertices
  rg::for_each(*it_base, [&,x=0,y=idx_base](auto u) mutable { f_place(x*slots,f_offset_y(y),u); ++x; });

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
    auto new_y{f_offset_y(y)};

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

// fn: run {{{
template<SignedIntegral T, typename L>
auto run(T root, Ops const& ops, L const& layers)
{
#if ! defined(NDEBUG) && defined(DEBUG_SHOW_ALG)
  spdlog::set_level(spdlog::level::debug);
  spdlog::debug("Algorithm: celaeno::graph::representations::grid");
#endif

  //
  // Find root nodes
  //
  auto root_nodes{fp::keep_if([&](auto e){ return ops.preds(e).empty(); },ns_search::bfs::run(root,ops))};

  // Find starting point for placement, which is the largest layer
  auto const it_base {rg::max_element(layers, {}, [](auto e){ return e.size(); })};

  // Keep the index of it_base for subgraphs
  auto i_base{std::distance(layers.begin(),it_base)};

  //
  // Determine the number of slots on each node
  //
  auto slots{root_nodes.size()};

  //
  // Process each subgraph sequentially or concurrently
  //
  std::vector<MapVertexTile> solutions;

#ifdef OPT_SEQ

  for (auto root_node : root_nodes)
  {
    solutions.emplace_back(subgraph(root_node, ops, layers, i_base, slots));
  } // for

#else

  // Keep a mutex to control insertions in solutions vector
  std::mutex mutex_solutions;

  // Solution insertion lambda
  auto add_to_solutions = [&]<typename S>(S&& solution)
  {
    std::lock_guard<std::mutex> guard(mutex_solutions);
    solutions.emplace_back(std::forward<S>(solution));
  };

  std::vector<std::thread> jobs;

  for (auto root_node : root_nodes)
  {
    jobs.emplace_back([=] { add_to_solutions(subgraph(root_node, ops, layers, i_base, slots)); } );
  } // for

  for (auto& job : jobs) { job.join(); } // for

#endif // OPT_SEQ


  //
  // Merge solutions
  //

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

      // Update x-pos
      e.second.x += offset;
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
    } // for
  } // for

  //
  // Include outgoing edge offset
  //
  // TODO FIX overlaps
  for (auto layer : layers)
  {
    layer = fp::sort_by([&](auto u, auto v){ return solution.at(u).x < solution.at(v).x; },layer);

    for (i64 x_offset{}; auto u : layer) { solution.at(u).x += x_offset; x_offset += 2; } // for
  } // for

  //
  // Set nodes to always be in the same/previous rows of successors
  //
  i64 x_layer{};
  for (auto it{layers.begin()}; it != layers.end(); ++it)
  {
    x_layer = 0;

    auto layer {fp::sort_by([&](auto u, auto v){ return solution.at(u).x < solution.at(v).x; },*it)};

    // Get max x-dist of a node to its successor
    for (auto u : layer)
    {
      auto f_x_dist = [&]( auto v ){ return std::abs(solution.at(u).x - solution.at(v).x); };

      auto succs{ops.succs(u)};

      if( succs.empty() ){ continue; }

      auto x_dists = fp::transform(f_x_dist,succs);

      auto max_x_dist = fp::maximum(x_dists);

      x_layer = (max_x_dist > x_layer)? max_x_dist : x_layer;
    } //

    x_layer += 2;

    if( std::next(it) != layers.end() )
    {
      for (auto u : *std::next(it))
      {
        solution.at(u).x += x_layer;
      } // for
    } // if
  } // for

  return solution;
} // }}}

// function: route {{{
template<typename L>
decltype(auto) route(Ops const& ops, MapVertexTile const& m_vertex_tile, L layers)
{
  std::map< std::pair<Tile,Tile>, std::deque<std::pair<i64,i64>> > paths;

  for (auto it{layers.begin()}; it != layers.end(); ++it)
  {
    if( std::next(it) == layers.end() ){ continue; }

    // Sort next layer by x-pos
    auto layer {fp::sort_by([&](auto u, auto v){ return m_vertex_tile.at(u).x < m_vertex_tile.at(v).x; },*std::next(it))};

    // Start from last to first node
    layer = ra::reverse(layer);

    // Keep track of visited preds
    std::set<i64> visited;

    // For each node u on layer
    for (i64 u_idx{1}; auto u : layer)
    {
      // For each node v, pred of u
      auto preds{ops.preds(u)};

      // Sort preds by x proximity of u
      preds = fp::sort_by([&](auto u, auto v){ return m_vertex_tile.at(u).x < m_vertex_tile.at(v).x; },preds);

      for (i64 v_idx{}; auto v : preds)
      {
        // Create begin/end pair
        auto tiles {std::make_pair(m_vertex_tile.at(v),m_vertex_tile.at(u))};

        // Use std::pair
        auto [t1,t2] = std::make_pair(tiles.first.to_pair(),tiles.second.to_pair());

        // Create path
        std::deque<std::pair<i64,i64>> path;

        // Set coordinates
        auto [y_i,y_m,y_f,x_i,x_f] = std::make_tuple(t1.second,t1.second+u_idx,t2.second,t1.first,t2.first);

        // Adjust starting x-pos if predecessor was already visited
        if (visited.contains(v)) { path.emplace_back(x_i++,y_i); } // if
        else { visited.insert(v); } // else

        // Adjust y-col for next predecessor
        if(v_idx != 0) { --x_f; } // if

        // Vert. in source col
        for (auto y : fp::numbers(y_i,y_m+1)) { path.emplace_back(x_i,y); } // for

        // Hor. to dest. col
        for (auto x : fp::numbers(x_i,x_f+1)) { path.emplace_back(x,y_m); } // for

        // Vert. in dest. col
        for (auto y : fp::numbers(y_m,y_f+1)) { path.emplace_back(x_f,y); } // for

        // Include last tile for solution in successor != first
        if(v_idx != 0) { path.emplace_back(x_f+1,y_f); } // if

        // Remove duplicate consecutive tiles and add path to solution
        path = fp::unique(path);
        paths.emplace(tiles, path);

        // Increment indices
        ++u_idx; ++v_idx;
      } // for

    } // for

  } // for

  return paths;


} // }}}

} // namespace celaeno::graph::representations::grid }}}
