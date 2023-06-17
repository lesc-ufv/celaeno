// vim: set expandtab fdm=marker ts=2 sw=2 tw=100 et :
//
// @author      : Ruan E. Formigoni (ruanformigoni@gmail.com)
// @file        : zig-zag
// @created     : Tuesday Jul 27, 2021 21:57:31 UTC
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


#include <algorithm>
#include <ranges>
#include <tuple>
#include <optional>
#include <variant>

#include <fmt/core.h>

#include <fplus/fplus.hpp>
#include <range/v3/all.hpp>
#include <spdlog/spdlog.h>
#include <cppcoro/generator.hpp>

#include <celaeno/aliases.hpp>
#include <celaeno/concepts.hpp>
#include <celaeno/err/err.hpp>
#include <celaeno/fun/fun.hpp>
#include <celaeno/fun/multimap.hpp>

#include <celaeno/graph/graph.hpp>
#include <celaeno/graph/io/verilog.hpp>
#include <celaeno/graph/io/dimacs.hpp>

#include <celaeno/graph/views/depth.hpp>
#include <celaeno/graph/draw/svg.hpp>

#include <celaeno/graph/search/a-star.hpp>
#include <celaeno/graph/search/bfs.hpp>
#include <celaeno/graph/search/kahn.hpp>
#include <celaeno/graph/search/zig-zag.hpp>
#include <celaeno/graph/search/aps.hpp>

#include <celaeno/heuristics/chebyshev.hpp>
#include <celaeno/heuristics/manhattan.hpp>

#include <celaeno/graph/operations/balance/crossings.hpp>
#include <celaeno/graph/operations/minimize/crossings.hpp>

#include "unbalance.hpp"
#include "bfs.hpp"
#include "cycles.hpp"

// TODO Remove
#include <celaeno/graph/operations/balance/outgoing.hpp>
#include <celaeno/graph/operations/balance/incoming.hpp>
#include <celaeno/graph/operations/count/crossings.hpp>

#include <pybind11/embed.h>
#include <pybind11/stl.h>

// Using namespace {{{
using namespace celaeno::fun::fn;
using namespace celaeno::concepts;
using namespace celaeno::aliases;
// }}}

// namespaces {{{
namespace py = pybind11;
namespace err = celaeno::err;
namespace fp = fplus;
namespace rg = ranges;
namespace rv = ranges::views;
namespace fun = celaeno::fun;

namespace ns_graph = celaeno::graph;
namespace ns_draw = celaeno::graph::draw;
namespace ns_ops = celaeno::graph::operations;
namespace ns_heuristics = celaeno::heuristics;
namespace ns_io_verilog = celaeno::graph::io::verilog;
namespace ns_io_dimacs = celaeno::graph::io::dimacs;
namespace ns_search = celaeno::graph::search;
namespace ns_views = celaeno::graph::views;
// }}}

// Aliases {{{
using Sink = std::shared_ptr<spdlog::logger>;
using Ops = ns_graph::Ops;
using Node = i64;
using Nodes = std::vector<Node>;
using NodeMMap = std::multimap<Node,Node>;
using Edge = std::pair<Node,Node>;
using Edges = std::vector<Edge>;
using Weight = i64;
using Distance = i64;
using Distances = std::set<i64>;
using PairsInterCycles = std::vector<std::pair<Nodes,Nodes>>;
using Annotations = std::map<Node,Distances>;
using MNodeMAnnotations = std::map<Node,Annotations>;
using MEdgeWeight = std::map<Edge,Weight>;
using Tile = std::pair<i64,i64>;
using Tiles = std::vector<Tile>;
using Paths = std::map<std::pair<Node,Node>,std::deque<Tile>>;
using Placement = std::map<Node,Tile>;
using Base = std::vector<Node>;
using Basis = std::vector<Base>;
using Location = celaeno::err::Location;
// }}}

// fn: f_timer {{{
template<typename F, typename... Args>
auto f_timer(Location const& loc, F&& f, Args&&... args)
{
  if constexpr(std::is_void_v<std::invoke_result_t<F, Args...>>)
  {
#ifdef DEBUG
    fmt::print("[exe] {}\n", loc.get());
    auto start {std::chrono::system_clock::now()};
    std::invoke(std::forward<F>(f), std::forward<Args>(args)...);
    auto end {std::chrono::system_clock::now()};
    std::chrono::duration<f64> dur {end-start};
    fmt::print("[end] {} took {} seconds\n", loc.get(), dur.count());
    return void();
#else
    std::invoke(std::forward<F>(f), std::forward<Args>(args)...);
    return void();
#endif
  }
  else
  {
#ifdef DEBUG
    fmt::print("[exe] {}\n", loc.get());
    auto start {std::chrono::system_clock::now()};
    auto result = std::invoke(std::forward<F>(f), std::forward<Args>(args)...);
    auto end {std::chrono::system_clock::now()};
    std::chrono::duration<f64> dur {end-start};
    fmt::print("[end] {} took {} seconds\n", loc.get(), dur.count());
    return result;
#else
    return std::invoke(std::forward<F>(f), std::forward<Args>(args)...);
#endif
  }
}; // fn: f_timer }}}

// fn: rev {{{
template<typename T1, typename T2>
decltype(auto) rev(std::pair<T1,T2> const& pair)
{
  return std::make_pair(pair.second,pair.first);
} // function: rev }}}

// fn: view_draw {{{
decltype(auto) view_draw(Map auto&& map_vertex_layer)
{
  // , Map auto&& map_vertex_tile, Range auto&& range_edge_path)
  // Get max layer id
  constexpr u64 const size_spacing = 4;

  u64 max_layer_id = fn(map_vertex_layer).as([](auto&& e){ return e.second; }).max();

  // Create layers
  ns_views::depth::LayerNodes<i64> map_layer_vertices;
  std::ranges::for_each(map_vertex_layer, [&](auto&& e)
  {
    u64 layer_curr = max_layer_id - e.second;
    if ( ! map_layer_vertices.contains(layer_curr) )
    {
      map_layer_vertices.emplace(layer_curr, Nodes{e.first});
    }
    else
    {
      map_layer_vertices.at(layer_curr).push_back(e.first);
    }
  });

  for (auto&& [l,v] : map_layer_vertices)
  {
    fmt::print("l: {} - v: {}\n", l, v);
  } // for

  // Find out the size of the largest layer
  u64 const size_max_layer = fn(map_layer_vertices).as([](auto&& e){ return e.second.size(); }).max();
  fmt::print("Max layer size: {}\n", size_max_layer);

  // Shift layers left by (size_max_layer-size_cur_layer)/2
  std::map<u64, u64> map_layer_shift = fn(map_layer_vertices)
    .as([&](auto&& e){ return std::make_pair(e.first, (size_max_layer - e.second.size()) / 2); })
    .as([&](auto&& e){ if (e.second == 0) { e.second = 1; } return e; })
    .map();

  // Convert layers into collections of xy positions
  std::map<Node, Tile> map_vertex_tile = fn(map_layer_vertices)
  .as([&](auto&& _1)
  {
    i64 y_pos = _1.first;
    i64 x_pos = 0;
    return fn(_1.second)
      .as([&](auto __1)
      {
        return std::make_pair(__1, Tile((++x_pos)*(map_layer_shift.at(y_pos)), y_pos));
      })
      .vec();
  })
  .squash()
  .map();

  std::vector<std::pair<Edge,Tiles>> path;

  ns_draw::svg::svg("out/celaeno.svg", map_vertex_tile, path, [](auto e){ return e; });

} // function: view_draw }}}

// fn: lowest_node_id {{{
//
// This function uses a BFS to determine which node has the lowest id in the graph
//
i64 lowest_node_id(Ops const& ops)
{
  // Dummy vertex with lowest value
  i64 idx{};

  // Define function to compare values lt 0
  auto f_lowest = [&idx](auto&& e) { if(e < idx){ idx=e; } return false; };

  // Get the dummy vertex with the lowest value
  ns_search::bfs::run(0,ops.preds,ops.succs,f_lowest);

  return --idx;
} // function: lowest_node_id }}}

// fn: path_from_multimap {{{
//
// This function searches for a path between u and v in a multimap
//
template<typename F = std::function<void(Edges&)>>
std::optional<Nodes> path_from_multimap(std::multimap<Node,Node> const& mm
    , Node u
    , Node v
    , F&& remove_by = [](Edges&){ })
{
  // Return path in vec
  Nodes out;

  // Map to backtrack paths
  std::map<Node,Node> m_backtrack;

  // Fetch next edges
  auto f_fetch_edge = [&](Node w)
  {
    // Adjacent to w
    auto r = mm.equal_range(w);
    // Edges (w,x1),(w,x2),...,(w,xn)
    Edges edges(r.first, r.second);
    // Do not explore edges based on condition
    remove_by(edges);
    // Return modification of remove_by
    return edges;
  };

  // Include edges in backtrack map
  auto f_backtrack_add = [&](auto&& e){ m_backtrack[e.second] = e.first; };

  // Get initial edges
  Edges edges_initial = f_fetch_edge(u);

  // Add initial edges to backtrack map
  std::ranges::for_each(edges_initial, f_backtrack_add);

  // Check if solution was found
  bool has_solution{false};

  // Make an edge-based bfs search until reaches v or ends
  ns_search::bfs::run(edges_initial
    , [](auto){ return std::vector<Edge>{}; }
    , [&](Edge e) { return f_fetch_edge(e.second); }
    , [&](Edge e)
    {
      // Update path
      f_backtrack_add(e);
      // If reached destination edge v, stop
      if ( e.second == v ) { has_solution = true; return true; }
      // Continue processing
      return false;
    });

  if ( ! has_solution ) { return std::nullopt; }

  // Rebuild path in backtrack map
  Node curr{v};
  out.push_back(curr);
  while ( m_backtrack.contains(curr) )
  {
    out.push_back(m_backtrack.at(curr));
    curr = m_backtrack.at(curr);
  } // while
  
  // Reverse to be from u to v
  std::ranges::reverse(out);

  return out;
} // function: path_from_multimap }}}

