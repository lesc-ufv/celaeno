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

// TODO Remove
#include <celaeno/graph/operations/balance/outgoing.hpp>
#include <celaeno/graph/operations/balance/incoming.hpp>
#include <celaeno/graph/operations/count/crossings.hpp>

#include <pybind11/embed.h>
#include <pybind11/stl.h>

#include <boost/geometry.hpp>
#include <boost/geometry/geometries/point_xy.hpp>
#include <boost/geometry/geometries/polygon.hpp>
#include <boost/geometry/geometries/geometries.hpp>
#include <boost/geometry/geometries/segment.hpp>


#define L01(cap,ret,expr) [cap]                                        ret { expr; }
#define L02(cap,ret,expr) [cap]                                        ret { expr; }
#define L03(cap,ret,expr) [cap]                                        ret { expr; }
#define L11(cap,ret,expr) [cap](auto&& _1)                             ret { expr; }
#define L12(cap,ret,expr) [cap](auto&& __1)                            ret { expr; }
#define L13(cap,ret,expr) [cap](auto&& ___1)                           ret { expr; }
#define L21(cap,ret,expr) [cap](auto&& _1, auto&& _2)                  ret { expr; }
#define L22(cap,ret,expr) [cap](auto&& __1, auto&& __2)                ret { expr; }
#define L23(cap,ret,expr) [cap](auto&& ___1, auto&& ___2)              ret { expr; }
#define L31(cap,ret,expr) [cap](auto&& _1, auto&& _2, auto&& _3)       ret { expr; }
#define L32(cap,ret,expr) [cap](auto&& __1, auto&& __2, auto&& __3)    ret { expr; }
#define L33(cap,ret,expr) [cap](auto&& ___1, auto&& ___2, auto&& ___3) ret { expr; }

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
decltype(auto) view_draw(Map auto&& map_layer_vertices, Map auto&& map_edge, std::string str_file_name)
{
  // , Map auto&& map_vertex_tile, Range auto&& range_edge_path)
  // Get max layer id
  constexpr u64 const size_spacing = 4;

  // Find out the size of the largest layer
  u64 const size_max_layer = fn(map_layer_vertices).as(L11(,,return _1.second.size())).max();
  fmt::print("Max layer size: {}\n", size_max_layer);

  // Shift layers left by (size_max_layer-size_cur_layer)/2
  std::map<i64, i64> map_layer_shift = fn(map_layer_vertices)
    .as([&](auto&& e){ return std::make_pair(e.first, static_cast<i64>(size_max_layer / e.second.size())); })
    // .as([&](auto&& e){ if (e.second == 0) { e.second = 1; } return e; })
    .map();

  // Convert layers into collections of xy positions
  std::map<Node, Tile> map_vertex_tile = fn(map_layer_vertices)
  .as([&](auto&& _1)
  {
    i64 y_pos = _1.first;
    i64 x_pos = -1;
    return fn(_1.second)
      .as([&](auto __1)
      {
        i64 __1_tile_y = y_pos;
        i64 __1_tile_x = (++x_pos*map_layer_shift.at(y_pos)+std::floor(map_layer_shift.at(y_pos)/2.0));
        __1_tile_y *= 4;
        return std::make_pair(__1, Tile(__1_tile_x, __1_tile_y));
      })
      .vec();
  })
  .squash()
  .map();

  std::vector<std::pair<Edge,Tiles>> path;
  fn(map_edge).ply([&](auto&& _1)
  {
    path.push_back(
      std::make_pair(Edge{_1}
        , Tiles{map_vertex_tile.at(_1.first)
        , map_vertex_tile.at(_1.second)
        }
      )
    );
  });

  ns_draw::svg::svg(str_file_name, map_vertex_tile, path, [](auto e){ return e; });

} // function: view_draw }}}

// fn: result_draw {{{
decltype(auto) result_draw(Placement _placement
  , Paths _paths
  , std::multimap<Node,Node> _node_map
  , i64 i
  , std::string file_name_out
  , Sink sink)
{
  err::Logger logger{sink};

  // Offset coordinates to remove negative values
  auto x_min{rg::min_element(_placement,{},[](auto e){ return e.second.first; })->second.first};
  auto y_min{rg::min_element(_placement,{},[](auto e){ return e.second.second; })->second.second};

  for (auto& [n,p] : _placement)
  {
    auto& [x,y] = p;

    x += std::abs(x_min);
    y += std::abs(y_min);
  } // for

  // // Expand grid to solve crossings
  // for (auto& [n,p] : _placement)
  // {
  //   auto& [x,y] = p;
  //
  //   x *= 2;
  //   y *= 2;
  // } // for

  for(auto& [pair,path] : _paths)
  {
    for(Tile& tile : path)
    {
      tile.first += std::abs(x_min);
      tile.second += std::abs(y_min);
      // tile.first *= 2;
      // tile.second *= 2;
    }
  }

  // Draw
  if ( file_name_out.empty() ) { file_name_out = fmt::format("out/step-{}.svg", i); }
  logger.info()("-- Figure {}", i);
  logger.info()("-- placement: ");
  for (auto&& e : _placement)
  {
    logger.info()("-- e: {}", e);
  } // for
  logger.info()("-- Start draw");
  ns_draw::svg::svg(file_name_out , _placement , _paths , L11(,,return _1));
  logger.info()("-- End draw");
  logger.info()("-- Placed edges:");
  std::ranges::for_each(_node_map, [&](auto e){ logger.info()("e: {}", e); });
} // function: result_draw }}}

// fn: get_cycle_basis {{{
decltype(auto) get_cycle_basis(std::string str_file_dimacs)
{
  auto str_bin_dimacs = "./parmcb/build/mcb-dimacs-mpi";
  auto str_file_out = "./out/cycles.txt";
  auto cmd = fmt::format("{} -i {} --printcycles &> {}", str_bin_dimacs, str_file_dimacs, str_file_out);
  system(cmd.c_str());

  std::ifstream file_cycles{str_file_out};

  err::err({ file_cycles.good() })("Could not open cycles file");

  Basis out;
  std::regex re("\\((\\d+),(\\d+)\\)");
  std::smatch match;

  for( std::string str_cycles; std::getline(file_cycles, str_cycles); )
  {
    Base base;
    for(auto searchStart(str_cycles.cbegin()); std::regex_search(searchStart, str_cycles.cend(), match, re); )
    {
        base.emplace_back(std::stoi(match[1]));
        base.emplace_back(std::stoi(match[2]));
        searchStart = match.suffix().first;
    }
    if ( ! base.empty() )
    {
      out.push_back(fn(base).sort().unique().vec());
    }
  }
  return out;
} // function: get_cycle_basis }}}

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

