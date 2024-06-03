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

#include <iostream>

#include <argparse/argparse.hpp>
#include <spdlog/spdlog.h>

#include <celaeno/aliases.hpp>
#include <celaeno/enum/enum.hpp>
#include <celaeno/err/err.hpp>
#include <celaeno/graph/graph.hpp>
#include <celaeno/graph/io/verilog.hpp>
#include <celaeno/graph/draw/svg.hpp>
#include <celaeno/graph/views/depth.hpp>
#include <celaeno/graph/operations/balance/crossings.hpp>
#include <celaeno/graph/operations/balance/unbalance.hpp>
#include <celaeno/graph/representations/collapsed.hpp>

using namespace celaeno::aliases;

namespace ns_graph = celaeno::graph;
namespace ns_verilog = celaeno::graph::io::verilog;
namespace ns_err = celaeno::err;
namespace ns_view = celaeno::graph::views;

namespace fs = std::filesystem;

struct Graph
{
  std::unique_ptr<ns_graph::Graph<i64>> data;
  std::unique_ptr<ns_verilog::Metadata> metadata;
  std::unique_ptr<ns_view::depth::Result<i64>> view;
  std::unique_ptr<ns_graph::Ops> ops;
  Graph()
    : data(nullptr)
    , metadata(nullptr)
    , view(nullptr)
    , ops(nullptr)
  {}; // Graph
  Graph(Graph const& other)
    : data(std::make_unique<ns_graph::Graph<i64>>(*other.data))
    , metadata(std::make_unique<ns_verilog::Metadata>(*other.metadata))
    , view(std::make_unique<ns_view::depth::Result<i64>>(*other.view))
    , ops(nullptr)
  {
    auto f_p = [&](auto v){ return data->predecessors(v); };
    auto f_s = [&](auto v){ return data->successors(v); };
    auto f_a = [&](auto u, auto v){ return data->adjacent(u,v); };
    auto f_l = [&](auto u, auto v){ data->emplace(std::make_pair(u,v)); };
    auto f_u = [&](auto u, auto v){ data->erase(std::make_pair(u,v)); };
    auto f_h = [&](auto u){ return data->has(u); };
    ops = std::make_unique<ns_graph::Ops>(f_p, f_s, f_a, f_l, f_u, f_h);
  } // Graph
}; // Graph

using Tile = std::pair<i64,i64>;
using Node = i64;

// read() {{{
Graph read(fs::path const& path_file_hdl)
{
  Graph graph;
  auto data = std::make_unique<ns_graph::Graph<Node>>();
  auto emplace = [&data](auto&& e) -> void { data->emplace(e); };
  graph.metadata = std::make_unique<ns_verilog::Metadata>(ns_verilog::Reader(path_file_hdl.c_str(), emplace).data());
  graph.data = std::move(data);
  auto f_p = [&](auto v){ return graph.data->predecessors(v); };
  auto f_s = [&](auto v){ return graph.data->successors(v); };
  auto f_a = [&](auto u, auto v){ return graph.data->adjacent(u,v); };
  auto f_l = [&](auto u, auto v){ graph.data->emplace(std::make_pair(u,v)); };
  auto f_u = [&](auto u, auto v){ graph.data->erase(std::make_pair(u,v)); };
  auto f_h = [&](auto u){ return graph.data->has(u); };
  graph.ops = std::make_unique<ns_graph::Ops>(f_p, f_s, f_a, f_l, f_u, f_h);
  graph.view = std::make_unique<ns_view::depth::Result<i64>>(
    ns_view::depth::run(Node{}, graph.ops->preds, graph.ops->succs)
  );
  return graph;
} // read() }}}

// draw() {{{
void draw(Graph const& g
  , auto&& view_depth
  , std::optional<std::map<Node,i64>> opt_map_node_xpos
  , fs::path path_file_svg)
{
  // Define y position as height in depth view
  auto f_calculate_ypos = [&](Node node)
  {
    return view_depth.nl.at(node);
  };

  // Define x position as index on layer
  auto f_calculate_xpos = [&](Node node)
  {
    if ( opt_map_node_xpos.has_value() )
    {
      return opt_map_node_xpos->at(node);
    } // if

    // Get index of layer which contains the node
    auto idx_layer = view_depth.nl.at(node);
    // Get layer reference by index
    auto const& layer = view_depth.ln.at(idx_layer);
    // Get node index on layer
    auto it = std::ranges::find(layer, node);
    if ( it == std::ranges::end(layer) )
    {
      std::runtime_error("Could not find node in graph");
    }
    return std::distance(layer.begin(), it);
  };

  // Calculate tile for each node
  std::map<Node, Tile> map_node_tile;
  for( auto i : g.data->vertices() )
  {
    map_node_tile.emplace(i, Tile(f_calculate_xpos(i), f_calculate_ypos(i)));
  } // for

  // Define paths as the source and target node tiles
  using Paths = std::vector<std::vector<Tile>>;
  Paths paths;
  for( auto [u,v] : g.data->data() )
  {
    paths.emplace_back(std::vector<Tile>{map_node_tile.at(u), map_node_tile.at(v)});
  } // for

  ns_graph::draw::svg::svg(path_file_svg.c_str()
    , map_node_tile
    , paths
    , [&](auto node) -> std::string { return ns_string::to_string(node); });

} // draw() }}}