// fn: cycle_find_endpoints {{{
//
// A cycle has two endpoints, one diverges into two edge, and the other converges two edges. This
// function finds these endpoints
//
decltype(auto) cycle_find_endpoints(Ops const& ops, Range auto&& cycle)
{
  // Predecessors within the cycle
  auto f_preds_in_cycle = [&](auto&& e){ return fn(ops.preds(e)).in(cycle).vec(); };

  // Successors within the cycle
  auto f_succs_in_cycle = [&](auto&& e){ return fn(ops.succs(e)).in(cycle).vec(); };

  // Find node with 2 preds in cycle (endpoint 1)
  auto it_endpoint1 = std::ranges::find_if(cycle, [&](auto e){ return f_preds_in_cycle(e).size() == 2; });
  err::err({it_endpoint1 != std::ranges::end(cycle)})("Could not find cycle endpoint");

  // Find node with 2 succs in cycle (endpoint 2)
  auto it_endpoint2 = std::ranges::find_if(cycle, [&](auto e){ return f_succs_in_cycle(e).size() == 2; });
  err::err({it_endpoint2 != std::ranges::end(cycle)})("Could not find cycle endpoint");

  return std::make_pair(*it_endpoint2, *it_endpoint1);
} // function: cycle_find_endpoints }}}

// fn: cycle_split_in_endpoints {{{
//
// A cycle has two endpoints, one diverges into two edge, and the other converges two edges. This
// function splits the cycle into two half cycles, the parents are included in both half cycles as
// the endpoints
//
decltype(auto) cycle_split_in_endpoints(Ops const& ops, Range auto&& cycle)
{
  std::pair<Nodes,Nodes> out;

  // Predecessors within the cycle
  auto f_preds_in_cycle = [&](auto&& e){ return fn(ops.preds(e)).in(cycle).vec(); };

  // Successors within the cycle
  auto f_succs_in_cycle = [&](auto&& e){ return fn(ops.succs(e)).in(cycle).vec(); };

  // Vector that contains both halves
  Nodes whole;

  // Find node with 2 preds in cycle (endpoint 1)
  auto it_endpoint1 = std::ranges::find_if(cycle, [&](auto e){ return f_preds_in_cycle(e).size() == 2; });
  err::err({it_endpoint1 != std::ranges::end(cycle)})("Could not find cycle endpoint");

  // Push initial node
  whole.push_back(*it_endpoint1);

  // Repeat initial value
  whole.push_back(*it_endpoint1);

  // Use a bfs to construct the rest of the cycle
  ns_search::bfs::run(*it_endpoint1, f_preds_in_cycle, [](auto&&) { return Nodes{}; },
  [&](auto e)
  {
    if ( auto succs = f_succs_in_cycle(e); ! succs.empty() )
    {
      if ( fn(succs).has(whole.front()) ) { whole.insert(whole.begin(), e); }
      else { whole.push_back(e); }
    }
    return false;
  });

  // Find node with 2 succs in cycle (endpoint 2)
  auto it_endpoint2 = std::ranges::find_if(cycle, [&](auto e){ return f_succs_in_cycle(e).size() == 2; });
  err::err({it_endpoint2 != std::ranges::end(cycle)})("Could not find cycle endpoint");

  // Repeat the endpoint in both ends of the solution
  if ( whole.front() == *it_endpoint2 ) { whole.push_back(*it_endpoint2); }
  else { whole.insert(whole.begin(), *it_endpoint2); }

  // Find the current position of endpoint_1 in whole
  auto it_endpoint1_whole = std::ranges::find_if(whole, [&](auto e){ return e == *it_endpoint1; });
  err::err({it_endpoint1_whole != std::ranges::end(whole)})("Could not find cycle endpoint");

  // Split the cycle in two
  Nodes out1(whole.begin(), std::next(it_endpoint1_whole));
  Nodes out2(std::next(it_endpoint1_whole), whole.end());

  return std::make_pair(out1,out2);
} // function: cycle_split_in_endpoints }}}

// cycle_find_nodes_in_between {{{
//
// Given two nodes uv and a cycle, find the nodes (w1,...,wn) in-between uv
//
decltype(auto) cycle_find_nodes_in_between(Ops const& ops, Node u, Node v, Range auto&& cycle, Sink sink)
{
  // err::Logger logger{sink};
  //
  // Nodes cycle_with_dummy = fn(cycle)
  //   .mut([](auto e){ e.push_back(e.front()); return e; })
  //   .slide(2)
  //   .as([&](auto e)
  //   {
  //     Nodes out;
  //     i64 u = e.at(0);
  //     i64 v = e.at(1);
  //
  //     // u must be sucessor of v
  //     bool is_reversed{false};
  //     if ( ! fn(ops.succs(u)).has(v) ) { is_reversed = true; v = std::exchange(u,v); }
  //     logger.info()("u,v: {},{}\n", u, v);
  //     
  //     // If endpoint is not v and is not introduced by A*, remove
  //     auto remove_by = [&](Edges& e)
  //     {
  //       e = fn(e).keep([&](Edge const& e)
  //       {
  //         // Check if is goal 'v'
  //         if ( e.second == v )       { return true; }
  //         // Check if is introduced by A*
  //         if ( ! ops.has(e.second) ) { return true; }
  //         // Discard
  //         return false;
  //       }).vec();
  //     };
  //
  //     if ( auto opt_nodes = path_from_multimap(mmap_node, u, v, remove_by) )
  //     {
  //       logger.info()("Nodes between uv: {}\n", *opt_nodes);
  //       if ( is_reversed ) { std::ranges::reverse(*opt_nodes); }
  //       return *opt_nodes;
  //     }
  //
  //     return (is_reversed)? Nodes{v,u} : Nodes{u,v};
  //   })
  //   .squash()
  //   .unique()
  //   .vec();
} // function: cycle_find_nodes_in_between }}}

// fn: merge_cycles_in_outer_cycle {{{
//
// Given two cycles and an intersection, merge the cycles in a new outer cycles without the
// intersection
//
decltype(auto) merge_cycles(Placement const& p, Range auto&& c1, Range auto&& c2, Range auto&& cint)
{
  // Fetch nodes in-between cint.front() and cint.back()

} // function: merge_cycles }}}

// fn: make_line_equation {{{
//
// This function creates a lambda that represents the line equation for two points
//
struct MakeLineEquationRet
{
  std::function<i64(i64)> fun;
  i64 m;
  i64 b;
};

decltype(auto) make_line_equation(Tile const& p1, Tile const& p2, Sink sink)
{
  err::Logger logger{sink};

  err::err({p1.first != p2.first})("Attempt to calculate equation of straight line");

  i64 m = (p2.second - p1.second) / (p2.first - p1.first);
  i64 b = p1.second - m * p1.first;

  logger.info()("-- line m: {}\n", m);
  logger.info()("-- line b: {}\n", b);

  return MakeLineEquationRet{.fun=[=](i64 x){ return m*x + b; }, .m=m, .b=b};
} // function: make_line_equation }}}

// fn: cycle_has_half_separation {{{
//
// This function checks if a positioned cycle halves are separate, this separation consists of a
// straight line from one endpoint to another.
//
decltype(auto) cycle_has_half_separation(Ops const& ops, Nodes const& cycle, Placement const& m_node_pos, Sink sink)
{
  err::Logger logger{sink};

  for (auto e : m_node_pos)
  {
    logger.info()("placement: {}\n", e);
  } // for

  // Get endpoints
  auto [epn1,epn2] = cycle_find_endpoints(ops, cycle);
  logger.info()("Cycle endpoints: {} | {}\n", epn1, epn2);

  // Split the cycle in two halves
  auto [chf1,chf2] = cycle_split_in_endpoints(ops, cycle);

  logger.info()("Cycle half 1: {}\n", chf1);
  logger.info()("Cycle half 2: {}\n", chf2);

  // Get line equation for endpoints
  // Current plane considers:
  // // up   == -y, down  == +y
  // // left == -x, right == +x
  // y sign needs to be reversed for the line equation to work
  Tile ep1 = m_node_pos.at(epn1);
  Tile ep2 = m_node_pos.at(epn2);
  ep1.first *= 2; ep1.second *= 2; ep1.second = -ep1.second;
  ep2.first *= 2; ep2.second *= 2; ep2.second = -ep2.second;


  std::function<i64(i64)> f_equation;
  std::function<bool(Tile)> f_is_below;

  // Check if is not a vertical line
  if( ep1.first != ep2.first )
  {
    f_equation = make_line_equation(ep1, ep2, sink).fun;
    f_is_below = [=](Tile t){ return t.second < f_equation(t.first); };
  }
  else
  {
    auto [x_min_it,x_max_it] = fn(cycle).minmax({}, [&](Node e){ return m_node_pos.at(e); });
    auto [x_min, x_max] = std::make_pair(m_node_pos.at(*x_min_it).first*2, m_node_pos.at(*x_max_it).first*2);
    i64 x_mea = (x_max+x_min)/2;

    logger.info()("x_mea: {}\n", x_mea);

    f_is_below = [=](Tile t)
    {
      // i64 x_base = ep1.first;
      // Consider 'below' being to the left
      return t.first < x_mea;
    };
  } // else

  std::function<bool(Tile)> f_is_above = [&](Tile const& t){ return ! f_is_below(t); };


  logger.info()("Cycle endpoints: {} | {}\n", ep1, ep2);

  // Pop endpoints of halves
  chf1.pop_back();
  chf1.erase(chf1.begin());
  chf2.pop_back();
  chf2.erase(chf2.begin());

  auto f_is_crossed = [&](auto const& cycle_half)
  {
    if ( cycle_half.size() < 2 ) { return false; }
    // Get first node
    auto n = cycle_half.front();
    // Get first point
    auto t_fst = m_node_pos.at(n);
    t_fst.first *= 2; t_fst.second *= 2; t_fst.second = -t_fst.second;
    // Check if point is above or below the line
    // > 0 above the line
    // < 0 below the line
    // = 0 on the line
    bool is_below{f_is_below(t_fst)};
    logger.info()("Node {} with Point {} is below? {}\n", n, t_fst, is_below);
    for( auto it{std::next(cycle_half.begin())}; it != cycle_half.end(); ++it )
    {
      err::err({ m_node_pos.contains(*it) })("Cycle node {} not found in placement", *it);

      // Other points must also be the same as the first
      Tile t = m_node_pos.at(*it);
      t.first *= 2; t.second *= 2; t.second = -t.second;

      logger.info()("Node {} with point {} is below? {}\n", *it, t, f_is_below(t));

      if ( is_below )
      {
        if ( f_is_above(t) ) { return true; }
      }
      else
      {
        if ( f_is_below(t) ) { return true; }
      }
    } // for

    return false;
  };

  return f_is_crossed(chf1) or f_is_crossed(chf2);
} // function: cycle_has_half_separation }}}

