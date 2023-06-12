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

#include <celaeno/graph/graph.hpp>
#include <celaeno/graph/io/verilog.hpp>
#include <celaeno/graph/io/dimacs.hpp>

#include <celaeno/graph/views/depth.hpp>

#include <celaeno/graph/draw/svg.hpp>
#include <celaeno/graph/representations/grid.hpp>

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

// fn: find_cycle_endpoints {{{
//
// A cycle has two endpoints, one diverges into two edge, and the other converges two edges. This
// function finds these endpoints
//
decltype(auto) find_cycle_endpoints(Ops const& ops, Range auto&& cycle)
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
} // function: find_cycle_endpoints }}}

// fn: split_cycle_in_endpoints {{{
//
// A cycle has two endpoints, one diverges into two edge, and the other converges two edges. This
// function splits the cycle into two half cycles, the parents are included in both half cycles as
// the endpoints
//
decltype(auto) split_cycle_in_endpoints(Ops const& ops, Range auto&& cycle)
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
  ns_search::bfs::run(*it_endpoint1, f_preds_in_cycle, [](auto e) { return Nodes{}; },
  [&](auto e)
  {
    if ( auto succs = f_succs_in_cycle(e); ! succs.empty() )
    {
      if ( succs.front() == whole.front() ) { whole.insert(whole.begin(), e); }
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
} // function: split_cycle_in_endpoints }}}

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

decltype(auto) make_line_equation(Tile const& p1, Tile const& p2)
{
  err::err({p1.first != p2.first})("Attempt to calculate equation of straight line");

  i64 m = (p2.second - p1.second) / (p2.first - p1.first);
  i64 b = p1.second - m * p1.first;

  fmt::print("-- line m: {}\n", m);
  fmt::print("-- line b: {}\n", b);

  return MakeLineEquationRet{.fun=[=](i64 x){ return m*x + b; }, .m=m, .b=b};
} // function: make_line_equation }}}

// fn: cycle_has_half_separation {{{
//
// This function checks if a positioned cycle halves are separate, this separation consists of a
// straight line from one endpoint to another.
//
decltype(auto) cycle_has_half_separation(Ops const& ops, Nodes const& cycle, Placement const& m_node_pos)
{
  for (auto e : m_node_pos)
  {
    fmt::print("placement: {}\n", e);
  } // for

  // Get endpoints
  auto [epn1,epn2] = find_cycle_endpoints(ops, cycle);
  fmt::print("Cycle endpoints: {} | {}\n", epn1, epn2);

  // Split the cycle in two halves
  auto [chf1,chf2] = split_cycle_in_endpoints(ops, cycle);

  fmt::print("Cycle half 1: {}\n", chf1);
  fmt::print("Cycle half 2: {}\n", chf2);

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
    f_equation = make_line_equation(ep1, ep2).fun;
    f_is_below = [=](Tile t){ return t.second < f_equation(t.first); };
  }
  else
  {
    auto [x_min_it,x_max_it] = fn(cycle).minmax({}, [&](Node e){ return m_node_pos.at(e); });
    auto [x_min, x_max] = std::make_pair(m_node_pos.at(*x_min_it).first*2, m_node_pos.at(*x_max_it).first*2);
    i64 x_mea = (x_max+x_min)/2;

    fmt::print("x_mea: {}\n", x_mea);

    f_is_below = [=](Tile t)
    {
      // i64 x_base = ep1.first;
      // Consider 'below' being to the left
      return t.first < x_mea;
    };
  } // else

  std::function<bool(Tile)> f_is_above = [&](Tile const& t){ return ! f_is_below(t); };


  fmt::print("Cycle endpoints: {} | {}\n", ep1, ep2);

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
    fmt::print("Node {} with Point {} is below? {}\n", n, t_fst, is_below);
    for( auto it{std::next(cycle_half.begin())}; it != cycle_half.end(); ++it )
    {
      err::err({ m_node_pos.contains(*it) })("Cycle node {} not found in placement", *it);

      // Other points must also be the same as the first
      Tile t = m_node_pos.at(*it);
      t.first *= 2; t.second *= 2; t.second = -t.second;

      fmt::print("Node {} with point {} is below? {}\n", *it, t, f_is_below(t));

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
decltype(auto) cycle_has_inner_crossings(Ops const& ops
    , Nodes cycle
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

  // Given three collinear points p, q, r, the function checks if 
  // point q lies on line segment 'pr' 
  auto f_on_segment = [](Tile p, Tile q, Tile r)
  {
    if ( q == p or q == r )
    {
      return false;
    }

    if ( (q.first <= std::max(p.first, r.first))
        and (q.first >= std::min(p.first, r.first))
        and (q.second <= std::max(p.second, r.second))
        and (q.second >= std::min(p.second, r.second)))
    {
      return true;
    }

    return false;
  };

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
    auto contains = []<Range R, typename... E>(R&& r, E&&... e)
      requires IsPairsOf<Node,E...> // Edges must be node pairs
    {
      return (r.contains(e) or ...);
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

// fn: minimal_basis_3 {{{
template<SignedIntegral I, typename C>
[[nodiscard]] decltype(auto) minimal_basis_3(I root, Ops const& ops, C&& m_crossing_nodes, Sink sink)
{
  // Init logger
  err::Logger logger{sink};

  std::vector<Nodes> out;
  std::queue<Nodes> q_cycles;

  auto depth_view = ns_views::depth::run(root, ops.preds, ops.succs);

  // std::set<Node> s_visited_global;
  //
  // Find first cycle
  //
  for (auto [layer,nodes] : depth_view.ln)
  {
    if ( ! q_cycles.empty() ) { break; }

    // Check for nodes with predecessors == 2 (possible cicle endpoints)
    fmt::print("l: {} n: {}\n", layer, nodes);

    nodes = fn(nodes).keep([&](auto n){ return ops.preds(n).size() == 2; }).vec();

    fmt::print("closes: {}\n", nodes);

    for(Node n : nodes)
    {
      if ( ! q_cycles.empty() ) { break; }

      fmt::print("n: {}\n", n);
      // Traverse predecessor tree in bfs manner
      std::set<Node> s_visited_local;
      s_visited_local.insert(n);
      std::map<Node,Node> m_cycle;

      // Queue of vertices
      std::queue<Node> queue;

      // Push initial vertex into the queue
      queue.push(n);

      while( ! queue.empty() )
      {
        // Get next vertex
        auto vertex {queue.front()}; queue.pop();

        // Get the vertices
        auto v_vertices = ops.preds(vertex);

        // Enqueue next
        for( auto e : v_vertices ) { queue.push(e); }

        //
        // Perform operations
        //
        fmt::print(":: vertex: {}\n", vertex);

        // Skip source node
        if ( vertex == n ) { continue; }

        // // Check if was previously visited
        // bool is_visited{s_visited_local.contains(vertex)};

        // Get parents of current node
        auto v_parents = fn(ops.succs(vertex)).in(s_visited_local).vec();

        // Select shortest path (parent in highest layer)
        if ( v_parents.size() > 1 )
        {
          v_parents = fn(v_parents).sort({}, [&](auto e) { return depth_view.nl.at(e); }).rev().vec();
        } // else if
        else if ( v_parents.size() == 0 )
        {
          continue;
        } // else parents == 1

        // Update cycle map
        m_cycle[vertex] = v_parents.at(0);

        // Add node to visited set
        s_visited_local.insert(vertex);

        // Check if node has 2 parents in the visited set (possible cycle)
        if ( v_parents.size() == 2 )
        {
          // Check if cycle closes in 'n'
          Nodes v_fst_half;
          Nodes v_snd_half;

          // Create path from map
          auto f_make_path = [&](Node n)
          {
            Nodes out;
            out.push_back(n);
            while ( m_cycle.contains(n) )
            {
              n = m_cycle.at(n);
              out.push_back(n);
            }
            return out;
          };

          // Create path from outputs of 'vertex'
          v_fst_half = f_make_path(v_parents.at(0));
          v_snd_half = f_make_path(v_parents.at(1));

          // Closes in 'n'
          if( fn(v_fst_half).in(v_snd_half).vec().size() == 1 )
          {
            // Build cycle
            Nodes cycle;

            // fmt::print("Map:");
            // for (auto&& e : m_cycle)
            // {
            //   fmt::print("e: {}\n", e);
            // } // for

            // Insert lower divergence
            cycle.push_back(vertex);

            // Insert cycle halves
            v_snd_half.pop_back();
            std::reverse(v_snd_half.begin(), v_snd_half.end());

            cycle = fn(cycle).chain(v_fst_half).chain(v_snd_half).vec();

            fmt::print("cycle: {} → {}\n", vertex, cycle);

            out.push_back(cycle);

            // Mark initial cycle as found
            q_cycles.push(cycle);

            // Found cycle for current node 'n'
            break;
          } // if fn(v_fst_half).in(v_snd_half).vec().size() == 1
        } // if parents == 2
      } // while
    } // for
  } // for


  //
  // Use cycle adjacencies to find other cycles
  //

  // Keep track of visited edges
  std::set<Edge> s_edge_visited;

  // Check if edge is visited
  auto f_is_edge_visited = [&](Edge e)
  {
    return s_edge_visited.contains(e)
      || s_edge_visited.contains(Edge{e.second, e.first});
  };

  // Visit edges in a cycle
  auto f_visit_edges = [&](auto const& cycle)
  {
    fn(cycle)
      .mut([](auto e){ e.push_back(e.front()); return e; }) // Duplicate first element in last position
      .pairs() // Make pairs [a,b,c,d,a] → [[a,b],[b,c],[c,d],[d,a]]
      .ply([&](Edge e){ s_edge_visited.insert(e); });
  };

  // Visit first cycle edges
  f_visit_edges(q_cycles.front());

  // Lambda to split subranges that might contains new cycles
  // Split cycle into paths in-between unvisited edges
  // // 1. Check if node has successors not in cycle
  // //   1.1 If it does mark as 'u'
  // // 2. Check if another node not in cycle
  // //   2.1 If it does mark as 'v'
  // // 3. Save the path from 'u' to 'v', mark 'v' as 'u' and goto 2
  auto f_split_subranges = [&](auto const& cycle)
  {
    // Return subranges
    std::vector<Nodes> out;
    // Subrange markers
    std::vector<int> subrange_indices;
    for( auto it{cycle.begin()}; it != cycle.end(); ++it )
    {
      if ( fn(ops.succs(*it)).dif(cycle).vec().size() != 0 )
      {
        subrange_indices.push_back(std::distance(cycle.begin(), it));
      }
    }
    // Create subranges
    if ( subrange_indices.size() > 1)
    {
      auto f_next = [&](auto e){  return std::next(cycle.begin(), e); };
      out = fn(subrange_indices)
        .pairs()
        .as([&](auto e){ return Nodes( f_next(e.first), f_next(e.second+1) ); })
        .vec();

      // Last subrange
      // Nodes subrange_last( f_next(subrange_indices.back()), cycle.end() );
      // subrange_last.insert(subrange_last.end(), cycle.begin(), f_next(subrange_indices.front()+1));
      // out.push_back(subrange_last);
    }
    return out;
  };

  auto f_find_cycle = [&](auto const& path)
  {
    fmt::print("Path: {}\n", path);

    std::vector<Nodes> _out;

    // Keep path map
    std::map<Node,Node> m_cycle;

    // Cycle must be formed from path endpoints
    std::queue<Node> q;
    q.push(path.front());
    q.push(path.back());

    // Keep track of visited nodes
    std::set<Node> s_visited;

    // Start main loop
    while( ! q.empty() )
    {
      auto u = q.front(); q.pop();
      fmt::print("Node: {}\n", u);

      // Get non-visited successors
      auto succs = fn(ops.succs(u)).keep([&](auto e){ return ! f_is_edge_visited(Edge{u,e}); }).vec();
      fmt::print("Succs: {}\n", succs);

      // Enqueue unvisited successors
      for (auto e : succs) { q.push(e); } // for

      // Mark u as visited
      s_visited.insert(u);

      // Ignore initial endpoints
      if ( u == path.front() or u == path.back() ) { continue; }

      // Get children of current node
      auto v_children = fn(ops.preds(u)).in(s_visited).vec();

      // Select shortest path (parent in lowest layer)
      if ( v_children.size() > 1 )
      {
        v_children = fn(v_children).sort({}, [&](auto e) { return depth_view.nl.at(e); }).vec();
      } // else if
      else if ( v_children.size() == 0 )
      {
        continue;
      } // else children == 1

      fmt::print("v_children: {}\n", v_children);

      // Update cycle map
      m_cycle[u] = v_children.at(0);

      // Check if node has 2 children in the visited set (possible cycle)
      if ( v_children.size() == 2 )
      {
        // Check if cycle closes in 'n'
        Nodes v_fst_half;
        Nodes v_snd_half;

        // Create path from map
        auto f_make_path = [&](Node n)
        {
          Nodes _out;
          _out.push_back(n);
          while ( m_cycle.contains(n) )
          {
            n = m_cycle.at(n);
            _out.push_back(n);
          }
          return _out;
        };

        // Create path from outputs of 'vertex'
        v_fst_half = f_make_path(v_children.at(0));
        v_snd_half = f_make_path(v_children.at(1));
        fmt::print("v_fst_half: {}\n", v_fst_half);
        fmt::print("v_snd_half: {}\n", v_snd_half);

        // Closes in path endpoints
        if(
            (v_fst_half.back() == path.front() && v_snd_half.back() == path.back())
            ||
            (v_fst_half.back() == path.back() && v_snd_half.back() == path.front())
          )
        {
          // Build cycle
          Nodes cycle;

          // Check which half has first element of path
          if ( v_fst_half.back()  !=  path.front() &&
               v_fst_half.front() !=  path.front()
             ) { v_snd_half = std::exchange(v_fst_half, v_snd_half); }
          if ( v_fst_half.back() !=  path.front() ) { std::ranges::reverse(v_fst_half); }
          if ( v_snd_half.front() !=  path.back() ) { std::ranges::reverse(v_snd_half); }

          cycle = fn(cycle)
            .chain(v_fst_half)
            .pop_back(1)
            .chain(path)
            .pop_back(1)
            .chain(v_snd_half)
            .mut([&](auto v){ v.push_back(u); return v; })
            .vec();

          // Found cycle for current node 'n'
          fmt::print("cycle: {} → {}\n", u, cycle);

          _out.push_back(cycle);

          break;
        } // if fn(v_fst_half).in(v_snd_half).vec().size() == 1
      } // if children == 2
    }

    return _out;
  };

  // Start cycle detection
  while( ! q_cycles.empty() )
  {
    auto cycle_curr = q_cycles.front(); q_cycles.pop();
    fmt::print("cycle_curr: {}\n", cycle_curr);

    // Split subranges
    auto subranges = f_split_subranges(cycle_curr);
    fmt::print("subranges: {}\n", subranges);

    // Search each subrange for a cycle with bfs
    std::vector<Nodes> cycles_new;
    for (auto&& subrange : subranges)
    {
      auto tmp = f_find_cycle(subrange);
      cycles_new.insert(cycles_new.end(), tmp.begin(), tmp.end());
    } // for

    // Include new cycles in queue
    for(auto const& e : cycles_new) { q_cycles.push(e); }

    // Mark new cycle edges as visited
    for(auto const& e : cycles_new) { f_visit_edges(e); }

    // Update result with new cycles
    out.insert(out.end(), std::make_move_iterator(cycles_new.begin()), std::make_move_iterator(cycles_new.end()));
  }

  for (auto const& cycle : out)
  {
    fmt::print("cycle: {}\n", cycle);
  } // for

  exit(0);

  return out;

} // fn: minimal_basis_3 }}}

// fn: minimal_basis_2 {{{
template<SignedIntegral I, typename C>
[[nodiscard]] decltype(auto) minimal_basis_2(I root, Ops const& ops, C&& m_crossing_nodes, Sink sink)
{
  // Init logger
  err::Logger logger{sink};

  std::vector<Nodes> out;

  auto depth_view = ns_views::depth::run(root, ops.preds, ops.succs);

  std::set<Node> s_visited_global;

  for (auto [layer,nodes] : depth_view.ln)
  {
    // Check for nodes with predecessors == 2 (possible cicle endpoints)
    // fmt::print("l: {} n: {}\n", layer, nodes);

    nodes = fn(nodes).keep([&](auto n){ return ops.preds(n).size() == 2; }).vec();

    // fmt::print("closes: {}\n", nodes);

    for(Node n : nodes)
    {
      // fmt::print("n: {}\n", n);
      // Traverse predecessor tree in bfs manner
      std::set<Node> s_visited_local;
      s_visited_local.insert(n);
      std::map<Node,Node> m_cycle;
      experimental::bfs_repeat::run(n
      // ns_search::bfs::run(n
          // Do not backtrack previously visited nodes
        , [&](Node n){ return ops.preds(n); }
          // Only go backwards in the directed graph
        // , [](Node){ return std::vector<Node>{}; }
          // Main
        , [&](Node o)
        {
          // fmt::print(":: o: {}\n", o);

          // Skip source node
          if ( o == n ) { return false; }

          // Check if was previously visited
          bool is_visited{s_visited_local.contains(o)};

          // Get parents of current node
          auto v_parents = fn(ops.succs(o)).in(s_visited_local).vec();

          // Select shortest path (parent in highest layer)
          if ( v_parents.size() > 1 )
          {
            v_parents = fn(v_parents).sort({}, [&](auto e)
            {
              return depth_view.nl.at(e);
            })
            .rev()
            .vec();
          } // else

          // Update cycle map
          m_cycle[o] = v_parents.at(0);

          // Add node to visited set
          s_visited_local.insert(o);

          // Check if node has 2 parents in the visited set (possible cycle)
          if ( v_parents.size() == 2 )
          {
            // Check if cycle closes in 'n'
            Nodes v_fst_half;
            Nodes v_snd_half;

            // Create path from map
            auto f_make_path = [&](Node n)
            {
              Nodes out;
              out.push_back(n);
              while ( m_cycle.contains(n) )
              {
                n = m_cycle.at(n);
                out.push_back(n);
              }
              return out;
            };

            // Create path from outputs of 'o'
            v_fst_half = f_make_path(v_parents.at(0));
            v_snd_half = f_make_path(v_parents.at(1));

            // Closes in 'n'
            if( fn(v_fst_half).in(v_snd_half).vec().size() == 1 )
            {
              // Build cycle
              Nodes cycle;

              // fmt::print("Map:");
              // for (auto&& e : m_cycle)
              // {
              //   fmt::print("e: {}\n", e);
              // } // for

              // Insert lower divergence
              cycle.push_back(o);

              // Insert cycle halves
              v_snd_half.pop_back();
              std::reverse(v_snd_half.begin(), v_snd_half.end());

              cycle = fn(cycle)
                .chain(v_fst_half)
                .chain(v_snd_half)
                .vec();

              // fmt::print("cycle: {} → {}\n", o, cycle);
              out.push_back(cycle);
              // Break
              return true;
            }

            // Continue
            return false;
          }
          // Continue
          return false;
        }
      );
    }

  } // for

  // exit(0);

  return out;

} // fn: minimal_basis_2 }}}

// fn: minimal_basis {{{
template<SignedIntegral I, typename C>
[[nodiscard]] decltype(auto) minimal_basis(I root, Ops const& ops, C&& m_crossing_nodes, Sink sink)
{
  // Init logger
  err::Logger logger{sink};

  // Start interpreter
  py::scoped_interpreter guard{};

  // Import networkx
  py::module_ networkx = py::module_::import("networkx");

  // Create networkx graph
  auto gnx{networkx.attr("Graph")()};

  // Insert edges
  auto bfs = ns_search::bfs::run(root,ops.preds,ops.succs,
    [&](Node u)
    {
      for( Node v : ops.succs(u) )
      {
        gnx.attr("add_edge")(u,v);
      }
      return false;
    });

  // Calculate basis
  auto basis {py::cast<std::vector<std::vector<Node>>>(networkx.attr("minimum_cycle_basis")(gnx))};

  // Adjust each base, such that, for [u,v], u is a predecessor of v
  for(auto& base : basis)
  {
    std::vector<Node> out;

    auto f_succs = [&](Node u){ return fn(ops.succs(u)).in(base).vec(); };

    // Get node with two outgoing edges that belong to base
    auto it{rg::find_if(base,[&](Node u){ return f_succs(u).size() == 2; })};

    err::err({ it != base.end() })("Outgoing base not found");

    // Include node with two outgoing edges in result
    out.push_back(*it);

    // Get both successors
    auto succs {f_succs(*it)};

    // For each create paths in out
    auto succ_fst{succs.front()};
    auto succ_snd{succs.back()};

    out.push_back(succ_fst);

    for(succs = f_succs(succ_fst); succs.size() != 0; succs = f_succs(succ_fst))
    {
      succ_fst = succs.front();
      out.push_back(succ_fst);
    }

    out.insert(out.begin(), succ_snd);

    for(succs = f_succs(succ_snd); succs.size() != 0; succs = f_succs(succ_snd))
    {
      succ_snd = succs.front();
      out.insert(out.begin(),succ_snd);
    }

    out.pop_back();

    base = out;
  }

  for (u64 i{}; auto e : basis)
  {
    logger.info()("Basis {}: {}", i++, e);
  } // for

  return basis;
} // fn: minimal_basis }}}

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
      if ( intersection.size() > 1 )
      {
        c1 = fn(c1).rot([&](auto v)
        {
          return fn(v).cut(u64{}, intersection.size()).in(intersection).vec().size() == intersection.size();
        }).vec();

        c2 = fn(c2).rot([&](auto v)
        {
          return fn(v).cut(u64{}, intersection.size()).in(intersection).vec().size() == intersection.size();
        }).vec();

        // Check if needs reverse
        u64 size_intersection = intersection.size();
        if ( fn(c1).cut(u64{}, size_intersection).vec() != fn(c2).cut(u64{}, size_intersection).vec() )
        {
          std::ranges::reverse(c2);
          std::ranges::rotate(c2, c2.end() - 1);
        }

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

  return out;
} // fn: align_intersections }}}

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

// enum: Partition {{{
enum class Partition
{
  L,
  R,
}; // enum: Partition }}}

// struct: Adjacencies {{{
struct Adjacencies
{
  Tile src;
  i64 dist_exact;
  Tiles adjacencies;

  template<typename T>
  Adjacencies(T&& src, i64 dist_exact);
  Tiles tiles();
};

template<typename T>
Adjacencies::Adjacencies(T&& src, i64 dist_exact)
  : src(src)
  , dist_exact(dist_exact)
  , adjacencies({})
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
      this->src
    , f_tiles_adjacent
    , [](Tile t){ return Tiles{}; }
    , [&](Tile t)
      {
        auto dist_cur{ns_heuristics::chebyshev::run(this->src,t)};

        if( dist_cur > this->dist_exact+1 )
        {
          return true; // stop
        } // if

        if( dist_cur == this->dist_exact )
        {
          this->adjacencies.push_back(t);
        } // else if

        return false; // continue
      }
  );

  return this->adjacencies;
}

// struct: Adjacencies }}}

// fn: place_intersection {{{
auto place_intersection(Ops const& ops, Range auto intersection)
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
  if ( fn(ops.succs(intersection.at(0))).has(intersection.at(1)) != 0 )
  {
    placement[intersection.at(1)] = std::make_pair(-1,-1);
    placement[intersection.at(0)] = std::make_pair(0,0);
  } // if
  else
  {
    placement[intersection.at(1)] = std::make_pair(0,0);
    placement[intersection.at(0)] = std::make_pair(-1,-1);
  } // else

  return placement;
} // function: place_intersection }}}

// fn: place_cycle {{{
struct PlaceCycleRet
{
  Placement placement;
  Paths paths;
  std::set<Node> unreachable;
  bool failed;
  PlaceCycleRet(Placement const& placement, Paths const& paths, std::set<Node> const& unreachable, bool failed)
    : placement(placement)
    , paths(paths)
    , unreachable(unreachable)
    , failed(failed)
  {}
};

cppcoro::generator<PlaceCycleRet> place_cycle(Ops const& ops
  , Placement p
  , Paths paths
  , Range auto cycle
  , Range auto inter
  , MEdgeWeight m_edge_weight
  , Partition r
  , Sink sink)
{
  // Init logger
  err::Logger logger{sink};

  // Get lowest node id
  static i64 lowest{lowest_node_id(ops)};

  // Save old state of p
  Placement p_backup{p};

  // Log
  {
    [[maybe_unused]] auto fold{logger.fold()};

    logger.info()("Placement:");
    for (auto e : p)
    {
      logger.info()("{}", e);
    } // for
  }

  // Check if a tile is occupied
  auto f_is_free = [&](Tile const& t){ return ! fn(p).val().has(t); };

  // TODO: Replace this O(n^2) function
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
  if(unplaced.empty()){ co_yield PlaceCycleRet(p,paths,{},false); }

  // Wire tiles
  std::unordered_map<Node,std::unordered_map<Node,std::unordered_map<Node,Tile>>> wires_of;

  while( ! unplaced.empty() )
  {
    [[maybe_unused]] auto fold{logger.fold()};

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
      auto f_get_candidates = [&](Tile const& t, i64 dist) { return Adjacencies{t,dist}.tiles(); };

      // Get all candidate positions from previously positioned neighbors
      // Remove occupied positions
      tile_candidates = fn(neighbors_positioned)
        // .as([&](Node v){ return f_get_candidates(p[v],1); }) // nodes → tiles
        .as([&](Node v){ return f_get_candidates(p[v], m_edge_weight.at({u,v})); }) // nodes → tiles
        .squash() // merge [[tiles],[tiles]...] → [tiles]
        .drop([&](Tile t){ return ! f_is_free(t); })
        .sort()
        .unique()
        .vec();

      logger.info()("Initial candidate tiles to place {} at: {}", u, tile_candidates);

      // Remove tile_candidates that are behind positioned input nodes
      tile_candidates = fn(tile_candidates).keep([&](Tile const& t)
      {
        for(Node const& n : neighbors_positioned)
        {
          Tile const& tn{p[n]};
          if( fn(ops.preds(u)).has(n) )
          {
            if( tn.second < t.second ) { return false; }
          }  // if
          else
          {
            if( tn.second > t.second ) { return false; }
          } // else
        }
        return true;
      }).vec();

      logger.info()("Candidates that are not behind input nodes {}", tile_candidates);

      // For each tile in tile_candidates, remove all that does not adhere to edge
      // constraints.
      auto f_target = [&](Node u, Node v) { return m_edge_weight.at({u,v}); };
      auto f_dist = [&](Tile a, Tile b) { return ns_heuristics::chebyshev::run(a,b); };

      // Keep a candidate if it satisfies edges constraints to all its placed
      // neighbors
      tile_candidates = fn(tile_candidates)
        .in_all(neighbors_positioned
          , [&](Tile t, Node v) { return f_target(u,v) == f_dist(t,p.at(v)); })
        .vec();

      logger.info()("Candidates that respect edge constraints: {}", tile_candidates);

      // Filter candidates by A*
      tile_candidates = fn(tile_candidates)
        .keep([&](Tile t)
        {
          for (auto v : neighbors_positioned)
          {
            // Calculate chebyshev distance
            auto d_chebyshev{f_dist(t,p.at(v))};
            // Calculate chebyshev-based A* path
            auto d_astar{ns_search::a_star::run(t
              , p.at(v)
              , [&](Tile dest) -> Tiles { return f_get_candidates(dest,1); }
              , [&](Tile dest) -> bool { return fn(p).val().has(dest); }
              , [](Tile t1, Tile t2){ return ns_heuristics::chebyshev::run(t1,t2); }
            )};
            // Log
            if( d_astar )
            {
              logger.info()("Tile {} Node {} - Neighbor {} - A*: {}", t, u, v, *d_astar);
            } // if
            // Check if path exists, and it is eq to chebyshev
            if( ! d_astar || static_cast<size_t>(d_chebyshev+1) != d_astar->size() )
            {
              return false;
            } // if
          } // for

          return true;
        })
        .vec();

      logger.info()("Candidates that are reachable through A*: {}", tile_candidates);

      // Remove tile_candidates that are below the cut between the intersection with the parent cycle in
      // the bfs tree

      // Remove duplicate positions in tile_candidates
      tile_candidates = fn(tile_candidates).sort().unique().vec();

      logger.info()("Candidates without duplicates: {}", tile_candidates);

      // Sort tile_candidates by distance
      tile_candidates = fn(tile_candidates).sort({}, [&](Tile const& t)
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

      // Save unreachable nodes
      if ( tile_candidates.empty() )
      {
        // Save neighbors in case they are unreachable
        std::ranges::for_each(neighbors_positioned, [&](auto e){ unreachable.insert(e); });
      }

      logger.info()("Candidates sorted by distance: {}", tile_candidates);

    } // if
    else
    {
      tile_candidates = m_backtrack.at(u);

      // Save unreachable nodes
      if ( tile_candidates.empty() )
      {
        // Save neighbors in case they are unreachable
        std::ranges::for_each(neighbors_positioned, [&](auto e){ unreachable.insert(e); });
      }
    } // else

    // Log
    logger.info()("Final Candidates: {}", tile_candidates);

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
        co_yield PlaceCycleRet({},{},unreachable,true);
        break;
      } // if
      // Remove previous node from placed stack
      auto v{placed.top()}; placed.pop();
      // // Remove wires of the previous node from placed stack
      // for (auto&& [node_target,map_dummy] : wires_of[v])
      // {
      //   for (auto&& [node_dummy, tile] : map_dummy)
      //   {
      //     p.erase(node_dummy);
      //   } // for
      // } // for
      // wires_of[v].clear();
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
      // Tile candidates
      auto f_get_candidates = [&](Tile const& t, i64 dist) { return Adjacencies{t,dist}.tiles(); };
      // Distance
      auto f_dist = [&](Tile a, Tile b) { return ns_heuristics::chebyshev::run(a,b); };
      // Calculate chebyshev distance
      auto d_chebyshev{f_dist(chosen,p.at(v))};
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
        paths[{u,v}] = *d_astar;
      } // if
      else
      {
        paths[{v,u}] = fn(*d_astar).rev().deque();
      } // else

      d_astar->pop_front();
      d_astar->pop_back();

      if( fn(ops.succs(u)).has(v) )
      {
        for (Tile tile : fn(*d_astar).rev().deque())
        {
          wires_of[u][v][--lowest] = tile;
        } // for
      }
      else
      {
        for (Tile tile : *d_astar)
        {
          wires_of[u][v][--lowest] = tile;
        } // for
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

          logger.info()("u,v: {},{}\n", u, v);

          std::deque<Tile> node_dummy_path;

          if ( wires_of.contains(u) && wires_of.at(u).contains(v) )
          {
            out = fn(wires_of.at(u).at(v)).key().sort().vec();
            logger.info()("wires (sort) if: {}\n", out);
          } // if
          else if ( wires_of.contains(v) && wires_of.at(v).contains(u) )
          {
            out = fn(wires_of.at(v).at(u)).key().sort().vec();
            logger.info()("wires (sort) el: {}\n", out);
          }

          if ( fn(ops.preds(u)).has(v) ) { std::ranges::reverse(out); }
          logger.info()("wires (final) {}\n", out);

          out.insert(out.begin(), u);
          out.push_back(v);

          return fn(out).unique().vec();
        })
        .squash()
        .vec();

      logger.info()("Cycle: {}\n", cycle);
      logger.info()("Dummy Cycle: {}\n", cycle_with_dummy);

      co_yield PlaceCycleRet(p,paths,{},false);

      // Remove previous node from placed stack
      auto v{placed.top()}; placed.pop();

      // Remove wires
      for (auto&& [node_target,map_dummy] : wires_of[v])
      {
        for (auto&& [node_dummy,tile] : map_dummy)
        {
          p.erase(node_dummy);
        } // for
      } // for
      wires_of[v].clear();

      // Remove previous node from placement map
      if( p.contains(v) ){ p.erase(v); }

      // Include previous node in unplaced stack
      unplaced.push(v);
    } // if
  } // while

  // fmt::print("Reached partial solution\n");

} // function: place_cycle }}}