// fn: is_tile_inside_cycle {{{
decltype(auto) is_tile_inside_cycle(Tile t, Tiles const& tiles)
{
  // Alias boost types
  using point_type = boost::geometry::model::d2::point_xy<i64>;
  using polygon_type = boost::geometry::model::polygon<point_type>;
  /// Create and correct polygon
  polygon_type poly;
  std::ranges::for_each(tiles,[&](auto&& _1){ boost::geometry::append(poly, point_type(_1.first, _1.second)); });
  boost::geometry::correct(poly);
  // Calculate if t is inside tiles
  point_type p(t.first, t.second);
  return boost::geometry::within(p, poly);
} // function: is_tile_inside_cycle }}}

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
  //     if ( auto opt_nodes = path_from_multimap(multimap_node_successor, u, v, remove_by) )
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

// fn: lines_intersect {{{
decltype(auto) lines_intersect(Tile p1, Tile p2, Tile p3, Tile p4)
{
  // Create the line segments from the input points
  typedef boost::geometry::model::d2::point_xy<int> point;
  typedef boost::geometry::model::segment<point> segment;
  segment s1(point(p1.first, p1.second), point(p2.first, p2.second));
  segment s2(point(p3.first, p3.second), point(p4.first, p4.second));

  if (boost::geometry::intersects(s1, s2))
  {
      // The segments intersect. Check if they just share a common endpoint.
      if (boost::geometry::equals(s1.first, s2.first) ||
          boost::geometry::equals(s1.first, s2.second) ||
          boost::geometry::equals(s1.second, s2.first) ||
          boost::geometry::equals(s1.second, s2.second))
      {
          // The segments share a common endpoint. This is not considered a crossing.
          return false;
      }

      // The segments genuinely cross each other.
      return true;
  }

  // The segments do not intersect.
  return false;
} // function: lines_intersect }}}

// fn: cycles_intersect {{{
bool cycles_intersect(Map auto&& placement, Nodes c1, Nodes c2)
{
  c1.push_back(c1.front());
  c2.push_back(c2.front());

  auto segments_c1 = fn(c1).as(L11(&,,return placement.at(_1))).slide(2).vec();
  auto segments_c2 = fn(c2).as(L11(&,,return placement.at(_1))).slide(2).vec();

  auto f_lines_intersect = L21(&,,return lines_intersect(_1.at(0), _1.at(1), _2.at(0), _2.at(1)));
  for (auto&& segment_c1 : segments_c1)
  {
    if ( fn(segments_c2).any([&](auto&& _1){ return f_lines_intersect(segment_c1, _1); }) )
    {
      return true;
    }
  } // for

  return false;
} // function: cycles_intersect }}}

// fn: cycle_has_inner_crossings {{{
//
// This function checks if a cycle crosses itself
//
bool cycle_has_inner_crossings(Nodes cycle , Placement const& m_node_pos , Sink sink)
{
  err::err({ cycle.front() == cycle.back() })("Requires node on front to equal the one on back");

  err::Logger logger{sink};

  namespace bg = boost::geometry;
  using point = bg::model::point<i64, 2, bg::cs::cartesian>;
  using polygon = bg::model::polygon<point>;

  auto cycle_edges = fn(cycle)
    .as([&](Node e){ return m_node_pos.at(e); })
    .as([](Tile e){ return Tile{e.first,e.second}; })
    .vec();

  fmt::print("Edges: {}\n", cycle_edges);

  polygon poly;
  std::ranges::for_each(cycle_edges, [&](auto&& _1){ bg::append(poly, point(_1.first, _1.second)); });
  bg::correct(poly);

  if ( ! bg::is_valid(poly) ) { return true; }

  err::err({ bg::is_valid(poly) })("Invalid polygon");

  return ! bg::is_simple(poly);
} // function: cycle_has_inner_crossings }}}

// fn: cycle_has_inner_crossings_2 {{{
//
// This function checks if a positioned cycle halves cross between themselves
//
decltype(auto) cycle_has_inner_crossings_2(Nodes cycle
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
} // function: cycle_has_inner_crossings_2 }}}

// fn: minimal_basis_bfs_ordering {{{
Basis minimal_basis_bfs_ordering(Ops const& ops, Basis basis, MEdgeWeight const& m_edge_weight)
{
  Basis out;

  // std::random_device rd;
  // std::mt19937 g(rd());
  // std::shuffle(basis.begin(), basis.end(), g);

  // // Try to start from smaller basis
  // std::ranges::sort(out, {}, [](auto&& _1){ return _1.size(); });

  // Find a base that has an intersection of two nodes, with an edge weight of 1
  for (auto it_curr{basis.begin()}; it_curr != basis.end(); ++it_curr)
  {
    bool stop{false};
    for (auto it_comp{basis.begin()}; it_comp != basis.end(); ++it_comp)
    {
      if ( *it_curr == *it_comp ) { continue; }
      Nodes intersection = fn(*it_curr).in(*it_comp).vec();
      if ( intersection.size() == 2 && m_edge_weight.at(Edge{intersection.at(0), intersection.at(1)}) == 1)
      {
        out.push_back(*it_curr);
        out.push_back(*it_comp);
        basis = fn(basis).keep([&](auto&& _1){ return _1 != *it_curr && _1 != *it_comp; }).vec();
        stop = true;
        break;
      }
    } // for
    if ( stop ) { break; }
  } // for

  std::ranges::copy(basis, std::back_inserter(out));

  // // Fetch inputs
  // Nodes inputs;
  // ns_search::bfs::run(0, ops, [&](auto e)
  // {
  //   if ( ops.preds(e).size() == 0 ) { inputs.push_back(e); };
  //   return false;
  // });
  //
  // // Make the first elements contain the inputs
  // std::ranges::partition(basis, [&](auto const& e){ return fn(e).in(inputs).vec().size() != 0; });

  // Take first base, use it as a starting point
  Basis::iterator cut{out.begin()};

  // Partition the vector into adjacencies of cur
  for(auto it{out.begin()}; it != out.end(); ++it)
  {
    cut = std::partition(cut
      , out.end()
      , [&](Base const& b) { return fn(b).in(*it).vec().size() >= 2; });
  }

  return out;
} // function: minimal_basis_bfs_ordering }}}