// fn: cycle_has_inner_crossings {{{
//
// This function checks if a positioned cycle halves cross between themselves
//
decltype(auto) cycle_has_inner_crossings(Nodes cycle
    , Placement const& m_node_pos
    , Sink sink)
{
  err::Logger logger{sink};

  // // Insert intermediate paths between nodes
  // cycle = fn(cycle)
  //   .mut([](auto e) { e.push_back(e.front()); return e; })
  //   .slide(2)
  //   .as([&](auto e)
  //   {
  //     i64 u = e.at(0);
  //     i64 v = e.at(1);
  //     Tile t1{u,v};
  //     Tile t2{v,u};
  //     Nodes out;
  //     if ( paths.contains(t1) )
  //     {
  //       out.insert(out.begin(), paths.at(t1).begin(), paths.at(t1).end());
  //     }
  //     else if ( paths.contains(t2) )
  //     {
  //       out.insert(out.begin(), paths.at(t2).begin(), paths.at(t2).end());
  //     }
  //     else
  //     {
  //       out.push_back(u);
  //       out.push_back(v);
  //     } // else
  //     return out;
  //   })
  //   .squash()
  //   .vec();

  for (auto e : m_node_pos)
  {
    logger.info()("placement: {}", e);
  } // for

  // // Given three collinear points p, q, r, the function checks if 
  // // point q lies on line segment 'pr' 
  // auto f_on_segment = [](Tile p, Tile q, Tile r)
  // {
  //   if ( q == p or q == r )
  //   {
  //     return false;
  //   }
  //
  //   if ( (q.first <= std::max(p.first, r.first))
  //       and (q.first >= std::min(p.first, r.first))
  //       and (q.second <= std::max(p.second, r.second))
  //       and (q.second >= std::min(p.second, r.second)))
  //   {
  //     return true;
  //   }
  //
  //   return false;
  // };

  auto f_orientation = [](Tile p, Tile q, Tile r)
  {
    int val = (q.second-p.second) * (r.first-q.first)
     - (q.first-p.first) * (r.second-q.second); 

    return (val > 0)? 1 // Clockwise orientation
        :  (val < 0)? 2 // Counterclockwise orientation
        :  0;           // Collinear orientation
  };

  auto f_get_intersection_point = [](Tile p1, Tile q1, Tile p2, Tile q2) -> std::optional<Tile>
  {
    i64 A1 = q1.second - p1.second;
    i64 B1 = p1.first - q1.first;
    i64 C1 = A1 * p1.first + B1 * p1.second;

    i64 A2 = q2.second - p2.second;
    i64 B2 = p2.first - q2.first;
    i64 C2 = A2 * p2.first + B2 * p2.second;

    i64 det = A1 * B2 - A2 * B1;

    if (det == 0)  // lines are parallel
    {
      return std::nullopt;
    }

    i64 x = (B2 * C1 - B1 * C2) / det;
    i64 y = (A1 * C2 - A2 * C1) / det;
    return std::make_optional(Tile{x,y});
  };

  auto f_intersect = [&](Tile p1, Tile q1, Tile p2, Tile q2)
  {
    // Four orientations for the general and special cases
    int o1 = f_orientation(p1, q1, p2);
    int o2 = f_orientation(p1, q1, q2);
    int o3 = f_orientation(p2, q2, p1);
    int o4 = f_orientation(p2, q2, q1);

    // Check if intersect
    if ((o1 != o2) and (o3 != o4))
    {
      if ( auto opt_tile_intersection = f_get_intersection_point(p1,q1,p2,q2); opt_tile_intersection )
      {
        Tile ti = *opt_tile_intersection;
        if ( ti != p1 and ti != q1 and ti != p2 and ti != q2 )
        {
          return true;
        }
      }
    }

    // Skip colinear
    // // p1 , q1 and p2 are collinear and p2 lies on segment p1q1
    // if ((o1 == 0) and f_on_segment(p1, p2, q1))
    // {
    //   return true;
    //
    // }
    // // p1 , q1 and q2 are collinear and q2 lies on segment p1q1
    // if ((o2 == 0) and f_on_segment(p1, q2, q1))
    // {
    //   return true;
    // }
    //
    // // p2 , q2 and p1 are collinear and p1 lies on segment p2q2
    // if ((o3 == 0) and f_on_segment(p2, p1, q2))
    // {
    //   return true;
    // }
    //
    // // p2 , q2 and q1 are collinear and q1 lies on segment p2q2
    // if ((o4 == 0) and f_on_segment(p2, q1, q2))
    // {
    //   return true;
    // }
    
    return false;
  };

  auto cycle_edges = fn(cycle)
    .mut([](auto e) { e.push_back(e.front()); return e; })
    .as([&](Node e){ return m_node_pos.at(e); })
    .as([](Tile e){ return Tile{e.first*2,e.second*2}; })
    .pairs()
    .vec();
  logger.info()("Cycle: {}", cycle);
  logger.info()("Cycle edges: {}", cycle_edges);

  for (auto const& e1 : cycle_edges)
  {
    for (auto const& e2 : cycle_edges)
    {
      logger.info()("Compare {} with {}", e1, e2);
      if ( f_intersect(e1.first, e1.second, e2.first, e2.second) )
      {
        logger.info()("Intersected edges: {} and {}", e1, e2);
        return true;
      }
    } // for
  } // for

  return false;
} // function: cycle_has_inner_crossings }}}

// fn: e_bfs {{{
template<typename F = std::function<bool(Edge)>>
[[nodiscard]] Annotations e_bfs(Ops const& ops
  , Node r
  , F&& f = [](Edge) -> bool { return false; }
)
  requires Returns<bool,F,Edge>
{
  // Empty node queue
  std::queue<Node> q;

  // Push initial element
  q.push(r);

  // Visited Nodes
  std::set<Node> vn;

  // Visited edges
  std::set<Edge> ve;

  // Annotations
  Annotations h;

  // Push initial distance
  h[r] = {0};

  while (! q.empty())
  {
    // Get next node from queue front
    Node u{q.front()}; q.pop();

    // If node has been visited, skip iteration
    if( vn.contains(u) ){ continue; }

    // Else visit node
    vn.insert(u);

    // Helper to retrieve neighboring vertices of a vertex 'v'
    auto neighbors = [&](Node v){ return fn(ops.preds(v)).chain(ops.succs(v)).vec(); };

    // Helper to check if a set of edges has been visited
    auto contains = []<Range R, typename... E>(R&& _1, E&&... _2)
      requires IsPairsOf<Node,E...> // Edges must be node pairs
    {
      return (_1.contains(_2) or ...);
    };

    // Retrieve neighbors of 'u', remove ones that form visited edges
    auto targets{fn(neighbors(u))
      .keep([&](Node v){ return ! contains(ve, Edge{u,v}, Edge{v,u}); })
      .vec()
    };

    // Iterate throught target edge nodes
    bool stop{false};
    for (auto v : targets)
    {
      // Apply callback
      stop = f(Edge{u,v})? true : stop;
      // Inherit distances from u to v
      rg::for_each(h[u],[&,v=v](auto d){ h[v].insert(d+1); });
      // Visit edge
      ve.emplace(u,v);
      // Enqueue v if not visited
      if( ! vn.contains(v) ){ q.push(v); }
    } // for

    if( stop ){ break; }

  } // while: ! q.empty()

  return h;
} // function: e_bfs }}}

// fn: minimal_basis_bfs_ordering {{{
void minimal_basis_bfs_ordering(Ops const& ops, Basis& basis)
{
  // std::random_device rd;
  // std::mt19937 g(rd());
  // std::shuffle(basis.begin(), basis.end(), g);

  // Fetch inputs
  Nodes inputs;
  ns_search::bfs::run(0, ops, [&](auto e)
  {
    if ( ops.preds(e).size() == 0 ) { inputs.push_back(e); };
    return false;
  });

  // Make the first elements contain the inputs
  std::ranges::partition(basis, [&](auto const& e){ return fn(e).in(inputs).vec().size() != 0; });

  // Take first base, use it as a starting point
  Basis::iterator cut{basis.begin()};

  // Partition the vector into adjacencies of cur
  for(auto it{basis.begin()}; it != basis.end(); ++it)
  {
    cut = std::partition(cut
      , basis.end()
      , [&](Base const& b) { return fn(b).in(*it).vec().size() >= 2; });
  }
} // function: minimal_basis_bfs_ordering }}}

