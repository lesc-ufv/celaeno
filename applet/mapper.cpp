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


#include <filesystem>

#include <fmt/core.h>

#include <cstdint>
#include <fplus/fplus.hpp>
#include <range/v3/all.hpp>
#include <spdlog/spdlog.h>
#include <cppcoro/generator.hpp>

#include <celaeno/aliases.hpp>
#include <celaeno/concepts.hpp>
#include <celaeno/log/log.hpp>
#include <celaeno/fun/fun.hpp>

#include <celaeno/graph/graph.hpp>
#include <celaeno/graph/io/verilog.hpp>
#include <celaeno/graph/io/dimacs.hpp>
#include <celaeno/graph/io/dot.hpp>

#include <celaeno/graph/views/depth.hpp>

#include <celaeno/graph/search/a-star.hpp>
#include <celaeno/graph/search/bfs.hpp>
#include <celaeno/graph/search/kahn.hpp>
#include <celaeno/graph/search/zig-zag.hpp>

#include <celaeno/heuristics/chebyshev.hpp>
#include <celaeno/heuristics/manhattan.hpp>

#include <celaeno/graph/operations/balance/crossings.hpp>
#include <celaeno/graph/operations/balance/unbalance.hpp>
#include <celaeno/graph/operations/minimize/crossings.hpp>


// TODO Remove
#include <celaeno/graph/operations/balance/outgoing.hpp>
#include <celaeno/graph/operations/balance/incoming.hpp>
#include <celaeno/graph/operations/count/crossings.hpp>

// #include <pybind11/embed.h>
// #include <pybind11/stl.h>

#include <boost/geometry.hpp>
#include <boost/geometry/geometries/point_xy.hpp>
#include <boost/geometry/geometries/polygon.hpp>
#include <boost/geometry/geometries/geometries.hpp>
#include <boost/geometry/geometries/segment.hpp>

#include <boost/preprocessor/repetition.hpp>
#include <boost/preprocessor/punctuation/comma_if.hpp>
#include <boost/preprocessor/cat.hpp>
#include <boost/preprocessor/variadic/size.hpp>

// Using namespace {{{
using namespace celaeno::fun::fn;
using namespace celaeno::concepts;
using namespace celaeno::aliases;
// }}}

// namespaces {{{
// namespace py = pybind11;
namespace fs = std::filesystem;

namespace ns_log = celaeno::log;
namespace ns_graph = celaeno::graph;
namespace ns_ops = celaeno::graph::operations;
namespace ns_io_verilog = celaeno::graph::io::verilog;
namespace ns_io_dot = celaeno::graph::io::dot;
namespace ns_views = celaeno::graph::views;
// }}}

// Aliases {{{
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
using Location = celaeno::log::Location;
// }}}

// fn: timer {{{
template<typename F, typename... Args>
auto timer(Location const& loc, F&& f, Args&&... args)
{
  if constexpr(std::is_void_v<std::invoke_result_t<F, Args...>>)
  {
    spdlog::info("[exe] {}", loc.get());
    auto start {std::chrono::system_clock::now()};
    std::invoke(std::forward<F>(f), std::forward<Args>(args)...);
    auto end {std::chrono::system_clock::now()};
    std::chrono::duration<f64> dur {end-start};
    spdlog::info("[end] {} took {} seconds", loc.get(), dur.count());
    return;
  }
  else
  {
    spdlog::info("[exe] {}", loc.get());
    auto start {std::chrono::system_clock::now()};
    auto result = std::invoke(std::forward<F>(f), std::forward<Args>(args)...);
    auto end {std::chrono::system_clock::now()};
    std::chrono::duration<f64> dur {end-start};
    spdlog::info("[end] {} took {} seconds", loc.get(), dur.count());
    return result;
  }
}; // fn: timer }}}