// fn: prox_view {{{
template<typename T>
decltype(auto) get_prox_view(Ops const& ops, T const& view)
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

} // fn: prox_view }}}

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
decltype(auto) global_backtracking(Ops const& ops, Basis& basis, C&& m_crossing_nodes, Sink sink)
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
    fmt::print("-- Base: {}\n", base);
  } // for

  basis = f_timer({}, [&] { return detect_ears(ops, basis); });

  for (auto&& base : basis)
  {
    fmt::print("-- Base (ear): {}\n", base);
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

  // Calculate graph depth-view
  auto depth_view =
    f_timer({}, [&] { return ns_views::depth::run(0,ops.preds,ops.succs).nl; });

  auto prox_view =
    f_timer({}, [&] { return get_prox_view(ops,depth_view); });

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

  // Log
  for (auto e : m_edge_weight)
  {
    logger.info()("Weight: {}", e);
  } // for


  // for (auto e : prox_view)
  // {
  //   fmt::print("prox e: {}\n", e);
  // } // for

  // Rotate while intersection has size != 2
  // pair_intersection_basis = fn(pair_intersection_basis).rot([](auto&& e){ return e.front().first.size() != 2; }).vec();

  // Reverse the pairs before processing, I don't remember why
  f_timer({}, [&] { std::reverse(pair_intersection_basis.begin(), pair_intersection_basis.end()); });

  // Get initial intersection
  auto const& [intersection,cycle] = pair_intersection_basis.back();

  // Place initial intersection
  placement =
    f_timer({}, [&] { return place_intersection(ops, intersection); });

  // Stack generators
  std::stack<cppcoro::generator<PlaceCycleRet>> st_generator;

  // Stack of processed solutions
  std::stack<std::pair<Nodes,Nodes>> st_solutions;

  // Push intersection
  if(auto [u,v] = std::make_pair(intersection.at(0),intersection.at(1)); fn(ops.succs(u)).has(v) )
  {
    paths[{u,v}] = std::deque<Tile>({placement.at(intersection.at(0)), placement.at(intersection.at(1))});
  } // if
  else
  {
    paths[{v,u}] = std::deque<Tile>({placement.at(intersection.at(1)), placement.at(intersection.at(0))});
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
            , cycle
            , intersection
            , m_edge_weight
            , Partition::L
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
            std::ranges::for_each(intersect, [&](auto e){ nodes_unreachable.erase(e); });
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
      while ( it_gen != generator.end() && it_gen->failed == false )
      {
        placement = it_gen->placement;
        paths = it_gen->paths;

        // Check for half separation
        bool has_inner_crossings = cycle_has_inner_crossings(ops, cycle, placement, sink);

        if ( ! placement.empty() )
        {
          Placement _placement {placement};
          Paths _paths {paths};

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
            ns_draw::svg::svg(ops
              , fmt::format("out/step-{}.svg", ++i)
              , _placement
              , _paths
              , [](auto e){ return e; });
            logger.info()("-- End draw");
          } // if has_inner_crossings
#endif
        }

        if ( has_inner_crossings )
        {
          fmt::print("Cycle {} has inner crossings\n", i);
          it_gen = std::next(it_gen);
        }
        else
        {
          fmt::print("Cycle {} has no inner crossings\n", i);
          break;
        } // else
      }
    };

    check_half_separations();

    if( it_gen != generator.end() && it_gen->failed == false )
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
      nodes_unreachable = (it_gen != generator.end())? it_gen->unreachable : std::set<Node>{};
      logger.info()("-- Failed for cycle: {}", cycle);
    } // else

  } // while


  // bool result{place(ops,placement,depth_view,i,p,Partition::L)};
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

  err::Logger logger;


  fmt::print("Gates: {}\n", g.vertices_count());
  fmt::print("Wires: {}\n", g.edges_count());

  // Create ops
  Ops ops(f_p, f_s, f_a, f_l, f_u);

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

  // ops.link(8, -7);


  f_timer({}, f_write_v, metadata.data(), f_p, f_s, "out/3-out.v");

  f_timer({}, f_write_d, g.data(), f_p, f_s, "out/3-out.dimacs");

  std::vector<std::vector<std::vector<Node>>> e_basis;

  // B1
  std::vector<std::vector<std::vector<Node>>> e_basis_b1 =
  {
    { {26,31}, {26,23}, {23,27}, {31,27}, }, 
    { {26,23}, {20,23}, {22,20}, {22,26}, }, 
    { {22,20}, {13,20}, {18,13}, {18,22}, }, 
    { {19,20}, {13,19}, {13,20}, }, 
    { {26,31}, {30,36}, {30,29}, {29,26}, {36,31}, }, 
    { {45,42}, {42,46}, {45,46}, }, 
    { {41,45}, {40,41}, {40,44}, {45,44}, }, 
    { {44,45}, {47,43}, {47,46}, {46,45}, {43,44}, }, 
    { {40,36}, {30,36}, {35,30}, {35,40}, }, 
    { {44,40}, {35,40}, {39,35}, {39,44}, }, 
    { {39,44}, {43,39}, {43,44}, }, 
    { {12,17}, {17,16}, {12,16}, }, 
    { {12,14}, {14,11}, {11,9}, {12,9}, }, 
    { {11,14}, {11,15}, {15,18}, {14,18}, }, 
    { {9,11}, {7,11}, {8,7}, {8,9}, }, 
    { {18,22}, {12,17}, {12,14}, {14,18}, {17,22}, }, 
    { {8,7}, {4,8}, {3,7}, {1,3}, {1,4}, }, 
    { {27,32}, {33,27}, {33,38}, {38,32}, }, 
    { {35,39}, {34,39}, {34,35}, }, 
    { {29,30}, {25,29}, {25,30}, }, 
    { {18,13}, {11,14}, {7,10}, {7,11}, {10,13}, {14,18}, }, 
    { {8,9}, {6,9}, {5,8}, {2,5}, {2,6}, }, 
    { {22,26}, {21,16}, {21,25}, {25,29}, {16,17}, {17,22}, {29,26}, }, 
    { {5,8}, {4,8}, {2,5}, {1,4}, {0,1}, {0,2}, }, 
    { {35,30}, {28,34}, {21,25}, {34,35}, {25,30}, {24,21}, {24,28}, }, 
    { {27,32}, {40,41}, {40,36}, {36,31}, {31,27}, {41,37}, {37,32}, }, 
    { {45,42}, {45,41}, {41,37}, {42,37}, },
  };

  std::vector<std::vector<std::vector<Node>>> e_basis_cm138a =
  {
    { {77,78}, {70,78}, {69,70}, {69,77}, } ,
    { {70,71}, {63,71}, {62,63}, {62,70}, } ,
    { {69,70}, {62,70}, {60,62}, {60,69}, } ,
    { {63,61}, {53,61}, {55,53}, {55,63}, } ,
    { {62,63}, {55,63}, {54,55}, {54,62}, } ,
    { {60,62}, {54,62}, {51,54}, {51,60}, } ,
    { {53,52}, {44,52}, {45,44}, {45,53}, } ,
    { {55,53}, {45,53}, {47,45}, {47,55}, } ,
    { {54,55}, {47,55}, {46,47}, {46,54}, } ,
    { {51,54}, {46,54}, {43,46}, {43,51}, } ,
    { {44,37}, {30,37}, {36,30}, {36,44}, } ,
    { {45,44}, {36,44}, {38,36}, {38,45}, } ,
    { {47,45}, {38,45}, {40,38}, {40,47}, } ,
    { {46,47}, {40,47}, {39,40}, {39,46}, } ,
    { {43,46}, {39,46}, {35,39}, {35,43}, } ,
    { {20,12}, {20,13}, {12,8}, {8,13}, } ,
    { {21,20}, {21,13}, {20,13}, } ,
    { {36,30}, {21,30}, {29,21}, {29,36}, } ,
    { {31,29}, {31,23}, {29,23}, } ,
    { {40,38}, {31,38}, {33,31}, {33,40}, } ,
    { {32,33}, {32,26}, {33,26}, } ,
    { {35,39}, {32,39}, {27,32}, {27,35}, } ,
    { {39,40}, {33,40}, {32,33}, {32,39}, } ,
    { {38,36}, {29,36}, {31,29}, {31,38}, } ,
    { {22,23}, {23,15}, {15,14}, {22,14}, } ,
    { {30,28}, {20,28}, {21,20}, {21,30}, } ,
    { {21,29}, {23,29}, {13,21}, {15,13}, {15,23}, } ,
    { {14,15}, {9,14}, {9,15}, } ,
    { {86,81}, {74,81}, {80,74}, {80,86}, } ,
    { {74,65}, {56,65}, {64,56}, {64,74}, } ,
    { {72,64}, {72,73}, {64,73}, } ,
    { {69,60}, {76,69}, {82,76}, {60,68}, {75,68}, {81,75}, {82,81}, } ,
    { {51,43}, {59,51}, {67,59}, {43,50}, {58,50}, {65,58}, {67,65}, } ,
    { {35,27}, {42,35}, {49,42}, {27,34}, {41,34}, {48,41}, {49,48}, } ,
    { {77,84}, {84,88}, {91,88}, {83,77}, {83,87}, {87,90}, {90,91}, } ,
    { {69,77}, {86,90}, {86,81}, {76,69}, {82,76}, {83,77}, {87,83}, {81,82}, {90,87}, } ,
    { {51,60}, {74,81}, {74,65}, {59,51}, {67,59}, {68,60}, {75,68}, {65,67}, {81,75}, } ,
    { {35,43}, {56,65}, {56,48}, {42,35}, {49,42}, {50,43}, {58,50}, {48,49}, {65,58}, } ,
    { {80,74}, {64,74}, {72,64}, {72,80}, } ,
    { {48,56}, {57,48}, {57,56}, } ,
    { {86,90}, {89,90}, {89,86}, } ,
    { {72,80}, {79,72}, {79,80}, } ,
    { {27,32}, {26,32}, {19,26}, {19,27}, } ,
    { {14,22}, {18,14}, {18,22}, } ,
    { {22,26}, {19,26}, {18,22}, {11,18}, {11,19}, } ,
    { {33,31}, {26,33}, {23,31}, {22,23}, {22,26}, } ,
    { {15,13}, {9,15}, {8,13}, {5,8}, {5,9}, } ,
    { {5,6}, {3,5}, {3,6}, } ,
    { {6,11}, {17,11}, {25,17}, {10,6}, {16,10}, {24,16}, {24,25}, } ,
    { {6,11}, {6,5}, {18,14}, {9,14}, {5,9}, {11,18}, } ,
    { {80,86}, {89,86}, {79,80}, {85,79}, {85,89}, } ,
    { {56,64}, {73,64}, {57,56}, {66,57}, {66,73}, },
  };

  e_basis = e_basis_cm138a;

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
    .as([&](auto&& e){ return fn(e).as([&](auto e) { return m_nodes_norm.at(e); }).vec(); })
    .vec();

  // Align cycles from upper node to bottom node
  auto f_align_cycle = [&](auto const& cycle)
  {
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

  fn(basis).ply([](auto&& e){ fmt::print("Basis: {}\n", e); });
  
  // celaeno::graph::operations::balance::paths::run(0,ops);
  //
  // f_timer({}, f_write_v, metadata.data(), f_p, f_s, "out/4-out.v");
  //
  // // Get minimal basis
  // auto basis = f_timer({}, [&] { return minimal_basis(i64{},ops,m_crossing_nodes,logger.sink()); });
  // basis = f_timer({}, [&] { return detect_ears(ops, basis); });
  //
  // // Include additional edges for cycles with size > 4
  // for(auto const& base : basis)
  // {
  //   if (base.size() > 4)
  //   {
  //     auto c1 = fn(base)
  //       .take_while(
  //         [&](auto n){ return fn(ops.succs(n)).in(base).vec().size() < 2; })
  //       .rev()
  //       .vec();
  //
  //     auto c2 = fn(base).dif(c1).vec();
  //
  //     c1.pop_back();
  //     c2.erase(c2.begin());
  //
  //     fmt::print("Base: {}\n", base);
  //     fmt::print("c1: {}\n", c1);
  //     fmt::print("c2: {}\n", c2);
  //
  //     // Insert additional edges
  //     // // Check which yields most cuts (c1 → c2 or c2 → c1)
  //     auto f_degree_out = [&](Node n) { return ops.succs(n).size(); };
  //     auto f_degree_in = [&](Node n) { return ops.preds(n).size(); };
  //     auto f_degree = [&](Node n) { return f_degree_out(n) + f_degree_in(n); };
  //     auto cuts_c1 = fn(c1)
  //       .zip(fn(c2).pop_front(1).vec())
  //       .keep([&](auto e){ return f_degree_out(e.first) < 2 && f_degree_in(e.second) < 2; })
  //       .vec();
  //     auto cuts_c2 = fn(c2)
  //       .zip(fn(c1).pop_front(1).vec())
  //       .keep([&](auto e){ return f_degree_out(e.first) < 2 && f_degree_in(e.second) < 2; })
  //       .vec();
  //     fmt::print("c1|c2: {}\n", cuts_c1);
  //     fmt::print("c1|c2: {}\n", cuts_c2);
  //
  //     if ( cuts_c1.empty() && cuts_c2.empty() ) { continue; }
  //
  //     auto cuts = ( cuts_c1.size() > cuts_c2.size() )? cuts_c1 : cuts_c2;
  //
  //     fn(cuts).ply([&](auto c){ ops.link(c.first, c.second); });
  //
  //     // :
  //   } // if base.size() > 4
  // }
  //
  // // Process into adjacent intersection
  // for (i32 i{}; auto const& base : basis)
  // {
  //   logger.info()("-- Base {}: {}", i++, base);
  // } // for
  //
  // f_timer({}, f_write_v, metadata.data(), f_p, f_s, "out/5-out.v");

  // exit(0);


  // unbalance(0,ops);

  // ns_io_verilog::Writer(metadata.data(), f_p, f_s, "out/3-out.v");

  // celaeno::graph::operations::balance::paths::run(0,ops);
  //
  // ns_io_verilog::Writer(metadata.data(), f_p, f_s, "4-out.v");
  //
  // auto layers{ns_ops::minimize::crossings::run(0, ops.preds, ops.succs, ops.adj, ops.link, ops.unlink)};

  //
  // ns_io_verilog::Writer(metadata.data(), f_p, f_s, "4-out.v");

  // fmt::print("Graph:\n");
  // for (auto e : g.data())
  // {
  //   fmt::print("{}\n", e);
  // } // for

  // Perform placement
  // std::cerr << "Started computation\n";
  auto start {std::chrono::system_clock::now()};
  auto [placement, routing] {global_backtracking(ops,basis,m_crossing_nodes,logger.sink())};
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
  ns_draw::svg::svg(ops
    , "out/out.svg"
    , placement
    , routing
    , [](auto i){ return i; }
  );


  return EXIT_SUCCESS;
} // main }}}