// fn: align_intersections {{{
template<Range R>
decltype(auto) align_intersections(R minimal_basis, Sink sink)
{
  // Init logger
  err::Logger logger{sink};

  err::err({ ! minimal_basis.empty() })("Minimal basis must not be empty");

  err::err({ minimal_basis.size() > 1 })("Minimal basis must contain at least two cycles");

  // // Rotate until first two basis have intersection of size 2
  // minimal_basis = fn(minimal_basis)
  //   .rot([](auto const& basis)
  //   {
  //     auto const& fst = basis.at(0);
  //     auto const& snd = basis.at(1);
  //     return fn(fst).in(snd).vec().size() == 2;
  //   })
  //   .vec();

  PairsInterCycles out;

  std::queue<Nodes> q, q_restore;

  auto c1{minimal_basis.front()};

  logger.info()("c1: {}", c1);

  i64 i{};

  for (auto it{minimal_basis.begin()}; it != std::prev(minimal_basis.end()); ++it)
  {
    auto c2{*std::next(it)};

    auto intersection{fn(c1).in(c2).vec()};

    auto f_align_intersection = [&]()
    {
      intersection = fn(c1).in(c2).vec();
      logger.info()("c1 (pre): {}", c1);
      logger.info()("c2 (pre): {}", c2);
      logger.info()("in (pre): {}", intersection);
      if ( intersection.size() > 1 )
      {
        c1 = fn(c1).rot([&](auto v)
        {
          return fn(v).cut(u64{}, intersection.size()).in(intersection).vec().size() == intersection.size();
        }).vec();
        logger.info()("Rotated c1");

        c2 = fn(c2).rot([&](auto v)
        {
          return fn(v).cut(u64{}, intersection.size()).in(intersection).vec().size() == intersection.size();
        }).vec();
        logger.info()("Rotated c2");

        // Check if needs reverse
        u64 size_intersection = intersection.size();
        if ( fn(c1).cut(u64{}, size_intersection).vec() != fn(c2).cut(u64{}, size_intersection).vec() )
        {
          std::ranges::reverse(c2);
          std::ranges::rotate(c2, c2.end() - 1);
        }
        logger.info()("Checked if requires reverse");

        intersection = fn(c1).in(c2).vec();
      }
      logger.info()("c1: {}", c1);
      logger.info()("c2: {}", c2);
      logger.info()("in: {}", intersection);
    };

    f_align_intersection();

    // Log data
    // Handle first element
    if( i == 0 )
    {
      err::err({ ! intersection.empty() })("Intersection between first two elements must not be empty");

      // err::err({ intersection.size() == 2 })("Intersection must have size of 2");

      // Remove last element of cycle, if it equas the first
      if( c1.front() == c1.back() ){ c1.pop_back(); }

      // Rotate elements until first two are the intersection
      f_align_intersection();

      logger.info()("Rotated c1: {}", c1);

      intersection = fn(intersection).cut(0,2).vec();
      out.emplace_back(intersection,c1);
    } // if

    logger.info()("c2: {}", c2);

    while( intersection.empty() or intersection.size() < 2 )
    {
      // Check if there are more cycles to intersect with
      if( q.empty() ){ err::err()("No intersection for incident cycle"); }

      // Update c1
      c1 = q.front(); q.pop();

      q_restore.push(c1);

      logger.info()("Trying new c1: {}", c1);

      // Try to form intersection
      f_align_intersection();

      logger.info()("new_c1 ∩ c2: {}", intersection);

    } // while

    while( ! q_restore.empty() )
    {
      q.push(q_restore.front()); q_restore.pop();
    } // while

    logger.info()("c1: {}", c1);
    logger.info()("c1 ∩ c2: {}", intersection);

    q.push(c2);

    // Remove last element of cycle, if it equals the first
    if( c2.front() == c2.back() ){ c2.pop_back(); }

    f_align_intersection();

    logger.info()("Final c2: {}", c2);

    // Only allow intersection of size 2
    intersection = fn(intersection).cut(0,2).vec();
    out.emplace_back(intersection,c2);

    ++i;
  } // for

  logger.info()("Finished align_intersections");

  return out;
} // fn: align_intersections }}}

// fn: get_cycle_supports {{{
decltype(auto) get_cycle_supports(Ops const& ops, Range auto&& basis, Map auto&& prox_view)
{
  Edges out;

  auto f_make_cuts = [&](Range auto&& _1, Range auto&& _2) -> Edges
  {
    Edges _1_out;

    // For each node, try to connect to another node of _2
    for (auto it_c1{_1.begin()}; it_c1 != _1.end(); ++it_c1)
    {
      auto it_c2 = std::ranges::find_if(_2, [&](auto&& __1){ return prox_view.at(__1) > prox_view.at(*it_c1); });
      if ( it_c2 == std::ranges::end(_2) ) { break; } // if
      _1_out.push_back(Edge{*it_c1, *it_c2});
    } // for

    return _1_out;
  };

  // Create additional edges for cycles with size > 4
  for(auto const& base : basis)
  {
    if (base.size() > 4)
    {
      // Split in half cycles
      auto [c1,c2] = cycle_split_in_endpoints(ops, base);

      fmt::print("c1 (pre): {}\n", c1);
      fmt::print("c2 (pre): {}\n", c2);

      std::ranges::reverse(c2);
      c1.pop_back();
      c2.pop_back();
      c1.erase(c1.begin());
      c2.erase(c2.begin());

      if ( c1.empty() or c2.empty() ) { continue; }

      fmt::print("Base: {}\n", base);
      fmt::print("c1: {}\n", c1);
      fmt::print("c2: {}\n", c2);

      // Insert additional edges
      auto f_degree_out = [&](Node n) { return ops.succs(n).size(); };
      auto f_degree_in = [&](Node n) { return ops.preds(n).size(); };

      // Check which yields most cuts (c1 → c2 or c2 → c1)
      Edges cuts_c1 = f_make_cuts(c1,c2);
      Edges cuts_c2 = f_make_cuts(c2,c1);

      fmt::print("c1|c2: {}\n", cuts_c1);
      fmt::print("c1|c2: {}\n", cuts_c2);

      if ( cuts_c1.empty() && cuts_c2.empty() ) { continue; }

      auto cuts = ( cuts_c1.size() >= cuts_c2.size() )? cuts_c1 : cuts_c2;

      fn(cuts).ply([&](auto c){ out.emplace_back(c.first, c.second); });
    } // if base.size() > 4
  } // for

  fmt::print("Supports:\n");
  rg::for_each(out, [](auto _1){ fmt::print("s: {}\n", _1); });

  return out;
} // function: get_cycle_supports }}}

// fn: edge_weights {{{
[[nodiscard]] std::map<Edge,Weight>
  edge_weights(Ops const& ops, Range auto&& cycle, auto&& view)
{
  // Map of graph edges and weights
  std::map<Edge,Weight> out;

  // Calculate weight for each node in intersection
  for (auto u : cycle )
  {
    // Add weight for placed neighbors of u and v
    for (auto v : fn(ops.preds(u)).chain(ops.succs(u)).vec())
    {
      auto weight {fp::abs_diff(view.at(u),view.at(v))};
      out.emplace(Edge{u,v},weight);
      out.emplace(Edge{v,u},weight);
    } // for
  } // for

  return out;
} // function: edge_weights }}}

// struct: Adjacencies {{{
struct Adjacencies
{
  Tile m_src;
  i64 m_dist_exact;
  Tiles m_adjacencies;

  template<typename T>
  Adjacencies(T&& src, i64 dist_exact);
  Tiles tiles();
};

template<typename T>
Adjacencies::Adjacencies(T&& src, i64 dist_exact)
  : m_src(src)
  , m_dist_exact(dist_exact)
  , m_adjacencies({})
{};

Tiles Adjacencies::tiles()
{
  auto f_tiles_adjacent = [](Tile t) -> Tiles
  {
    return Tiles{
        Tile{t.first-1,t.second}
      , Tile{t.first+1,t.second}
      , Tile{t.first,t.second-1}
      , Tile{t.first,t.second+1}
      , Tile{t.first-1,t.second+1}
      , Tile{t.first+1,t.second-1}
      , Tile{t.first-1,t.second-1}
      , Tile{t.first+1,t.second+1}
    };
  };

  // Do a bfs to find all tiles with distance 'dist_exact' from src
  // Stop when a tile with distance > dist_exact is found, as all the following are increasingly more
  // distant
  (void) ns_search::bfs::run(
      m_src
    , f_tiles_adjacent
    , [](auto&&){ return Tiles{}; }
    , [&](Tile t)
      {
        auto dist_cur{ns_heuristics::chebyshev::run(m_src,t)};

        if( dist_cur > m_dist_exact+1 )
        {
          return true; // stop
        } // if

        if( dist_cur == m_dist_exact )
        {
          m_adjacencies.push_back(t);
        } // else if

        return false; // continue
      }
  );

  return m_adjacencies;
}

// struct: Adjacencies }}}

// fn: place_intersection {{{
auto place_intersection(Ops const& ops
  , Range auto intersection
  , [[maybe_unused]] MEdgeWeight const& m_edge_weight)
{
  // Initialize new placement
  Placement placement;

  // Check if intersection is empty
  err::err({! intersection.empty()})("Intersection must not be empty");

  // Intersection must have size less or eq to 2
  err::err({intersection.size() >= 2})("Intersection size must at least 2");

  // Intersection must have size less or eq to 2
  // err::err({intersection.size() <= 2})("Intersection size must be two or less");
  
  // // Place single-node intersection
  // if( intersection.size() == 1 )
  // {
  //   auto u{intersection.at(0)};
  //   placement[u] = std::make_pair(0,0);
  //   return placement;
  // } // if

  // Update placement
  // for (i64 i{}; auto u : intersection)
  // {
  //   placement[u] = std::make_pair(i,--i);
  // } // for

  // For b1 is (-1,-1)
  auto [u,v] = std::tie(intersection.at(0), intersection.at(1));

  // Edge weight for the first edge must equal 1
  // err::err({m_edge_weight.at({u,v}) == 1})("Initial intersection weight must be exactly 1");

  if ( fn(ops.succs(u)).has(v) != 0 )
  {

    placement[v] = std::make_pair( -1,-1);
    placement[u] = std::make_pair( 0, 0);
  } // if
  else
  {
    placement[v] = std::make_pair( 0, 0);
    placement[u] = std::make_pair( -1,-1);
  } // else

  return placement;
} // function: place_intersection }}}