// fn: cycle_align_intersection {{{
//
// Align an intersection between two cycles
//
template<Range R1, Range R2, Range R3>
decltype(auto) cycle_align_intersection(R1 r1, R2 r2, R3 r3, Sink sink)
{
  err::Logger logger{sink};

  // err::err({ ! r3.empty() })("Intersection must not be empty");
  err::err({ ! r1.empty() && ! r2.empty() })("r1 or r2 are empty");

  if ( r3.empty() ) { return std::make_tuple(r1,r2,r3); }

  logger.info()("r1 (pre): {}", r1);
  logger.info()("r2 (pre): {}", r2);
  logger.info()("r3 (pre): {}", r3);

  // Rotate a cycle until intersection is at the start
  auto f_rotate_intersection_to_start = [](auto&& _1, auto&& _2)
  {
    return fn(_1).rot([&](auto __1)
    {
      return fn(__1).cut(u64{}, _2.size()).in(_2).vec().size() == _2.size();
    }).vec();
  };

  // Remove elements that are not part of the intersection
  auto f_rotate_and_remove_in_between = [](auto&& _1, auto&& _2)
  {
    // Move fst element to beginning
    Nodes _1_out_norm = fn(_1).rot([&](auto __1)
    {
      return __1.front() == _2.front();
    }).vec();

    // Do the same but reverse
    Nodes _1_out_rev = fn(_1).rev().rot([&](auto __1)
    {
      return __1.front() == _2.front();
    }).vec();

    // Search the one with a smaller distance to _2.back()
    auto _1_out_norm_it = std::ranges::find(_1_out_norm, _2.back());
    auto _1_out_rev_it  = std::ranges::find(_1_out_rev , _2.back());

    err::err({ _1_out_norm_it != std::ranges::end(_1_out_norm) })("Intersection element not found");
    err::err({ _1_out_rev_it  != std::ranges::end(_1_out_rev) })("Intersection element not found");

    // Calculate distances
    u64 _1_dist_to_last_norm = std::distance(_1_out_norm.begin(), _1_out_norm_it);
    u64 _1_dist_to_last_rev  = std::distance(_1_out_rev.begin(), _1_out_rev_it);

    // Select smallest
    if ( _1_dist_to_last_norm < _1_dist_to_last_rev )
    {
      // Remove intersection with elements in-between
      _1_out_norm.erase(_1_out_norm.begin(), std::next(_1_out_norm_it));
      // Return intersection without elements in-between
      return fn(_2).chain(_1_out_norm).vec();
    } // if
    else
    {
      // Remove intersection with elements in-between
      _1_out_rev.erase(_1_out_rev.begin(), std::next(_1_out_rev_it));
      // Return intersection without elements in-between
      return fn(_2).chain(_1_out_rev).vec();
    } // else

  };

  // Align r1 with intersection
  // r1 defines the order of the intersection
  // r1 expects the intersection to have no other element in-between
  // If there is, infinite loop
  r1 = f_rotate_intersection_to_start(r1, r3);

  // Set r3 to the order in which it appears on r1
  r3 = fn(r1).cut(u64{}, r3.size()).vec();

  // Align r2 with intersection
  // Rotates intersection to the beginning
  // Removes elements in-between the intersection that are not a part it
  r2 = f_rotate_and_remove_in_between(r2, r3);

  logger.info()("r1 (pos): {}", r1);
  logger.info()("r2 (pos): {}", r2);
  logger.info()("r3 (pos): {}", r3);

  return std::make_tuple(r1,r2,r3);
} // function: cycle_align_intersection }}}