// fn: pre_processing {{{
decltype(auto) pre_processing(Ops const& ops
  , std::map<Node, ns_graph::operations::balance::crossings::Direction>& map_node_direction)
{
  // Balance outgoing edges to a maximum of 2
  timer({}, [&]{ns_ops::balance::outgoing::run(0,ops);});

  // Create depth-view
  auto view = ns_views::depth::run(i64{}, ops.preds, ops.succs);

  // Balance paths
  timer({}, [&]{ns_ops::balance::paths::run(i64{},ops,view);});

  // ns_graph::operations::minimize::crossings::run(0, ops.preds, ops.succs, ops.adj, view);

  // Write to file
  auto f_write_v = [&]<typename... Args>(Args&&... args) { ns_io_dot::Writer(std::forward<Args>(args)...); };
  timer({}, f_write_v, view, ops, "out/1-out.dot");

  // Count number of crossings
  spdlog::info("Number of crossings: {}", ns_ops::count::crossings::run(ops, view.ln));

  // Balance crossings
  view = timer({}, LR(ns_ops::balance::crossings::run(ops, view, map_node_direction),0));
  spdlog::info("Number of layers: {}", view.ln.size());
  spdlog::info("Largest layer size: {}", fn(view.ln).as(LR(_1.second.size())).max() );

  // Write to file
  timer({}, f_write_v, view, ops, "out/2-out.dot");

  view = timer({}, LR(ns_ops::balance::unbalance::run(0, ops, view),0));

  // Write to file
  timer({}, f_write_v, view, ops, "out/3-out.dot");

  return view;
} // function: pre_processing }}}

// fn get_area {{{
template<typename Map>
decltype(auto) get_area(Map&& map_node_position)
{
  struct Area
  {
    int64_t width;
    int64_t height;
  }; // Area
  auto width_minmax = std::ranges::minmax_element(map_node_position | std::views::values | std::views::keys);
  auto height_minmax = std::ranges::minmax_element(map_node_position | std::views::values | std::views::values);
  i64 width = *(width_minmax.max) - *(width_minmax.min);
  i64 height = *(height_minmax.max) - *(height_minmax.min);
  return Area(width, height);
} // function: get_area }}}

// fun: main  {{{
int main([[maybe_unused]] int argc, char const* argv[])
{
  ns_log::init();
  spdlog::set_level(spdlog::level::info);

  if ( argc < 2 )
  {
    spdlog::error("No arguments passed to the program");
    spdlog::error("Usage:");
    spdlog::error("./mapper circuit.v");
  } // if

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

  spdlog::info("Gates: {}", g.vertices_count());
  spdlog::info("Wires: {}", g.edges_count());

  // Create ops
  Ops ops(f_p, f_s, f_a, f_l, f_u, f_h);

  // Create output directory
  fs::create_directory("./out");

  // Pre-processings
  std::map<Node, ns_graph::operations::balance::crossings::Direction> map_node_direction;
  auto view = pre_processing(ops, map_node_direction);
  for(auto [k,v] : map_node_direction)
  {
    spdlog::info("Node: {} IsLeft: {}", k, v == ns_graph::operations::balance::crossings::LEFT);
  } // for

  // Collapse
  auto [nodes_collapsed, positions_collapsed] = timer({}
    , [&]{ return ns_ops::balance::crossings::view_collapse(ops, view, map_node_direction); }
  );

  auto map_node_position = fn(nodes_collapsed)
    .zip(positions_collapsed)
    .as([&](auto&& e){ return std::make_pair(e.first, std::make_pair(e.second, view.nl.at(e.first))); })
    .map();

  // Write to file
  auto f_write_v = [&]<typename... Args>(Args&&... args) { ns_io_dot::Writer(std::forward<Args>(args)...); };
  timer({}, f_write_v, view, ops, "out/4-out.dot", map_node_position);

  // Print area
  auto area = get_area(map_node_position);
  spdlog::info("Area: {}x{}", area.width, area.height);

  return EXIT_SUCCESS;
} // main }}}

// cmd: set noautochdir
// cmd: chdir ~/Repositories/celaeno
// cmd: GdbStart gdb --args ./build/test/g_search_zz test/data/synth-91/C17.v