// main() {{{
int main(int argc, char const* argv[])
{
  ns_err::Logger logger;

  // Parse args
  argparse::ArgumentParser program("Celaeno");

  program.add_argument("--verilog")
    .required()
    .help("Specify the input verilog file");

  program.add_argument("--output-svg")
    .required()
    .help("Target svg file name");

  try
  {
    program.parse_args(argc, argv);
  } // try
  catch (const std::exception& err)
  {
    std::cerr << err.what() << std::endl;
    std::cerr << program;
    std::exit(1);
  } // catch

  // Create graph
  fs::path path_file_hdl = program.get<std::string>("--verilog");
  auto graph = read(program.get<std::string>("--verilog"));
  fmt::print("Gates: {}\n", graph.data->vertices_count());
  fmt::print("Wires: {}\n", graph.data->edges_count());

  // Balance outgoing edges (max of 2)
  ns_graph::operations::balance::outgoing::run(Node{}, *graph.ops);

  // Create depth view
  auto view_depth = ns_view::depth::run(Node{}, graph.ops->preds, graph.ops->succs);

  // Balance outgoing edges
  ns_graph::operations::balance::outgoing::run(Node{}, *graph.ops);
  graph.view = std::make_unique<ns_graph::views::depth::Result<i64>>(
    ns_view::depth::run(Node{}, graph.ops->preds, graph.ops->succs)
  );

  // Balance graph paths
  ns_graph::operations::balance::paths::run(Node{}, *graph.ops, view_depth);
  graph.view = std::make_unique<ns_graph::views::depth::Result<i64>>(
    ns_view::depth::run(Node{}, graph.ops->preds, graph.ops->succs)
  );

  // Mark crossings
  graph.view = std::make_unique<ns_graph::views::depth::Result<i64>>(
    ns_graph::operations::balance::crossings::run(*graph.ops, view_depth)
  );

  // Balance graph paths
  ns_graph::operations::balance::paths::run(Node{}, *graph.ops, view_depth);
  graph.view = std::make_unique<ns_graph::views::depth::Result<i64>>(
    ns_view::depth::run(Node{}, graph.ops->preds, graph.ops->succs)
  );

  // Draw svg
  fs::path path_file_svg = program.get<std::string>("--output-svg");
  path_file_svg = path_file_svg.parent_path() / "out.0.svg";
  draw(graph, view_depth, std::nullopt, path_file_svg);

  // Save separate balanced graph
  auto graph_balanced = Graph(graph);

  // Helper to determine if a node is to the left or to the right of parent
  auto f_orientation = [&](Node node_parent, Node node_child)
  {
    // // Get successors
    // auto succs_parent = graph_balanced.ops->succs(node_parent);
    // if ( succs_parent.size() == 2 )
    // {
    //   // Get layer in which they reside
    //   auto const& layer = graph_balanced.view->ln.at(graph_balanced.view->nl.at(node_child));
    //   // Find first occurrence in layer
    //   auto it = std::ranges::find_if(layer, [&](auto e)
    //   {
    //     return e == succs_parent.at(0) or e == succs_parent.at(1);
    //   });
    //   ns_err::err({ it != std::ranges::end(layer) })("Nodes not found in reference layer");
    //   // Return left or right based on if the found node is child
    //   return (*it == node_child)?
    //       ns_graph::representations::collapsed::Orientation::LEFT
    //     : ns_graph::representations::collapsed::Orientation::RIGHT;
    // } // if

    // Get predecessors
    auto preds_child = graph_balanced.ops->preds(node_child);
    if ( preds_child.size() == 2 )
    {
      // Get layer in which they reside
      auto const& layer = graph_balanced.view->ln.at(graph_balanced.view->nl.at(node_parent));
      // Find first occurrence in layer
      auto it = std::ranges::find_if(layer, [&](auto e)
      {
        return e == preds_child.at(0) or e == preds_child.at(1);
      });
      ns_err::err({ it != std::ranges::end(layer) })("Nodes not found in reference layer");

      // Return left or right based on if the found node is parent
      return (*it == node_parent)?
          ns_graph::representations::collapsed::Orientation::RIGHT
        : ns_graph::representations::collapsed::Orientation::LEFT;
    } // if

    throw std::runtime_error("Invalid graph configuration");
  };

  // Unbalance
  graph.view = std::make_unique<ns_graph::views::depth::Result<i64>>(
    ns_graph::operations::balance::unbalance::run(Node{}, *graph.ops, view_depth)
  );

  // Determine x positions for each layer
  auto map_node_xpos = ns_graph::representations::collapsed::collapse(*graph.ops, view_depth, f_orientation);

  // Draw svg
  path_file_svg = path_file_svg.parent_path() / "out.1.svg";
  draw(graph, view_depth, std::make_optional(map_node_xpos), path_file_svg);

  return EXIT_SUCCESS;
} // main() }}}