// fn: cycle_align_intersections {{{
template<Range R>
decltype(auto) cycle_align_intersections(R minimal_basis, Sink sink)
{
  // Init logger
  err::Logger logger{sink};

  err::err({ ! minimal_basis.empty() })("Minimal basis must not be empty");

  err::err({ minimal_basis.size() > 1 })("Minimal basis must contain at least two cycles");

  PairsInterCycles out;

  std::queue<Nodes> q, q_restore;

  auto c1{minimal_basis.front()};

  logger.info()("c1: {}", c1);

  i64 i{};

  for (auto it{minimal_basis.begin()}; it != std::prev(minimal_basis.end()); ++it)
  {
    auto c2{*std::next(it)};

    auto intersection{fn(c1).in(c2).vec()};

    std::tie(c1,c2,intersection) = cycle_align_intersection(c1,c2,fn(c1).in(c2).vec(),sink);

    // Log data
    // Handle first element
    if( i == 0 )
    {
      err::err({ ! intersection.empty() })("Intersection between first two elements must not be empty");

      // err::err({ intersection.size() == 2 })("Intersection must have size of 2");

      // Remove last element of cycle, if it equas the first
      if( c1.front() == c1.back() ){ c1.pop_back(); }

      // Rotate elements until first two are the intersection
      std::tie(c1,c2,intersection) = cycle_align_intersection(c1,c2,fn(c1).in(c2).vec(),sink);

      logger.info()("Rotated c1: {}", c1);

      intersection = fn(intersection).cut(0,2).vec();
      out.emplace_back(intersection,c1);
    } // if

    logger.info()("c2: {}", c2);

    while( intersection.empty() or intersection.size() < 2 )
    {
      // Check if there are more cycles to intersect with
      if( q.empty() ){ err::err()("No intersection for incident cycle {}", c1); }

      // Update c1
      c1 = q.front(); q.pop();

      q_restore.push(c1);

      logger.info()("Trying new c1: {}", c1);

      // Try to form intersection
      std::tie(c1,c2,intersection) = cycle_align_intersection(c1,c2,fn(c1).in(c2).vec(),sink);

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

    std::tie(c1,c2,intersection) = cycle_align_intersection(c1,c2,fn(c1).in(c2).vec(),sink);

    logger.info()("Final c2: {}", c2);

    // Only allow intersection of size 2
    intersection = fn(intersection).cut(0,2).vec();
    out.emplace_back(intersection,c2);

    ++i;
  } // for

  logger.info()("Finished cycle_align_intersections");

  return out;
} // fn: cycle_align_intersections }}}

// fn: cycle_merge_on_intersection {{{
//
// Merge two cycles with a common intersection, into one large cycle
//
template<Range R1, Range R2, Range R3>
Nodes cycle_merge_on_intersection(Ops const& ops, R1 r1, R2 r2, R3 r3, Sink sink)
{
  // Both cannot be empty
  err::err({ ! r1.empty() or ! r2.empty() })("Empty intersection for merge cycle");

  // Intersection must not be empty
  err::err({ ! r3.empty() })("Empty intersection for merge cycle");

  // If r2 == r3, then r2 is already in r1
  if ( r2 == r3 ) { return r1; }

  // If either are empty, return the other
  if ( r1.empty() && ! r2.empty() ) { return r2; }
  if ( r2.empty() && ! r1.empty() ) { return r1; }

  // Align intersection to be at the start, and on the same order, in both cycles
  std::tie(r2,r1,r3) = cycle_align_intersection(r2, r1, r3, sink);

  // Remove intersection from r1
  r1.erase(r1.begin(), std::next(r1.begin(), r3.size()));

  // Check for undetected edges
  auto f_neigh = [&](auto&& _1){ return fn(ops.preds(_1)).chain(ops.succs(_1)).vec(); };

  // r3 = fn(r1)
  //   // Remove intersection
  //   .dif(r3)
  //   // Transform into neighbors of nodes
  //   .as([&](auto&& _1){ return f_neigh(_1); })
  //   // Squash vectors in vector
  //   .squash().sort().unique()
  //   // Intersection with outer cycle uniquely
  //   .in(r2).sort().unique()
  //   // Include original intersection nodes uniquely
  //   .chain(r3).sort().unique()
  //   // Rotate outer cycle until it matches the whole intersection
  //   .mut([&](auto&& _1)
  //   {
  //     // Rotate until nodes in r2 slice are all in _1
  //     return fn(r2).rot([&](auto&& __1)
  //     {
  //       return fn(__1).cut(u64{},_1.size()).in(_1).vec().size() == _1.size();
  //     })
  //     // Cut for the size of the intersection
  //     .mut([&](auto&& __1){ return Nodes{__1.begin(), std::next(__1.begin(), _1.size())}; })
  //     .vec();
  //   })
  //   .vec();

  // Erase intersection from r2
  r2.erase(r2.begin(), std::next(r2.begin(), r3.size()));

  // Reduce intersection to first and last elements
  r3 = fn(r3).mut([](auto&& _1){ return Nodes{_1.front(), _1.back()}; }).vec();

  // Introduce first an last elements
  Nodes merge = fn(r1).push_front(r3.back()).push_back(r3.front()).rev().chain(r2).vec();

  // Check for sequentiality
  auto f_seq = [&](auto&& _1, auto&& _2){ return fn(ops.succs(_1)).has(_2) or fn(ops.preds(_1)).has(_2); };
  // err::err({ fn(merge)
  //   .push_back(merge.at(0))
  //   .slide(2)
  //   .all([&](auto&& _1){ return f_seq(_1.at(0),_1.at(1)); })
  // })( "Non sequential result in merge cycles" );
  if ( ! fn(merge)
    .push_back(merge.at(0))
    .slide(2)
    .all([&](auto&& _1){ return f_seq(_1.at(0),_1.at(1)); }))
  {
    err::err()( "Non sequential result in merge cycles" );
  }

  return merge;
} // function: cycle_merge_on_intersection }}}

// fn: get_cycle_supports {{{
decltype(auto) get_cycle_supports(Ops const& ops, Range auto&& cycle, Map auto&& view)
{
  Edges out;

  auto f_make_cuts = [&](Range auto&& _1, Range auto&& _2) -> Edges
  {
    Edges _1_out;

    // For each node, try to connect to another node of _2
    for (auto it_c1{_1.begin()}; it_c1 != _1.end(); ++it_c1)
    {
      auto it_c2 = std::ranges::find_if(_2, [&](auto&& __1){ return view.at(__1) > view.at(*it_c1); });
      if ( it_c2 == std::ranges::end(_2) ) { break; } // if
      _1_out.push_back(Edge{*it_c1, *it_c2});
    } // for

    return _1_out;
  };

  // Create additional edges for cycles with size > 4
  if (cycle.size() > 4)
  {
    // Split in half cycles
    auto [c1,c2] = cycle_split_in_endpoints(ops, cycle);

    fmt::print("c1 (pre): {}\n", c1);
    fmt::print("c2 (pre): {}\n", c2);

    std::ranges::reverse(c2);
    c1.pop_back();
    c2.pop_back();
    c1.erase(c1.begin());
    c2.erase(c2.begin());

    if ( c1.empty() or c2.empty() ) { return Edges{}; }

    fmt::print("cycle: {}\n", cycle);
    fmt::print("c1: {}\n", c1);
    fmt::print("c2: {}\n", c2);

    // Insert additional edges
    // auto f_degree_out = [&](Node n) { return ops.succs(n).size(); };
    // auto f_degree_in = [&](Node n) { return ops.preds(n).size(); };

    // Check which yields most cuts (c1 → c2 or c2 → c1)
    Edges cuts_c1 = f_make_cuts(c1,c2);
    Edges cuts_c2 = f_make_cuts(c2,c1);

    fmt::print("c1|c2: {}\n", cuts_c1);
    fmt::print("c1|c2: {}\n", cuts_c2);

    if ( cuts_c1.empty() && cuts_c2.empty() ) { return Edges{}; }

    auto cuts = ( cuts_c1.size() >= cuts_c2.size() )? cuts_c1 : cuts_c2;

    fn(cuts).ply([&](auto c){ out.emplace_back(c.first, c.second); });
  } // if cycle.size() > 4
    // for

  fmt::print("Supports:\n");
  rg::for_each(out, [](auto _1){ fmt::print("s: {}\n", _1); });

  return out;
} // function: get_cycle_supports }}}

// fn: cycle_get_support_weights {{{
template<Range R, typename V>
decltype(auto) cycle_get_support_weights(Ops const& ops, R&& cycle, V&& view)
{
  std::map<Edge, i64> map_edge_weight_supports;
  for (auto const& edge : get_cycle_supports(ops, cycle, view))
  {
    auto [u,v] = edge;
    err::err({ view.at(v) > view.at(u) })("v is not after u");
    i64 weight = view.at(v) - view.at(u);
    // i64 weight = 1;
    map_edge_weight_supports[edge]      = weight;
    map_edge_weight_supports[rev(edge)] = weight;
  } // for

  return map_edge_weight_supports;
  
} // function: cycle_get_support_weights }}}

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

        if( dist_cur > m_dist_exact )
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

  // Intersection must have size size of two
  err::err({intersection.size() >= 2})("Intersection size must at least 2");

  auto [u,v] = std::tie(intersection.at(0), intersection.at(1));

  // Edge weight for the first edge must equal 1
  i64 weight = m_edge_weight.at({u,v});
  err::err({weight == 1})("Initial intersection of {} and {} is {}, should be 1", u, v, weight);

  // For b1     is ( 1,-1)
  // For cm138a is ( 0,-1)
  if ( fn(ops.succs(u)).has(v) != 0 )
  {

    placement[v] = std::make_pair(-1,-1);
    placement[u] = std::make_pair( 0, 0);
  } // if
  else
  {
    placement[v] = std::make_pair( 0, 0);
    placement[u] = std::make_pair(-1,-1);
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
    , std::multimap<Node,Node> const& multimap_node_successor
    , std::set<Node> const& unreachable
    , bool failed
    )
    : m_placement(placement)
    , m_paths(paths)
    , m_mmap_node(multimap_node_successor)
    , m_unreachable(unreachable)
    , m_failed(failed)
  {}
};