// fn: place_cycle {{{
struct PlaceCycleRet
{
  Placement m_placement;
  Paths m_paths;
  NodeMMap m_mmap_node;
  std::set<Node> m_unreachable;
  bool m_failed;
  PlaceCycleRet(Placement const& placement
    , Paths const& paths
    , std::multimap<Node,Node> const& mmap_node
    , std::set<Node> const& unreachable
    , bool failed
    )
    : m_placement(placement)
    , m_paths(paths)
    , m_mmap_node(mmap_node)
    , m_unreachable(unreachable)
    , m_failed(failed)
  {}
};

cppcoro::generator<PlaceCycleRet> place_cycle(Ops const& ops
  , Placement p
  , Paths paths
  , std::multimap<Node,Node> mmap_node
  , Range auto cycle
  , Range auto inter
  , MEdgeWeight m_edge_weight
  , Sink sink)
{
  // Init logger
  err::Logger logger{sink};

  // Edge operations {{{

  // // Insert dummy nodes in edge map if not exists
  auto f_edge_update_dummy = [&]<typename M>(M& m, Node u, Node v, Nodes const& dummies)
  {
    fn(dummies)
      .sort()
      .push_front(u)
      .push_back(v)
      .slide(2)
      .ply([&](auto&& e){ fun::multimap::emplace_if_not_exists(m,e.at(0), e.at(1)); });
  };

  // // Erase dummy nodes from edge map
  auto f_edge_erase_dummy = [&]<typename M>(M& m, Node u, Node v, Nodes const& dummies)
  {
    fn(dummies)
      .sort()
      .push_front(u)
      .push_back(v)
      .slide(2)
      .ply([&](auto&& e){ fun::multimap::erase(m,e.at(0),e.at(1)); });
  };

  // // Erase dummy node paths created by A* from u to all neighbors
  auto f_erase_dummy = [&](Node u, auto& wires_of)
  {
    // Remove wires between uv placed by A*
    for (auto&& [node_target,map_dummy] : wires_of[u])
    {
      logger.info()("Node u          : {}", u);
      logger.info()("Node target     : {}", node_target);
      logger.info()("Dummies to erase: {}", fn(map_dummy).key().vec());
      // Remove from placed map
      for (auto&& [node_dummy,tile] : map_dummy) { p.erase(node_dummy); } // for
      // Remove from mmap_node map
      bool is_path_u_to_v{fn(ops.succs(u)).has(node_target)};
      logger.info()("Path is {} to {}? {}", u, node_target, is_path_u_to_v);
      f_edge_erase_dummy(mmap_node
        , (is_path_u_to_v)? u : node_target
        , (is_path_u_to_v)? node_target : u
        , fn(map_dummy).key().vec());
    } // for
    wires_of[u].clear();
  };
  // }}}

  // Tile operations {{{

  // // Check if a tile is occupied
  auto f_is_free = [&](Tile const& t){ return ! fn(p).val().has(t); };

  // // Get tiles sorrounding 't' until a distance 'dist' is reached
  auto f_get_candidates = [&](Tile const& t, i64 dist) { return Adjacencies{t,dist}.tiles(); };

  // // Actual distance by chebyshev distance
  auto f_dist_chebyshev = [&](Tile a, Tile b) { return ns_heuristics::chebyshev::run(a,b); };

  // // For each neighbor v of u, get all available tile positions that are at a max dist of WEIGHT(u,v)
  auto f_tiles_by_positioned_edge_weight = [&](Node u, Nodes const& neighbors_positioned)
  {
    return fn(neighbors_positioned)
      .as([&](Node v){ return f_get_candidates(p[v], m_edge_weight.at({u,v})); }) // nodes → tiles
      .squash() // merge [[tiles],[tiles]...] → [tiles]
      .drop([&](Tile t){ return ! f_is_free(t); })
      .sort()
      .unique()
      .vec();
  };

  // // A good position for a node u, is a tile that is after its positioned inputs, or before its
  // // positioned outputs
  auto f_tiles_keep_by_y = [&](Node u, Nodes const& neighbors_positioned, Tiles const& tile_candidates)
  {
    // The graph is placed 'upwards' which is -y
    return fn(tile_candidates).keep([&](Tile const& t)
    {
      for(Node const& n : neighbors_positioned)
      {
        Tile const& tn{p[n]};
        // A position before the placed incoming edges is not good
        if( fn(ops.preds(u)).has(n) && tn.second < t.second ) { return false; }  // if
        // A position after the placed outgoing edges is not good
        else if( fn(ops.succs(u)).has(n) && tn.second > t.second ) { return false; } // else if
      }
      return true;
    }).vec();
  };

  // // Only keep tiles that respect the distance heuristic
  auto f_tiles_keep_by_chebyshev_distance = [&](Node u, Nodes const& neighbors_positioned, Tiles const& tile_candidates)
  {
    // Target distance by edge weight
    auto f_target = [&](Node _1, Node _2) { return m_edge_weight.at({_1,_2}); };

    // For each tile, check if it adheres the distance constraint to every placed neighbor of u
    return fn(tile_candidates)
      .in_all(neighbors_positioned , [&](Tile t, Node v) { return f_target(u,v) == f_dist_chebyshev(t,p.at(v)); })
      .vec();
  };

  // // Only keep tiles that are reacheable. A pair of tiles (t1,t2) are reacheable when it is
  // // possible to create a path from t1 to t2, and this path has the same size as the chebyshev
  // // distance from (t1,t2)
  auto f_tiles_keep_by_astar = [&](Nodes const& neighbors_positioned, Tiles const& tile_candidates)
  {
    return fn(tile_candidates).keep([&](Tile t)
    {
      for (auto v : neighbors_positioned)
      {
        // Calculate chebyshev distance
        auto d_chebyshev{f_dist_chebyshev(t,p.at(v))};
        // Calculate chebyshev-based A* path
        auto d_astar{ns_search::a_star::run(t
          , p.at(v)
          , [&](Tile dest) -> Tiles { return f_get_candidates(dest,1); }
          , [&](Tile dest) -> bool { return fn(p).val().has(dest); }
          , [](Tile t1, Tile t2){ return ns_heuristics::chebyshev::run(t1,t2); }
        )};
        // Check if path exists, and it is eq to chebyshev
        if( ! d_astar || static_cast<size_t>(d_chebyshev+1) != d_astar->size() ) { return false; } // if
      } // for
      return true;
    })
    .vec();
  };

  // // Get the maxmin values for x and for y
  auto f_tiles_maxmin_xy = [&]
  {
    // Save lowest & highest x coords
    std::pair<i64,i64> coords_low = p.begin()->second;
    std::pair<i64,i64> coords_high = p.begin()->second;
    fn(p).ply([&](auto const& e)
    {
      auto const& tile = e.second;
      if ( tile.first < coords_low.first ) { coords_low.first = tile.first; }
      if ( tile.second < coords_low.second ) { coords_low.second = tile.second; }
      if ( tile.first > coords_high.first ) { coords_high.first = tile.first; }
      if ( tile.second > coords_high.second ) { coords_high.second = tile.second; }
    });

    return std::make_pair(coords_low, coords_high);
  };

  // // Sort candidate tiles by area overhead relative to current placed tiles
  auto f_tiles_sort_by_overhead = [&](Tiles const& tile_candidates)
  {
    auto [coords_low, coords_high] = f_tiles_maxmin_xy();

    return fn(tile_candidates).sort({}, [&](Tile const& t)
    {
      // +x right
      // +y down

      // +x
      if ( t.first > coords_high.first )
      {
        // +y = Bottom right corner
        if ( t.second > coords_high.second )
        {
          return celaeno::heuristics::chebyshev::run(t, coords_high);
        }
        // -y = Top right corner
        if ( t.second < coords_low.second )
        {
          return celaeno::heuristics::chebyshev::run(t, Tile{coords_high.first, coords_low.second});
        }
        return std::abs(t.first - coords_high.first);
      }

      // -x
      if ( t.first < coords_low.first )
      {
        // +y Bottom left corner
        if ( t.second > coords_high.second )
        {
          return celaeno::heuristics::chebyshev::run(t, Tile{coords_low.first, coords_high.second});
        }
        // -y Top left corner
        if ( t.second < coords_low.second )
        {
          return celaeno::heuristics::chebyshev::run(t, coords_low);
        }
        return std::abs(coords_low.first - t.first);
      }

      // // only +y
      // if ( t.second > coords_high.second )
      // {
      //   return std::abs(t.second - coords_high.second);
      // }
      //
      // // only -y
      // if ( t.second < coords_low.second )
      // {
      //   return std::abs(coords_low.second - t.second);
      // }

      // Inside
      return i64{};
    }).vec();
  };
  // }}}

  // Get lowest node id
  static i64 lowest{lowest_node_id(ops)};

  // Log
  {
    [[maybe_unused]] auto fold{logger.fold()};

    logger.info()("Placement:");
    for (auto e : p)
    {
      logger.info()("{}", e);
    } // for
  }

  // Get unique element without changing the order
  auto path{fp::nub(cycle)};

  // It is desirable to start from the intersection nodes in the cycle,
  // therefore:
  // - Find first occurrence of a node from inter in path
  // - Shift the path left, until the intersection elements are the first/last ones
  // - Remove intersection elements
  // - Create subrange
  Nodes slice {fn(path).rot(fn(path).find_first_of(inter)).dif(inter).vec()};

  // Log
  logger.info()("Cycle: {}", slice);
  logger.info()("Slice: {}", slice);

  // Keep a map with a list of possible tiles, for backtracking
  std::map<Node,Tiles> m_backtrack;

  // Keep track of unplaced and placed elements
  std::stack<Node> placed, unplaced;

  // Push all nodes to the stack
  fn(slice).ply([&](Node v){ if( ! p.contains(v) ){ unplaced.push(v); }  }).discard();

  // Check if cycle was already placed
  if(unplaced.empty()){ co_yield PlaceCycleRet(p,paths,mmap_node,{},false); }

  // Wire tiles
  std::unordered_map<Node,std::unordered_map<Node,std::unordered_map<Node,Tile>>> wires_of;

  while( ! unplaced.empty() )
  {
    [[maybe_unused]] auto fold{logger.fold()};

    // Get current node
    auto u{unplaced.top()}; unplaced.pop();

    // Log
    logger.info()("current node unplaced: {}", u);

    // Skip if placed
    if( p.contains(u) ){ continue; }

    // Get all neighbors
    auto f_neighbors = [&](Node v){ return fn(ops.preds(v)).chain(ops.succs(v)).vec(); };

    // Filter nodes that are not positioned
    Nodes neighbors_positioned{fn(f_neighbors(u)).in(p).vec()};

    // Log
    logger.info()("Neighbors of {} which are positioned: {}", u, neighbors_positioned);

    // Get all possible positions adjacent to positions of neighbors
    Tiles tile_candidates;

    // Keep track of unreachable nodes
    std::set<Node> unreachable;

    if( ! m_backtrack.contains(u) )
    {
      // Fetch initial candidates with a max distance of weight(u,v), for each neighbor
      tile_candidates = f_tiles_by_positioned_edge_weight(u, neighbors_positioned);
      logger.info()("Initial candidate tiles to place {} at: {}", u, tile_candidates);

      // Remove tiles behind u inputs or after u outputs
      tile_candidates = f_tiles_keep_by_y(u, neighbors_positioned, tile_candidates);
      logger.info()("Candidates that are not behind input nodes {}", tile_candidates);

      // Remove tiles that do not ahere to edge distance constraints
      tile_candidates = f_tiles_keep_by_chebyshev_distance(u, neighbors_positioned, tile_candidates);
      logger.info()("Candidates that respect edge constraints: {}", tile_candidates);

      // Filter candidates by A*
      tile_candidates = f_tiles_keep_by_astar(neighbors_positioned, tile_candidates);
      logger.info()("Candidates that are reachable through A*: {}", tile_candidates);

      // Remove duplicate positions in tile_candidates
      tile_candidates = fn(tile_candidates).sort().unique().vec();
      logger.info()("Candidates without duplicates: {}", tile_candidates);

      // Sort tile_candidates by distance x (or/and) y, to reduce area overhead
      tile_candidates = f_tiles_sort_by_overhead(tile_candidates);
      logger.info()("Candidates sorted by area overhead: {}", tile_candidates);
    } // if
    else
    {
      tile_candidates = m_backtrack.at(u);
    } // else

    // Save unreachable nodes
    if ( tile_candidates.empty() )
    {
      // Save neighbors in case they are unreachable
      std::ranges::for_each(neighbors_positioned, [&](auto e){ unreachable.insert(e); });
    }

    // Log
    logger.info()("Chosen candidates by order: {}", tile_candidates);

    // Check if tile_candidates are empty, if so, backtrack
    if (tile_candidates.empty())
    {
      // Revert changes made by u
      // Move it back to unplaced stack
      unplaced.push(u);
      // Remove its positions in m_backtrack, this is due to them being
      // invalidated, since it is based on neighbors positions, which will now
      // change
      if( m_backtrack.contains(u) ){ m_backtrack.erase(u); }
      // Failed
      if( placed.empty() )
      {
        logger.info()("Unreacheable neighbors of {}: {}", u, unreachable);
        co_yield PlaceCycleRet({},{},mmap_node,unreachable,true);
        break;
      } // if
      // Remove previous node from placed stack
      auto v{placed.top()}; placed.pop();
      // Remove A* intermediate dummy nodes between v and all its neighbors
      f_erase_dummy(v, wires_of);
      // Remove previous node from placement map
      if( p.contains(v) ){ p.erase(v); }
      // Include previous node in unplaced stack
      unplaced.push(v);
      continue;
    } // if

    // Get most promissing position
    Tile chosen{tile_candidates.front()};

    // Block paths between u and candidate
    for (auto v : neighbors_positioned)
    {
      // Calculate chebyshev-based A* path
      auto d_astar{ns_search::a_star::run(chosen
          , p.at(v)
          , [&](Tile dest) -> Tiles { return f_get_candidates(dest,1); }
          , [&](Tile dest) -> bool { return fn(p).val().has(dest); }
          , [](Tile t1, Tile t2){ return ns_heuristics::chebyshev::run(t1,t2); }
          )};

      err::err({d_astar != std::nullopt})("A* second run failed");

      // Check if path is reversed or not
      if( fn(ops.succs(u)).has(v) )
      {
        paths[std::make_pair(u,v)] = *d_astar;
      } // if
      else
      {
        paths[std::make_pair(v,u)] = fn(*d_astar).rev().deque();
      } // else

      d_astar->pop_front();
      d_astar->pop_back();

      if( fn(ops.succs(u)).has(v) )
      {
        // Save new dummy nodes in-between
        // These are were introduced by A* to block paths
        for (Tile tile : fn(*d_astar).rev().deque()) { wires_of[u][v][--lowest] = tile; } // for
        // Update mmap_node from u to v
        f_edge_update_dummy(mmap_node, u, v, fn(wires_of[u][v]).key().vec());
      }
      else
      {
        // Save new dummy nodes in-between
        // These are were introduced by A* to block paths
        for (Tile tile : *d_astar) { wires_of[u][v][--lowest] = tile; } // for
        // Update mmap_node from v to u
        f_edge_update_dummy(mmap_node, v, u, fn(wires_of[u][v]).key().vec());
      } // else
    } // for

    for (auto&& [node_target, map_dummy] : wires_of[u])
    {
      for (auto&& [node_dummy, tile] : map_dummy)
      {
        p[node_dummy] = tile;
      } // for
    } // for
    // if

    // Save node u in it
    p[u] = tile_candidates.front();

    // Erase used position
    tile_candidates.erase(tile_candidates.begin());

    // Save other positions to backtracking map
    m_backtrack[u] = tile_candidates;

    // Update placed stack
    placed.push(u);

    if( unplaced.empty() )
    {
      logger.info()("Cycle: {}\n", cycle);
      co_yield PlaceCycleRet(p,paths,mmap_node,{},false);

      // Remove previous node from placed stack
      placed.pop();

      // Remove A* intermediate dummy nodes between v and all its neighbors
      f_erase_dummy(u, wires_of);

      // Remove previous node from placement map
      if( p.contains(u) ){ p.erase(u); }

      // Include previous node in unplaced stack
      unplaced.push(u);
    } // if
  } // while

  // fmt::print("Reached partial solution\n");

} // function: place_cycle }}}

