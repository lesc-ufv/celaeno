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
#include <filesystem>

#include <fmt/core.h>

#include <cstdint>
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
#include <celaeno/graph/io/dot.hpp>

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
namespace ns_io_dot = celaeno::graph::io::dot;
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

// fn: timer {{{
template<typename F, typename... Args>
auto timer(Location const& loc, F&& f, Args&&... args)
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
}; // fn: timer }}}

// fn: pre_processing {{{
decltype(auto) pre_processing(Ops const& ops, auto&& metadata, auto&& edges)
{
  // Balance outgoing edges to a maximum of 2
  timer({}, [&]{ns_ops::balance::outgoing::run(0,ops);});

  // Create depth-view
  auto view = ns_views::depth::run(i64{}, ops.preds, ops.succs);

  // Balance paths
  timer({}, [&]{ns_ops::balance::paths::run(i64{},ops,view);});

  // Write to file
  auto f_write_v = [&]<typename... Args>(Args&&... args) { ns_io_dot::Writer(std::forward<Args>(args)...); };
  timer({}, f_write_v, view, ops, "out/1-out.dot");

  // Count number of crossings
  fmt::print("Number of crossings: {}\n", ns_ops::count::crossings::run(0, ops, view.ln));

  // Balance crossings
  view = timer({}, LR(ns_ops::balance::crossings::run(ops, view),0));
  fmt::print("Number of layers: {}\n", view.ln.size());
  fmt::print("Largest layer size: {}\n", fn(view.ln).as(LR(_1.second.size())).max() );
  fmt::print("Balance crossings layer/nodes:\n");
  for (auto&& [l,nds] : view.ln)
  {
    fmt::print("l: {} - n: {}\n", l, nds);
  } // for

  // Write to file
  timer({}, f_write_v, view, ops, "out/2-out.dot");

  // Balance paths
  timer({}, [&]{ns_ops::balance::paths::run(i64{},ops,view);});

  return view;
} // function: pre_processing }}}

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

  // Create output directory
  std::error_code ec{}; fs::create_directory("./out", ec);

  // Pre-processings
  auto view = pre_processing(ops, metadata.data(),g.data());

  // Collapse
  auto [nodes_collapsed, positions_collapsed] = ns_ops::balance::crossings::view_collapse(ops, view.ln);
  auto map_node_position = fn(nodes_collapsed)
    .zip(positions_collapsed)
    .as([&](auto&& e){ return std::make_pair(e.first, std::make_pair(e.second, view.nl.at(e.first))); })
    .map();

  // Write to file
  auto f_write_v = [&]<typename... Args>(Args&&... args) { ns_io_dot::Writer(std::forward<Args>(args)...); };
  timer({}, f_write_v, view, ops, "out/3-out.dot", map_node_position);

  return EXIT_SUCCESS;
} // main }}}

// cmd: set noautochdir
// cmd: chdir ~/Repositories/celaeno
// cmd: GdbStart gdb --args ./build/test/g_search_zz test/data/synth-91/C17.v