cppcoro::generator<PlaceCycleRet> place_cycle(Ops const& ops
  , Placement p
  , Paths paths
  , std::multimap<Node,Node> multimap_node_successor
  , Range auto cycle
  , Range auto inter
  , Range auto cycle_outer
  , MEdgeWeight m_edge_weight
  , Map auto&& view
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
      // Remove from multimap_node_successor map
      bool is_path_u_to_v{fn(ops.succs(u)).has(node_target)};
      logger.info()("Path is {} to {}? {}", u, node_target, is_path_u_to_v);
      f_edge_erase_dummy(multimap_node_successor
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
        if( fn(ops.preds(u)).has(n) && tn.second <= t.second ) { return false; }  // if
        // A position after the placed outgoing edges is not good
        else if( fn(ops.succs(u)).has(n) && tn.second >= t.second ) { return false; } // else if
      }
      return true;
    }).vec();
  };

  // Only keep tiles that respect the distance heuristic
  auto f_tiles_keep_by_chebyshev_distance = [&](Node u, Nodes const& neighbors_positioned, Tiles tile_candidates)
  {
    // Target distance by edge weight
    auto f_target = [&](Node _1, Node _2) { return m_edge_weight.at({_1,_2}); };

    // For each tile, check if it adheres the distance constraint to every placed neighbor of u
    tile_candidates = fn(tile_candidates)
      .in_all(fn(neighbors_positioned).in(cycle).vec() , [&](Tile t, Node v) { return f_target(u,v) == f_dist_chebyshev(t,p.at(v)); })
      .vec();

    // // Get edge weights for supports
    // std::map<Edge, i64> map_edge_weight_supports = cycle_get_support_weights(ops, cycle, view);
    // auto f_has_support = [&](Node __1) -> std::optional<Node>
    // {
    //   auto __1_it = rg::find_if(map_edge_weight_supports, [&](auto&& ___1){ return ___1.first.first == __1; });
    //   if ( __1_it !=  rg::end(map_edge_weight_supports) ) { return __1_it->first.second; }
    //   return std::nullopt;
    // };
    // // Check if 'u' has support
    // if ( auto opt_support = f_has_support(u); opt_support )
    // {
    //   logger.info()("Node {} has support {}", u, *opt_support);
    //   // If it does check if it is placed
    //   if ( p.contains(*opt_support) )
    //   {
    //     // If it is, check if the distance equals to chebyshev
    //     auto tile_support = p.at(*opt_support);
    //     logger.info()("Support tile: {}", tile_support);
    //     tile_candidates = fn(tile_candidates).keep([&](auto&& __1)
    //     {
    //       return f_dist_chebyshev(__1, tile_support) == map_edge_weight_supports.at({u,*opt_support})
    //         or f_dist_chebyshev(__1, tile_support) == map_edge_weight_supports.at({u,*opt_support})+1;
    //     }).vec();
    //   } // if
    // } // if

    return tile_candidates;
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

  // All nodes of the same layer should be placed on the same 'y'
  auto f_tiles_by_layer = [&](Node u, Tiles const& _1)
  {
    Tiles _1_out{_1};

    // Current layer of u
    i64 _1_idx_layer_u = view.at(u);

    auto _1_it = std::ranges::find_if(view, [&](auto&& __1)
    {
      // Is on same layer and is placed
      return  __1.second == _1_idx_layer_u && p.contains(__1.first);
    });

    if ( _1_it != std::ranges::end(view) )
    {
      // Fetch current 'y' for layer
      Node _1_idx_layer_v{p.at(_1_it->first).second};
      
      return fn(_1_out).keep([&](auto&& __1){ return __1.second == _1_idx_layer_v; }).vec();

    } // if

    return _1_out;
  };

  // Do not allow intersection between cycles
  auto f_keep_outside_cycle = [&](auto&& _1)
  {
    // Fetch cycle tiles
    auto _1_tiles_cycle = fn(cycle_outer)
      .keep([&](Node __1){ return p.contains(__1); })
      .as([&](Node __1){ return p.at(__1); })
      .vec();

    return fn(_1).keep([&](auto&& __1)
    {
      // Keep tiles with same y as current tile __1
      auto _1_tiles_cycle_with_same_y = fn(_1_tiles_cycle).keep([&](auto&& ___1){ return ___1.second == __1.second; }).vec();
      // If has a y outside cycle range, is not in cycle
      if ( _1_tiles_cycle_with_same_y.empty() )     { return true; }
      // If current y has only 1 element, is not in cycle
      if ( _1_tiles_cycle_with_same_y.size() == 1 ) { return true; }
      // Check winding number
      return ! is_tile_inside_cycle(__1, _1_tiles_cycle);
    }).vec();
  };

  // // Only keep tiles that are reachable. A pair of tiles (t1,t2) are reachable when it is
  // // possible to create a path from t1 to t2, and this path has the same size as the chebyshev
  // // distance from t1 to t2
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
          , [&](Tile _1) -> Tiles { return f_keep_outside_cycle(f_get_candidates(_1,1)); }
          , [&](Tile _1) -> bool { return ! f_is_free(_1); }
          , [](Tile t1, Tile t2){ return ns_heuristics::chebyshev::run(t1,t2); }
          // , [&](auto&& _1){ return _1.size() == static_cast<size_t>(d_chebyshev+1); }
        )};
        // Check if path exists, and it is eq to chebyshev
        if( ! d_astar || static_cast<size_t>(d_chebyshev+1) != d_astar->size() ) { return false; } // if
      } // for
      return true;
    })
    .vec();
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

  // Get unique elements without changing the order
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
  if(unplaced.empty()) { co_yield PlaceCycleRet(p,paths,multimap_node_successor,{},false); } // if

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

      // Make all nodes on the same graph layer be on the same y coordinate
      tile_candidates = f_tiles_by_layer(u, tile_candidates);
      logger.info()("Candidates on the same layer: {}", tile_candidates);

      // Avoid nodes of a cycle inside another cycle (should be only adjacently)
      tile_candidates = f_keep_outside_cycle(tile_candidates);
      logger.info()("Candidates outside cycle: {}", tile_candidates);

      // Sort tile_candidates by distance x (or/and) y, to reduce area overhead
      tile_candidates = f_tiles_sort_by_overhead(tile_candidates);
      logger.info()("Candidates sorted by area overhead: {}", tile_candidates);

    } // if
    else
    {
      tile_candidates = m_backtrack.at(u);
    } // else

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
        unreachable = fn(cycle)
          .as([&](auto&& _1){ return fn(ops.preds(_1)).chain(ops.succs(_1)).vec(); })
          .squash()
          .keep([&](auto&& _1){ return p.contains(_1); })
          .chain(unreachable)
          .set();
        logger.info()("unreachable neighbors of {}: {}", u, unreachable);

        co_yield PlaceCycleRet({},{},multimap_node_successor,unreachable,true);
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
    bool has_routes{true};
    // for (auto v : fn(neighbors_positioned).in(cycle).vec())
    for (auto v : neighbors_positioned)
    {
      // Calculate chebyshev distance
      auto d_chebyshev{f_dist_chebyshev(chosen,p.at(v))};
      // Calculate chebyshev-based A* path
      auto d_astar{ns_search::a_star::run(chosen
          , p.at(v)
          , [&](Tile _1) -> Tiles { return f_keep_outside_cycle(f_get_candidates(_1,1)); }
          , [&](Tile _1) -> bool { return ! f_is_free(_1); }
          , [](Tile t1, Tile t2){ return ns_heuristics::chebyshev::run(t1,t2); }
          // , [&](auto&& _1){ return _1.size() == static_cast<size_t>(d_chebyshev+1); }
          )};

      // Check if path exists, and it is eq to chebyshev
      if( ! d_astar || static_cast<size_t>(d_chebyshev+1) != d_astar->size() )
      {
        has_routes = false;
        break;
      } // if

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
        for (Tile tile : fn(*d_astar).rev().deque())
        {
          wires_of[u][v][--lowest] = tile;
          p[lowest] = tile;
        } // for
        // Update multimap_node_successor from u to v
        f_edge_update_dummy(multimap_node_successor, u, v, fn(wires_of[u][v]).key().vec());
      }
      else
      {
        // Save new dummy nodes in-between
        // These are were introduced by A* to block paths
        for (Tile tile : *d_astar)
        {
          wires_of[u][v][--lowest] = tile;
          p[lowest] = tile;
        } // for
        // Update multimap_node_successor from v to u
        f_edge_update_dummy(multimap_node_successor, v, u, fn(wires_of[u][v]).key().vec());
      } // else
    } // for

    // Save node u in it
    p[u] = tile_candidates.front();

    // Erase used position
    tile_candidates.erase(tile_candidates.begin());

    // Save other positions to backtracking map
    m_backtrack[u] = tile_candidates;

    // Update placed stack
    placed.push(u);

    if( unplaced.empty() or ! has_routes )
    {
      if ( has_routes )
      {
        co_yield PlaceCycleRet(p,paths,multimap_node_successor,{},false);
      }

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

// fn: backtrack_until_unreachable_position_changes {{{
std::optional<cppcoro::generator<PlaceCycleRet>::iterator> backtrack_until_unreachable_position_changes(cppcoro::generator<PlaceCycleRet>& generator
  , Nodes const& vec_node_unreachable
  , Placement const& map_node_tile
  , Sink sink)
{
  err::Logger logger{sink};

  logger.info()("vec_node_unreachable: {}", vec_node_unreachable);

  for( auto it{generator.begin()}; it != generator.end(); ++it )
  {
    // Fetch current result
    auto const& gen_map_node_tile = it->m_placement;

    // Stop on failure
    if ( it->m_failed ) { break; }

    // Check if the target positions have changed
    auto f_has_changed_position = [&](Node _1)
    {
      err::err({ map_node_tile.contains(_1) })("map_node_tile does not contain: {}", _1);
      err::err({ gen_map_node_tile.contains(_1) })("gen_map_node_tile does not contain: {}", _1);
      return map_node_tile.at(_1) != gen_map_node_tile.at(_1);
    };
    if ( std::ranges::any_of(vec_node_unreachable, f_has_changed_position) )
    {
      logger.info()("nodes_unreachable_in_cycle (new positions): {}", fn(vec_node_unreachable)
        .keep([&](auto&& _1){ return gen_map_node_tile.contains(_1); })
        .as([&](auto&& _1){ return std::make_pair(_1, gen_map_node_tile.at(_1)); })
        .vec());
      return it;
    } // if
  } // for

  return std::nullopt;
} // function: backtrack_until_unreachable_position_changes }}}

// fn: insert_dummy_in_between {{{
//
// Given a cycle, and a dummy map, insert missing dummies between cycle nodes
//
decltype(auto) insert_dummy_in_between(Ops const& _1_ops, auto&& _1_mmap_node, auto&& _1_cycle, Sink sink)
{
  err::Logger _1_logger{sink};

  return fn(_1_cycle)
    .mut([](auto e){ e.push_back(e.front()); return e; })
    .slide(2)
    .as([&](auto e)
    {
      Nodes out;
      i64 u = e.at(0);
      i64 v = e.at(1);

      // u must be sucessor of v
      bool is_reversed{false};
      if ( ! fn(_1_ops.succs(u)).has(v) ) { is_reversed = true; v = std::exchange(u,v); }
      _1_logger.info()("u,v: {},{}\n", u, v);
      
      // If endpoint is not v and is not introduced by A*, remove
      auto remove_by = [&](Edges& _1)
      {
        _1 = fn(_1).keep([&](Edge const& __1)
        {
          // Check if is goal 'v'
          if ( __1.second == v )       { return true; }
          // Check if is introduced by A*
          if ( ! _1_ops.has(__1.second) ) { return true; }
          // Discard
          return false;
        }).vec();
      };

      if ( auto opt_nodes = path_from_multimap(_1_mmap_node, u, v, remove_by) )
      {
        _1_logger.info()("Nodes between uv: {}\n", *opt_nodes);
        if ( is_reversed ) { std::ranges::reverse(*opt_nodes); }
        return *opt_nodes;
      }

      return (is_reversed)? Nodes{v,u} : Nodes{u,v};
    })
    .squash()
    .unique()
    .vec();
}; // fn: insert_dummy_in_between }}}

// fn: global_backtracking {{{
template<typename C>
decltype(auto) global_backtracking(Ops const& ops
  , Basis& basis
  , [[maybe_unused]] C&& map_crossings_nodes
  , Sink sink)
{
  err::Logger logger{sink};

  // Solution
  Placement placement;
  Paths paths;

  // Get minimal basis
  // auto basis =
  //   f_timer({}, [&] { return minimal_basis_2(i64{},ops,map_crossings_nodes,logger.sink()); });

  for (auto&& base : basis)
  {
    logger.info()("-- Base: {}\n", base);
  } // for

  // Calculate graph depth-view
  auto view = f_timer({}, [&] { return ns_views::depth::run(i64{}, ops.preds, ops.succs).nl; });
  logger.info()("-- Built prox depth");
  for (auto e : view)
  {
    fmt::print("prox e: {}\n", e);
  } // for

  // Get edge weights for each basis

  MEdgeWeight m_edge_weight;
  for( auto const& cycle : basis )
  {
    auto m_cycle_edge_weight {edge_weights(ops, cycle, view)};
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

  // basis = f_timer({}, [&] { return detect_ears(ops, basis); });
  //
  // for (auto&& base : basis)
  // {
  //   logger.info()("-- Base (ear): {}\n", base);
  // } // for

  basis = f_timer({}, [&] { return minimal_basis_bfs_ordering(ops, basis, m_edge_weight); });

  // Process into adjacent intersection
  for (i32 i{}; auto const& base : basis)
  {
    logger.info()("-- Base (bfs) {}: {}", i++, base);
  } // for

  auto pair_intersection_basis =
    f_timer({}, [&] { return cycle_align_intersections(basis,logger.sink()); });

  // Process into adjacent intersection
  for (auto const& base : pair_intersection_basis)
  {
    logger.info()("-- Base (align): {}", base);
  } // for

  // Check if it has intersection.size() == 2
  err::err({ fn(pair_intersection_basis).any([&](auto&& _1)
  {
    auto const& __1_intersection = _1.first;
    return m_edge_weight.at({__1_intersection.front(),__1_intersection.back()}) == 1; })
  })
  ("The algorithm requires an intersection with weight 1");

  // Reverse the pairs before processing, I don't remember why
  f_timer({}, [&] { std::reverse(pair_intersection_basis.begin(), pair_intersection_basis.end()); });

  // Get initial intersection
  auto const& intersection_initial = pair_intersection_basis.back().first;

  // Place initial intersection
  placement =
    f_timer({}, [&] { return place_intersection(ops, intersection_initial, m_edge_weight); });

  // Currently placed node relationship
  // Nodes on this map are always point to their successors
  NodeMMap multimap_node_successor;

  // Stack generators and pair cycle/intersection
  struct StackBacktrack
  {
    cppcoro::generator<PlaceCycleRet> generator;
    Nodes cycle;
    Nodes intersection;
    Nodes cycle_outer;
  };

  std::stack<StackBacktrack> st_generator;

  // Push intersection
  if(auto [u,v] = std::make_pair(intersection_initial.at(0),intersection_initial.at(1)); fn(ops.succs(u)).has(v) )
  {
    paths[{u,v}] = std::deque<Tile>({placement.at(u), placement.at(v)});
    multimap_node_successor.emplace(u,v);
  } // if
  else
  {
    paths[{v,u}] = std::deque<Tile>({placement.at(v), placement.at(u)});
    multimap_node_successor.emplace(v,u);
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
    Nodes cycle_outer;
    cppcoro::generator<PlaceCycleRet> generator;
    std::optional<cppcoro::generator<PlaceCycleRet>::iterator> it_gen = std::nullopt;

    if( ! b_backtrack )
    {
      intersection = pair_intersection_basis.back().first;
      cycle = pair_intersection_basis.back().second;
      cycle_outer = (st_generator.empty())? cycle
        : cycle_merge_on_intersection(ops
          , st_generator.top().cycle_outer
          , cycle
          , fn(cycle).in(st_generator.top().cycle_outer).vec()
          , sink);
      pair_intersection_basis.pop_back();
      // Create generator
      generator = f_timer({}, [&]
      {
        return place_cycle(ops
            , placement
            , paths
            , multimap_node_successor
            , cycle
            , intersection
            , (st_generator.empty())? cycle : st_generator.top().cycle_outer
            , m_edge_weight
            , view
            , logger.sink()
          );
      });
      it_gen = generator.begin();
    } // if
    else
    {
      // Progressively remove nodes from nodes_unreachable set
      // This allows to backtrack until the earliest cycle that
      // contains one or more unreachable nodes(s)
      err::err({ ! st_generator.empty() })("Solution not found");

      logger.info()("unreachable: {}", nodes_unreachable);
      if ( ! nodes_unreachable.empty() and ! st_generator.empty() )
      {
        while ( ! nodes_unreachable.empty() and ! st_generator.empty() )
        {
          // Get previous generator
          generator = std::move(st_generator.top().generator);
          cycle = st_generator.top().cycle;
          cycle_outer = st_generator.top().cycle_outer;
          intersection = st_generator.top().intersection;
          st_generator.pop();

          // Check if this cycle contains unreachable nodes
          if ( auto nodes_unreachable_in_cycle = fn(cycle).in(nodes_unreachable).vec(); ! nodes_unreachable_in_cycle.empty() )
          {
            logger.info()("Cycle: {}", cycle);
            logger.info()("nodes_unreachable_in_cycle: {}", fn(nodes_unreachable_in_cycle)
              .keep([&](auto&& _1){ return placement.contains(_1); })
              .as([&](auto&& _1){ return std::make_pair(_1, placement.at(_1)); })
              .vec());
            // Erase intersection of the current cycle with unreachable nodes
            std::ranges::for_each(nodes_unreachable_in_cycle, [&](auto _1){ nodes_unreachable.erase(_1); });
            // Try to generate a solution with a different position for at least 1 node in the
            // intersection
            if ( auto opt_it = backtrack_until_unreachable_position_changes(generator, cycle, placement, logger.sink()); opt_it )
            {
              placement = opt_it.value()->m_placement;
              paths     = opt_it.value()->m_paths;
              multimap_node_successor = opt_it.value()->m_mmap_node;
              it_gen = *opt_it;
              break;
            } // if
          } // if

          // Re-insert intersection and base into the to-place vector
          if ( ! nodes_unreachable.empty() and ! st_generator.empty() )
          {
            pair_intersection_basis.push_back(std::make_pair(intersection,cycle));
          } // if
        } // while

      } // if
      else
      {
        // Update generator/cycle
        generator = std::move(st_generator.top().generator);
        cycle = st_generator.top().cycle;
        intersection = st_generator.top().intersection;
        cycle_outer = st_generator.top().cycle_outer;
        st_generator.pop();

        // Set iterator to begin
        it_gen = generator.begin();
      } // else

      // Disable backtracking
      b_backtrack = false;

    } // else

    logger.info()("Cycle        : {}", cycle);
    logger.info()("Cycle Outer  : {}", cycle_outer);
    logger.info()("Intersection : {}", intersection);

    // If a solution is found from current placement
    // - Update final solution
    // - Push generator to backtracking stack
    // - Push incident cycle to backtracking stack

    auto check_half_separations = [&]
    {
      while ( it_gen && it_gen.value() != generator.end() && it_gen.value()->m_failed == false )
      {
        placement = it_gen.value()->m_placement;
        paths     = it_gen.value()->m_paths;
        multimap_node_successor = it_gen.value()->m_mmap_node;

        // Check for half separation
        // Test resulting cycle, to see if it hasn't twisted within itself
        // // Re-create cycle with dummy nodes

        auto f_insert_dummy_in_between = [&](auto&& _1){ return  insert_dummy_in_between(ops, multimap_node_successor, _1, logger.sink()); };

        Nodes cycle_with_dummy = f_insert_dummy_in_between(cycle);

        logger.info()("Dummy Cycle: {}\n", cycle_with_dummy);

        bool has_inner_crossings = cycle_has_inner_crossings_2(cycle_with_dummy, placement, sink);

        // Check for inner_crossings
        if ( has_inner_crossings )
        {
          logger.info()("Cycle {} has inner crossings\n", i);
          it_gen = std::next(it_gen.value());
        }
        else
        {
          // Check if backtracking on the first cycle
          // Check if there is a node of outer_cycle inside cycle
          Nodes cycle_outer_with_dummy;
          if ( ! st_generator.empty() )
          {
            cycle_outer_with_dummy = f_insert_dummy_in_between(st_generator.top().cycle_outer);
          }
          else
          {
            cycle_outer_with_dummy = cycle;
          } // else

          if ( st_generator.empty() or ! cycles_intersect(placement, cycle_with_dummy, cycle_outer_with_dummy) )
          {
            // Pass
#ifdef DEBUG
            result_draw(Placement{placement}, Paths{paths}, std::multimap<Node,Node>{multimap_node_successor}, i, "", logger.sink());
#endif
            break;
          } // if

          // Failed
          it_gen = std::next(it_gen.value());

        } // else
      }
    };

    check_half_separations();

    if( it_gen && it_gen.value() != generator.end() && it_gen.value()->m_failed == false )
    {
      st_generator.push({std::move(generator),cycle,intersection,cycle_outer});
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
      nodes_unreachable = (it_gen && it_gen.value() != generator.end())? it_gen.value()->m_unreachable : std::set<Node>{};
      logger.info()("-- Failed for cycle: {}", cycle);
    } // else

    // Increase idx
    ++i;
  } // while


  std::erase_if(placement,[&](auto&& e){ return e.first < id_lowest; });

  return std::make_pair(placement,paths);

} // fn: global_backtracking }}}

// fn: pre_processing {{{
decltype(auto) pre_processing(Ops const& ops)
{
  auto view = ns_views::depth::run(i64{}, ops.preds, ops.succs);
  f_timer({}, [&]{ ns_ops::balance::outgoing::run(0,ops); });
  view = ns_views::depth::run(i64{}, ops.preds, ops.succs);
  f_timer({}, [&]{ ns_ops::balance::paths::run(i64{},ops,view); });
  view = ns_views::depth::run(i64{}, ops.preds, ops.succs);
  auto map_crossings_nodes = f_timer({}, [&]{ return ns_ops::balance::crossings::run(i64{}, ops); });
  unbalance(0, ops);
  return map_crossings_nodes;
} // function: pre_processing }}}

// fn: decode_node_ids {{{
auto decode_node_ids(auto&& ops, auto&& e_basis, Sink sink)
{
  err::Logger logger{sink};

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
    .as([&](auto&& e){ return fn(e).as([&](auto _1) { return m_nodes_norm.at(_1); }).vec(); })
    .vec();

  // Align cycles from upper node to bottom node
  auto f_align_cycle = [&](auto const& cycle)
  {
    auto [fst,snd] = cycle_split_in_endpoints(ops, cycle);
    return fn(fst).pop_back(1).chain(snd).pop_back(1).vec();
  };

  basis = fn(basis).as([&](auto&& e){ return f_align_cycle(e); }).vec();
  fn(basis).ply([&](auto&& e){ logger.info()("Basis: {}\n", e); });

  return basis;
} // fn: decode_node_ids }}}

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

  auto f_write_v = [&]<typename... Args>(Args&&... args) { ns_io_verilog::Writer(std::forward<Args>(args)...); };
  auto f_write_d = [&]<typename... Args>(Args&&... args) { ns_io_dimacs::Writer(std::forward<Args>(args)...); };

  //
  // Pre-processings
  //
  f_timer({}, f_write_v, metadata.data(), f_p, f_s, "out/0-out.v");
  auto map_crossings_nodes = pre_processing(ops);
  f_timer({}, f_write_v, metadata.data(), f_p, f_s, "out/1-out.v");
  f_timer({}, f_write_d, g.data(), ops.preds, ops.succs, "out/out.dimacs");
  Basis e_basis = get_cycle_basis("./out/out.dimacs");
  Basis basis = decode_node_ids(ops, e_basis, logger.sink());
  auto view = ns_views::depth::run(i64{}, ops.preds, ops.succs);

  //
  // Perform placement
  //
  auto start {std::chrono::system_clock::now()};
  auto [placement, routing] = global_backtracking(ops,basis,map_crossings_nodes,logger.sink());
  auto end {std::chrono::system_clock::now()};
  std::chrono::duration<f64> dur {end-start};
  std::stringstream ss; ss << dur.count();

  fmt::print("Circuit: {}\n", argv[1]);
  fmt::print("Time(s): {}\n", ss.str());

  result_draw(placement, routing, {}, 0, "out/out.svg", logger.sink());

  return EXIT_SUCCESS;
} // main }}}