// fn: get_prox_view {{{
template<typename T, typename U>
decltype(auto) get_prox_view(Ops const& ops, T const& view, [[maybe_unused]] U const& view_ln)
{
  T out{view};

  std::queue<Node> q;

  for (auto [u,depth] : view)
  {
    // Get succs
    Nodes succs{ops.succs(u)};

    if( succs.empty() ){ continue; }

    // Check if smallest successor has a dist > 1
    auto v{fn(succs).as([&](Node v){ return std::make_pair(view.at(v),v); }).min().second};

    // If dist of v to u is gt than 1, make it 1
    if( auto depth_v{view.at(v)}; depth_v > 1 )
    {
      out.at(u) = depth_v - 1;

      // Enqueue predecessors of u
      for( auto w : ops.preds(u) ){ q.push(w); }
    } // if
  } // for

  while( ! q.empty() )
  {
    Node u{q.front()}; q.pop();

    // Get succs
    Nodes succs{ops.succs(u)};

    if( succs.empty() ){ continue; }

    // Check if smallest successor has a dist > 1
    auto v{fn(succs).as([&](Node v){ return std::make_pair(out.at(v),v); }).min().second};

    // If dist of v to u is gt than 1, make it 1
    if( auto depth_v{out.at(v)}; depth_v > 1 )
    {
      out.at(u) = depth_v - 1;

      // Enqueue predecessors of u
      for( auto w : ops.preds(u) ){ q.push(w); }
    } // if

  } // While

  return out;


} // fn: get_prox_view }}}

// fn: detect_ears {{{
template<Range R>
decltype(auto) detect_ears(Ops const& ops, R const& basis)
{
  std::vector<Nodes> out{basis.begin(), basis.end()};

  for (auto it{out.begin()}; it != out.end(); ++it)
  {
    auto base{*it};

    auto intersections = fn(out)
      .keep([&](auto&& e){ return e != base; })
      .as([&](auto&& e) { return std::make_pair(fn(e).in(base).vec(), e); })
      .keep([&](auto&& e){ return e.first.size() > 1; })
      .vec();

    // fmt::print("Cycle: {}\n", base);

    if ( intersections.size() > 1 )
    {
      auto largest = fn(intersections).max({}, [](auto e){ return e.first.size(); });
      // for (auto&& e : intersections) { fmt::print("Intersected: {}\n", e); } // for
      // fmt::print("Largest: {}\n", largest);

      bool is_subsets = fn(intersections)
        .all([&](auto e){ return fn(e.first).in(largest.first).vec().size() > 1; });

      if ( is_subsets )
      {
        // fmt::print("Ear: {}\n", base);
        // Largest cycle
        auto largest_cycle = largest.second;
        // fmt::print("Larger cycle: {}\n", largest_cycle);
        // Largest intersection
        auto largest_inter = largest.first;
        // fmt::print("Larger inter: {}\n", largest_inter);
        // Base intersection
        auto base_inter = fn(base).in(largest.first).rot([&](auto v){ return v.front() == largest_inter.front(); }).vec();
        // fmt::print("Base inter: {}\n", base_inter);
        // Save position of it
        auto it_largest = std::find(out.begin(), out.end(), largest_cycle);
        err::err({it_largest != out.end()})("Could not find cycle in basis");
        // Align intersection
        largest_cycle = fn(largest_cycle).rot([&](auto&& v){ return v.front() == largest_inter.front(); }).vec();
        // fmt::print("Aligned Larger cycle: {}\n", largest_cycle);
        // Subtract the intersection
        largest_cycle = fn(largest_cycle).dif(fn(largest_inter).pop_front(1).pop_back(1).vec()).vec();
        // fmt::print("Subtracted with largest_int: {}\n", largest_cycle);
        // Add the dif with ear
        if ( base_inter == largest_inter )
        {
          largest_cycle = fn(largest_cycle).rot(1).chain_front(fn(base).dif(base_inter).rev().vec()).vec();
        }
        else
        {
          largest_cycle = fn(largest_cycle).rot(1).chain_front(fn(base).dif(base_inter).vec()).vec();
        } // else
        // fmt::print("Dif with ear: {}\n", largest_cycle);
        // Replace cycle
        *it_largest = largest_cycle;
      }
    }
  } // for
  
  for (auto& base : out)
  {
    base = fn(base).rot([&](auto&& v){ return fn(ops.preds(v.front())).in(v).vec().size() == 2; }).vec();
  } // for
  
  return out;

} // function: detect_ears }}}

