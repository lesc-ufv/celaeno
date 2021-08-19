// vim: set expandtab fdm=marker ts=2 sw=2 tw=80 et :
//
// @author      : Ruan E. Formigoni (ruanformigoni@gmail.com)
// @file        : graph
// @created     : Wednesday Aug 18, 2021 17:35:57 UTC
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

#include <numeric>
#include <string_view>
#include <sstream>
#include <fstream>
#include <queue>
#include <random>

#include <fmt/core.h>
#include <fmt/ranges.h>
#include <range/v3/all.hpp>
#include <fplus/fplus.hpp>
#include <spdlog/spdlog.h>

#include <celaeno/aliases.hpp>
#include <celaeno/concepts.hpp>
#include <celaeno/heuristics/manhattan.hpp>
#include <celaeno/graph/graph.hpp>
#include <celaeno/graph/views/depth.hpp>
#include <celaeno/graph/search/bfs.hpp>
#include <celaeno/graph/search/a-star.hpp>
#include <celaeno/graph/operations/balance/paths.hpp>
#include <celaeno/graph/operations/balance/outgoing.hpp>
#include <celaeno/graph/operations/minimize/crossings.hpp>
#include <celaeno/graph/operations/minimize/pseudo.hpp>
#include <celaeno/graph/operations/minimize/edge-length.hpp>
#include <celaeno/graph/representations/grid/layered.hpp>


// namespace celaeno::svg::graph {{{
namespace celaeno::svg::graph
{

// Macros {{{
#define assertm(exp, msg) assert(((void)msg, exp))
// }}}

// Using declarations {{{
using Ops = celaeno::graph::Ops;
// }}}

// namespaces {{{
namespace rg = ranges;
namespace rv = ranges::views;
namespace fp = fplus;

namespace ns_balance = celaeno::graph::operations::balance;
namespace ns_minimize = celaeno::graph::operations::minimize;
namespace ns_grid = celaeno::graph::representations::grid;
namespace ns_search = celaeno::graph::search;
namespace ns_views = celaeno::graph::views;
// }}}

// Using namespaces {{{
using namespace celaeno::concepts;
using namespace celaeno::aliases;
// }}}

// Shapes {{{
constexpr std::string_view const h_template
{
  "<svg version='1.1' viewBox='0 0 {} {}'>\n"
};

constexpr std::string_view const v_label_template
{
  "<text x='{}' y='{}' text-anchor='middle' font-size='{}' fill='{}'>{}</text>\n"
};

constexpr std::string_view const v_template
{
  "<circle stroke='black' stroke-width='2px' r='{}' cx='{}' cy='{}' fill='{}'/>\n",
};

constexpr std::string_view const e_template
{
  "<line x1 = '{}' y1 = '{}' x2 = '{}' y2 = '{}' "
  "stroke = 'black' stroke-width = '1'/>\n"
};

constexpr std::string_view const footer{"</svg>\n"};
// }}}

// Settings {{{

// Radius
constexpr i32 const vertex_radius{10};
// Size of each tile
constexpr i32 const tile_size{vertex_radius*3};
// Offset of circles from the edges
constexpr i32 const circle_offset{tile_size/2};

// }}}

// fn: svg {{{
template<SignedIntegral I, typename Map, String S>
void svg(I root, Ops const& ops, Map&& m_vertex_tile, S&& filename)
{
  // Output file
  std::ofstream of{filename};

  // Check output file state
  if ( ! of.good() )
  {
    spdlog::error("{}@{} Not possible to create file {}", __FILE__,__LINE__,filename);
    exit(1);
  } // if

  // Streams
  std::stringstream header;
  std::stringstream vertices;
  std::stringstream edges;

  //Get farthest vertices to build viewport
  auto view_box_x {rg::max_element(m_vertex_tile,{},
  [](auto e) { return e.second.x; })->second.x};

  auto view_box_y {rg::max_element(m_vertex_tile,{},
  [](auto e) { return e.second.y; })->second.y};

  // Adjust positions for drawing
  for (auto& [_,tile] : m_vertex_tile)
  {
    tile.x = tile.x*tile_size+circle_offset;
    tile.y = tile.y*tile_size+circle_offset;
  } // for

  // Edge Routing
  ns_search::bfs::run(root
    ,ops
    , [&,visited=std::set<std::pair<I,I>>{}](I u) mutable
    {
      auto f_neighbors = [&](I v){ return fp::append(ops.preds(v),ops.succs(v)); };
      auto neighbors{f_neighbors(u)};

      for (auto v : neighbors)
      {
        if( visited.contains({u,v}) || visited.contains({v,u}) ) { continue; }

        auto [ux,uy] = m_vertex_tile.at(u).to_pair();
        auto [vx,vy] = m_vertex_tile.at(v).to_pair();

        edges << fmt::format(e_template, ux, uy, vx, vy);
      } // for
      return false;
    }
  );

  // @ Stream/File writting
  header << fmt::format(h_template,view_box_x*tile_size+circle_offset*2, view_box_y*tile_size+circle_offset*2);

  // Vertices and labels
  for (auto [v,tile] : m_vertex_tile)
  {
    auto [x,y] = std::make_pair(tile.x,tile.y);
    // Draw pseudo-nodes with black filling
    if (v < 0)
    {
      vertices << fmt::format(v_template, vertex_radius, x, y, "black");
      vertices << fmt::format(v_label_template, x, y, 10, "white", v);
    } // if
    else
    {
      vertices << fmt::format(v_template, vertex_radius, x, y, "white");
      vertices << fmt::format(v_label_template, x, y, 10, "black", v);
    } // else
  } // for

  // File writting
  of << fmt::format("{}\n", header.str());


  of << fmt::format("{}\n", edges.str());
  of << fmt::format("{}\n", vertices.str());
  of << fmt::format("{}\n", footer);
  of.close();
} // function: svg }}}

// fn: run  {{{
template<SignedIntegral S, String Str>
decltype(auto) run(S root, Ops ops, Str&& fn)
{

#if ! defined(NDEBUG) && defined(DEBUG_SHOW_ALG)
  spdlog::set_level(spdlog::level::debug);
  spdlog::debug("Algorithm: celaeno::svg::graph");
#endif

  //
  // Pre-processing
  //
  ns_balance::paths::run(root,ops);
  ns_balance::outgoing::run(root,ops);
  ns_balance::paths::run(root,ops);

  //
  // Layer ordering
  //
  auto depth_view{ns_views::depth::run(root, ops.preds, ops.succs).ln};
  auto layers {fp::get_map_values(depth_view)};

  //
  // Tile Placement and Edge Routing
  //
  auto m_vertex_tile {ns_grid::layered::run(root, ops, layers)};

  // Write output .svg file
  svg(root, ops, m_vertex_tile, fmt::format("{}.svg", fn));

} // function: run }}}

} // namespace celaeno::svg::graph }}}