// fn: global_backtracking {{{
template<typename C>
decltype(auto) global_backtracking(Ops const& ops
  , Basis& basis
  , [[maybe_unused]] C&& m_crossing_nodes
  , Edges const& cycle_supports
  , Sink sink)
{
  err::Logger logger{sink};

  // Solution
  Placement placement;
  Paths paths;

  // Get minimal basis
  // auto basis =
  //   f_timer({}, [&] { return minimal_basis_2(i64{},ops,m_crossing_nodes,logger.sink()); });

  for (auto&& base : basis)
  {
    logger.info()("-- Base: {}\n", base);
  } // for

  // Calculate graph depth-view
  auto depth_view =
    f_timer({}, [&] { return ns_views::depth::run(0,ops.preds,ops.succs); });
  logger.info()("-- Built depth view");

  auto prox_view =
    f_timer({}, [&] { return get_prox_view(ops,depth_view.nl,depth_view.ln); });
  logger.info()("-- Built prox view");

  for (auto e : prox_view)
  {
    fmt::print("prox e: {}\n", e);
  } // for

  // Get edge weights for each basis

  MEdgeWeight m_edge_weight;
  for( auto const& cycle : basis )
  {
    auto m_cycle_edge_weight {edge_weights(ops, cycle, prox_view)};
    fn(m_cycle_edge_weight).ply([&](auto e)
    {
      m_edge_weight[e.first] = e.second;
    });
  }

  // // Get edge weights for supports
  // for (auto const& edge : cycle_supports)
  // {
  //   auto [u,v] = edge;
  //   err::err({ prox_view.at(v) > prox_view.at(u) })("v is not sucessor of u");
  //   m_edge_weight[edge]      = prox_view.at(v) - prox_view.at(u);
  //   m_edge_weight[rev(edge)] = prox_view.at(v) - prox_view.at(u);
  // } // for

  // Log
  for (auto e : m_edge_weight)
  {
    logger.info()("Weight: {}", e);
  } // for

  basis = f_timer({}, [&] { return detect_ears(ops, basis); });

  for (auto&& base : basis)
  {
    logger.info()("-- Base (ear): {}\n", base);
  } // for

  f_timer({}, [&] { minimal_basis_bfs_ordering(ops, basis); });

  // Process into adjacent intersection
  for (i32 i{}; auto const& base : basis)
  {
    logger.info()("-- Base (bfs) {}: {}", i++, base);
  } // for

  auto pair_intersection_basis =
    f_timer({}, [&] { return align_intersections(basis,logger.sink()); });

  // Process into adjacent intersection
  for (auto const& base : pair_intersection_basis)
  {
    logger.info()("-- Base (align): {}", base);
  } // for

  // Rotate while intersection has size != 2
  // pair_intersection_basis = fn(pair_intersection_basis).rot([](auto&& e){ return e.front().first.size() != 2; }).vec();

  // Reverse the pairs before processing, I don't remember why
  f_timer({}, [&] { std::reverse(pair_intersection_basis.begin(), pair_intersection_basis.end()); });

  // Get initial intersection
  auto const& intersection_initial = pair_intersection_basis.back().first;

  // Place initial intersection
  placement =
    f_timer({}, [&] { return place_intersection(ops, intersection_initial, m_edge_weight); });

  // Currently placed node relationship
  // Nodes on this map are always point to their successors
  NodeMMap mmap_node;

  // Stack generators
  std::stack<cppcoro::generator<PlaceCycleRet>> st_generator;

  // Stack of processed solutions
  std::stack<std::pair<Nodes,Nodes>> st_solutions;

  // Push intersection
  if(auto [u,v] = std::make_pair(intersection_initial.at(0),intersection_initial.at(1)); fn(ops.succs(u)).has(v) )
  {
    paths[{u,v}] = std::deque<Tile>({placement.at(u), placement.at(v)});
    mmap_node.emplace(u,v);
  } // if
  else
  {
    paths[{v,u}] = std::deque<Tile>({placement.at(v), placement.at(u)});
    mmap_node.emplace(v,u);
  } // if

  bool b_backtrack{false};

  // Save lowest id to remove path-blocking nodes inserted by cycle placement
  // algorithm
  i64 id_lowest{lowest_node_id(ops)};

  i64 i{};

  // Loop until a solution is found for every incident cycle
  std::set<Node> nodes_unreachable;
  while ( ! pair_intersection_basis.empty() )
  {
    // Get next pair intersection/placement
    Nodes intersection;
    Nodes cycle;
    cppcoro::generator<PlaceCycleRet> generator;

    if( ! b_backtrack )
    {
      intersection = pair_intersection_basis.back().first;
      cycle = pair_intersection_basis.back().second;
      pair_intersection_basis.pop_back();
      // Create generator
      generator = f_timer({}, [&]
      {
        return place_cycle(ops
            , placement
            , paths
            , mmap_node
            , cycle
            , intersection
            , m_edge_weight
            , logger.sink()
          );
      });
    } // if
    else
    {
      // Progressively remove nodes from nodes_unreachable set
      // This allows to backtrack until the earliest cycle that
      // contains one or more unreachable nodes(s)
      err::err({ ! st_solutions.empty() })("Solution not found");

      err::err({ st_solutions.size() == st_generator.size() })
        ("Number of solutions differ from number of generators");

      logger.info()("----------------");
      logger.info()("Unreacheable: {}", nodes_unreachable);
      if ( ! nodes_unreachable.empty() )
      {
        while ( ! nodes_unreachable.empty() and ! st_solutions.empty() )
        {
          auto e{st_solutions.top()}; st_solutions.pop();

          intersection = e.first;
          cycle = e.second;

          // Check if this cycle contains unreachable nodes
          if ( auto intersect = fn(cycle).in(nodes_unreachable).vec(); ! intersect.empty() )
          {
            logger.info()("Cycle: {}", cycle);
            logger.info()("Intersect: {}", intersect);
            std::ranges::for_each(intersect, [&](auto _1){ nodes_unreachable.erase(_1); });
          } // if

          // Get previous generator
          generator = std::move(st_generator.top()); st_generator.pop();

          // Re-insert intersection and base into the to-place vector
          if ( ! nodes_unreachable.empty() )
          {
            pair_intersection_basis.push_back(std::make_pair(intersection,cycle));
          } // if
        } // while
      } // if
      else
      {
        auto e{st_solutions.top()}; st_solutions.pop();

        intersection = e.first;
        cycle = e.second;

        // Get previous generator
        generator = std::move(st_generator.top()); st_generator.pop();
      } // else

      // Disable backtracking
      b_backtrack = false;

      logger.info()("----------------");
    } // else

    // If a solution is found from current placement
    // - Update final solution
    // - Push generator to backtracking stack
    // - Push incident cycle to backtracking stack

    auto it_gen{generator.begin()}; 

    auto check_half_separations = [&]
    {
      while ( it_gen != generator.end() && it_gen->m_failed == false )
      {
        placement = it_gen->m_placement;
        paths = it_gen->m_paths;
        mmap_node = it_gen->m_mmap_node;

        // Check for half separation
        // Test resulting cycle, to see if it hasn't twisted within itself
        // // Re-create cycle with dummy nodes
        Nodes cycle_with_dummy = fn(cycle)
          .mut([](auto e){ e.push_back(e.front()); return e; })
          .slide(2)
          .as([&](auto e)
          {
            Nodes out;
            i64 u = e.at(0);
            i64 v = e.at(1);

            // u must be sucessor of v
            bool is_reversed{false};
            if ( ! fn(ops.succs(u)).has(v) ) { is_reversed = true; v = std::exchange(u,v); }
            logger.info()("u,v: {},{}\n", u, v);
            
            // If endpoint is not v and is not introduced by A*, remove
            auto remove_by = [&](Edges& _1)
            {
              _1 = fn(_1).keep([&](Edge const& __1)
              {
                // Check if is goal 'v'
                if ( __1.second == v )       { return true; }
                // Check if is introduced by A*
                if ( ! ops.has(__1.second) ) { return true; }
                // Discard
                return false;
              }).vec();
            };

            if ( auto opt_nodes = path_from_multimap(mmap_node, u, v, remove_by) )
            {
              logger.info()("Nodes between uv: {}\n", *opt_nodes);
              if ( is_reversed ) { std::ranges::reverse(*opt_nodes); }
              return *opt_nodes;
            }

            return (is_reversed)? Nodes{v,u} : Nodes{u,v};
          })
          .squash()
          .unique()
          .vec();
        logger.info()("Dummy Cycle: {}\n", cycle_with_dummy);

        bool has_inner_crossings = cycle_has_inner_crossings(cycle_with_dummy, placement, sink);

        if ( ! placement.empty() )
        {
          Placement _placement {placement};
          Paths _paths {paths};
          std::multimap<Node,Node> _node_map{mmap_node};

          // Offset coordinates to remove negative values
          auto x_min{rg::min_element(_placement,{},[](auto e){ return e.second.first; })->second.first};
          auto y_min{rg::min_element(_placement,{},[](auto e){ return e.second.second; })->second.second};

          for (auto& [n,p] : _placement)
          {
            auto& [x,y] = p;

            x += std::abs(x_min);
            y += std::abs(y_min);
          } // for

          // Expand grid to solve crossings
          for (auto& [n,p] : _placement)
          {
            auto& [x,y] = p;

            x *= 2;
            y *= 2;
          } // for

          for(auto& [pair,path] : _paths)
          {
            for(Tile& tile : path)
            {
              tile.first += std::abs(x_min);
              tile.second += std::abs(y_min);
              tile.first *= 2;
              tile.second *= 2;
            }
          }

          // Draw
#ifdef DEBUG
          // Draw only cycles with inner crossings
          if ( ! has_inner_crossings )
          {
            logger.info()("-- Figure {}", i+1);
            logger.info()("-- cycle: {}", cycle);
            logger.info()("-- placement: ");
            for (auto&& e : placement)
            {
              logger.info()("-- e: {}", e);
            } // for
            logger.info()("-- Start draw");
            ns_draw::svg::svg(fmt::format("out/step-{}.svg", ++i)
              , _placement
              , _paths
              , [](auto e){ return e; });
            logger.info()("-- End draw");
            logger.info()("-- Placed edges:");
            std::ranges::for_each(_node_map, [&](auto e){ logger.info()("e: {}", e); });
          } // if has_inner_crossings
#endif
        }

        if ( has_inner_crossings )
        {
          logger.info()("Cycle {} has inner crossings\n", i);
          it_gen = std::next(it_gen);
        }
        else
        {
          logger.info()("Cycle {} has no inner crossings\n", i);
          break;
        } // else
      }
    };

    check_half_separations();

    if( it_gen != generator.end() && it_gen->m_failed == false )
    {
      // placement = it_gen->placement;
      // paths = it_gen->paths;
      st_generator.push(std::move(generator));
      st_solutions.push(std::make_pair(intersection,cycle));
      logger.info()("-- Success for cycle: {}", cycle);
      logger.info()("-- Placement:");
      for (auto e : placement) { logger.info()("e: {}", e); } // for

    } // if
    // Else if a solution is not found
    // - Push the incident basis onto unfinished stack
    // - Enable backtracking
    else
    {
      pair_intersection_basis.push_back(std::make_pair(intersection,cycle));
      b_backtrack = true;
      nodes_unreachable = (it_gen != generator.end())? it_gen->m_unreachable : std::set<Node>{};
      logger.info()("-- Failed for cycle: {}", cycle);
    } // else

  } // while


  std::erase_if(placement,[&](auto&& e){ return e.first < id_lowest; });

  return std::make_pair(placement,paths);

} // fn: global_backtracking }}}

// fun: main  {{{
int main([[maybe_unused]] int argc, char const* argv[])
{
  // Read graph
  ns_graph::Graph<i64> g;
  auto emplace = [&g](auto&& e) -> void { g.emplace(e); };

  auto metadata {ns_io_verilog::Reader{argv[1],emplace}};

  // Helpers
  auto f_p = [&g](auto v){ return g.predecessors(v); };
  auto f_s = [&g](auto v){ return g.successors(v); };
  auto f_a = [&g](auto u, auto v){ return g.adjacent(u,v); };
  auto f_l = [&g](auto u, auto v){ g.emplace(std::make_pair(u,v)); };
  auto f_u = [&g](auto u, auto v){ g.erase(std::make_pair(u,v)); };
  auto f_h = [&g](auto u){ return g.has(u); };

  err::Logger logger;


  fmt::print("Gates: {}\n", g.vertices_count());
  fmt::print("Wires: {}\n", g.edges_count());

  // Create ops
  Ops ops(f_p, f_s, f_a, f_l, f_u, f_h);

  auto f_write_v = [&]<typename... Args>(Args&&... args)
  {
    ns_io_verilog::Writer(std::forward<Args>(args)...);
  };

  auto f_write_d = [&]<typename... Args>(Args&&... args)
  {
    ns_io_dimacs::Writer(std::forward<Args>(args)...);
  };

  f_timer({}, f_write_v, metadata.data(), f_p, f_s, "out/0-out.v");

  f_timer({}, [&]{ celaeno::graph::operations::balance::outgoing::run(0,ops); });

  f_timer({}, f_write_v, metadata.data(), f_p, f_s, "out/1-out.v");

  f_timer({}, [&]{ celaeno::graph::operations::balance::paths::run(0,ops); });

  f_timer({}, f_write_v, metadata.data(), f_p, f_s, "out/2-out.v");

  auto m_crossing_nodes = f_timer({}, [&]{ return celaeno::graph::operations::balance::crossings::run(0,ops); });

  // unbalance(0,ops);

  f_timer({}, f_write_v, metadata.data(), f_p, f_s, "out/3-out.v");

  f_timer({}, f_write_d, g.data(), f_p, f_s, "out/3-out.dimacs");

  //
  // Balance cycles
  //
  
  std::vector<std::vector<std::vector<Node>>> e_basis = e_basis_b1;

  // Read cycle output
  // Decode to original values in graph
  // Get all nodes through bfs
  auto bfs{ns_search::bfs::run(0,ops.preds,ops.succs)};

  // Normalize node indices to start from 1, and be sequential
  std::map<i64,i64> m_nodes_norm;
  fn(bfs)
    .zip(rv::ints(u64{},bfs.size()) | rg::to<std::vector>)
    .ply([&](auto&& e){ m_nodes_norm[e.second] = e.first; });

  Basis basis = fn(e_basis)
    .as([](auto&& e){  return fn(e).squash().sort().unique().vec(); })
    .as([&](auto&& e){ return fn(e).as([&](auto _1) { return m_nodes_norm.at(_1); }).vec(); })
    .vec();

  // Align cycles from upper node to bottom node
  auto f_align_cycle = [&](auto const& cycle)
  {
    // auto [fst,snd] = cycle_split_in_endpoints(ops, cycle);
    // return fn(fst).pop_back(1).chain(snd).pop_back(1).vec();
    Nodes out;

    // Find node with 2 preds in cycle
    auto it = std::ranges::find_if(cycle, [&](auto e){ return fn(ops.preds(e)).in(cycle).vec().size() == 2; });
    err::err({it != std::ranges::end(cycle)})("Could not find cycle endpoint");

    // Push initial node
    out.push_back(*it);

    // Use a bfs to construct the rest of the cycle
    auto f_preds_in_cycle = [&](auto&& e){ return fn(ops.preds(e)).in(cycle).vec(); };
    ns_search::bfs::run(*it, f_preds_in_cycle, [](auto e) { return Nodes{}; },
    [&](auto e)
    {
      if ( auto succs = fn(ops.succs(e)).in(cycle).vec(); ! succs.empty() )
      {
        if ( succs.front() == out.front() ) { out.insert(out.begin(), e); }
        else { out.push_back(e); }
      }
      return false;
    });

    return out;
  };

  basis = fn(basis).as([&](auto&& e){ return f_align_cycle(e); }).vec();

  fn(basis).ply([&](auto&& e){ logger.info()("Basis: {}\n", e); });


  // Calculate graph depth-view and prox-view
  auto depth_view = f_timer({}, [&] { return ns_views::depth::run(0,ops.preds,ops.succs); });
  logger.info()("-- Built depth view");

  auto prox_view = f_timer({}, [&] { return get_prox_view(ops,depth_view.nl,depth_view.ln); });
  logger.info()("-- Built prox view");

  auto cycle_supports = f_timer({}, [&] { return get_cycle_supports(ops, basis, prox_view); });
  logger.info()("-- Built cycle supports");

  view_draw(prox_view);

  // Perform placement
  // std::cerr << "Started computation\n";
  auto start {std::chrono::system_clock::now()};
  auto [placement, routing] {global_backtracking(ops,basis,m_crossing_nodes,cycle_supports,logger.sink())};
  auto end {std::chrono::system_clock::now()};
  // std::cerr << "Finished computation\n";
  std::chrono::duration<f64> dur {end-start};
  std::stringstream ss; ss << dur.count();

  fmt::print("Circuit: {}\n", argv[1]);
  fmt::print("Time(s): {}\n", ss.str());

  // Offset coordinates to remove negative values
  auto x_min{rg::min_element(placement,{},[](auto e){ return e.second.first; })->second.first};
  auto y_min{rg::min_element(placement,{},[](auto e){ return e.second.second; })->second.second};

  for (auto& [n,p] : placement)
  {
    auto& [x,y] = p;

    x += std::abs(x_min);
    y += std::abs(y_min);
  } // for

  // Expand grid to solve crossings
  for (auto& [n,p] : placement)
  {
    auto& [x,y] = p;

    x *= 2;
    y *= 2;
  } // for

  for(auto& [pair,path] : routing)
  {
    for(Tile& tile : path)
    {
      tile.first += std::abs(x_min);
      tile.second += std::abs(y_min);
      tile.first *= 2;
      tile.second *= 2;
    }
  }

  // fmt::print("\n------\n");
  // for( auto&& e : placement ){ fmt::print("{}\n", e); }
  // fmt::print("------\n");

  // Print area
  std::pair<int,int> pair_min = placement.begin()->second;
  std::pair<int,int> pair_max = placement.begin()->second;
  fn(placement).ply([&](auto const& e)
  {
    auto const& tile = e.second;
    if ( tile.first < pair_min.first ) { pair_min.first = tile.first; }
    if ( tile.second < pair_min.second ) { pair_min.second = tile.second; }
    if ( tile.first > pair_max.first ) { pair_max.first = tile.first; }
    if ( tile.second > pair_max.second ) { pair_max.second = tile.second; }
  });

  fmt::print("Area: {}x{}\n"
    , std::abs(pair_max.first - pair_min.first)
    , std::abs(pair_max.second - pair_min.second)
  );

  // Draw
  ns_draw::svg::svg("out/out.svg"
    , placement
    , routing
    , [](auto i){ return i; }
  );


  return EXIT_SUCCESS;
} // main }}}
