// vim: set expandtab fdm=marker ts=2 sw=2 tw=80 et :
//
// @author      : Ruan E. Formigoni (ruanformigoni@gmail.com)
// @file        : svg
// @created     : monday set 14, 2020 21:18:34 -03
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
#include <celaeno/fun/fun.hpp>
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
#include <celaeno/graph/representations/grid.hpp>

// namespace celaeno::graph::draw::svg {{{
namespace celaeno::graph::draw::svg
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
namespace ns_representations = celaeno::graph::representations;
namespace ns_search = celaeno::graph::search;
namespace ns_views = celaeno::graph::views;
// }}}

// Using namespaces {{{
using namespace celaeno::concepts;
using namespace celaeno::aliases;
using namespace celaeno::fun::fn;
// }}}

// function: runtime {{{
template<typename F>
decltype(auto) runtime(F&& f_test)
{
  auto start {std::chrono::system_clock::now()};
  auto result{f_test()};
  auto end {std::chrono::system_clock::now()};
  std::chrono::duration<f64> dur {end-start};
  std::stringstream ss; ss << dur.count();
  return std::make_pair(result,ss.str());
} // function: runtime }}}

// Shapes {{{
constexpr std::string_view const a_head
{
  "<defs>"
  "  <marker id='arrowhead' markerWidth='5' markerHeight='4'" 
  "  refX='0' refY='1.75' orient='auto'>"
  "    <polygon points='0 0, 5 1.75, 0 3.5' fill='red' />"
  "  </marker>"
  "</defs>\n"
};

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
  "<circle stroke='{}' stroke-width='1px' r='{}' cx='{}' cy='{}' fill='{}'/>\n",
};

constexpr std::string_view const e_template_marker
{
  "<line x1 = '{}' y1 = '{}' x2 = '{}' y2 = '{}' "
  "stroke = 'black' stroke-width = '1' marker-end='url(#arrowhead)'/>\n"
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
template<String S, typename Map, typename Paths, typename F>
void svg(Ops const& ops, S&& filename, Map&& vertex_tile, Paths&& paths, F&& f_label)
{
  // Output file
  std::ofstream of{filename};

  // Check if map is not empty
  if ( vertex_tile.empty() )
  {
    spdlog::error("{}@{} Attempt to draw empty vertex map", __FILE__,__LINE__);
    exit(1);
  } // if

  // Check output file state
  if ( ! of.good())
  {
    spdlog::error("{}@{} Not possible to create file {}", __FILE__,__LINE__,filename);
    exit(1);
  } // if

  // Streams
  std::stringstream header;
  std::stringstream vertices;
  std::stringstream edges;

  //Get farthest vertices to build viewport
  auto min_box_x {rg::min_element(vertex_tile,{},
  [](auto e) { return e.second.first; })->second.first};

  auto min_box_y {rg::min_element(vertex_tile,{},
  [](auto e) { return e.second.second; })->second.second};

  for (auto& [v,t] : vertex_tile)
  {
    t.first += std::abs(min_box_x);
    t.second += std::abs(min_box_y);
  } // for

  auto view_box_x = rg::max_element(vertex_tile,{}, [](auto e) { return e.second.first; })->second.first;
  auto view_box_y = rg::max_element(vertex_tile,{}, [](auto e) { return e.second.second; })->second.second;

  // Adjust positions for drawing
  for (auto& [_,tile] : vertex_tile)
  {
    tile.first = tile.first*tile_size+circle_offset;
    tile.second = tile.second*tile_size+circle_offset;
  } // for

  for (auto&& [pair,path] : paths)
  {
    size_t size_path{path.size()};

    fn(path).slide(2).ply([&,i=0](auto&& r) mutable
    {
      auto&& source{r.front()};
      auto&& dest{r.back()};

      auto [ux,uy] = source;
      auto [vx,vy] = dest;

      // Update values for drawing
      ux = ux*tile_size+circle_offset;
      uy = uy*tile_size+circle_offset;
      vx = vx*tile_size+circle_offset;
      vy = vy*tile_size+circle_offset;

      // Tail offset to fit between dest and vertex
      i64 offset{tile_size/2 - (tile_size/2-vertex_radius)};
      i64 marker_width{5};
      i64 marker_height{4};

      if( i == 0 )
      {
        if( ux > vx )
        {
          ux -= offset;
        }
        else if( ux < vx )
        {
          ux += offset;
        }

        if( vy < uy )
        {
          uy -= offset;
        }
        else if( vy > uy )
        {
          uy += offset;
        }

      } // if

      // -2 because slide is equal to (n-1) iterations on container
      if (i == (size_path-2))
      {
        if( ux > vx )
        {
          vx += offset + marker_width;
        }
        else if( ux < vx )
        {
          vx -= offset + marker_width;
        }

        if( vy < uy )
        {
          vy += offset + marker_height;
        }
        else if( vy > uy )
        {
          vy -= offset + marker_height;
        }
      } // if

      if( i == (size_path-2) )
      {
        edges << fmt::format(e_template_marker, ux, uy, vx, vy);
      } // if
      else
      {
        edges << fmt::format(e_template, ux, uy, vx, vy);
      } // else

      ++i;

    }).discard();
  } // for

  // @ Stream/File writting
  header << fmt::format(h_template,view_box_x*tile_size+circle_offset*2, view_box_y*tile_size+circle_offset*2);

  // Vertices and labels
  for (auto [v,tile] : vertex_tile)
  {
    auto [x,y] = std::make_pair(tile.first,tile.second);
    // Draw pseudo-nodes with black filling
    if (v < 0)
    {
      vertices << fmt::format(v_template, "white", vertex_radius, x, y, "black");
      vertices << fmt::format(v_label_template, x, y, 10, "white", f_label(v));
    } // if
    else
    {
      vertices << fmt::format(v_template, "black", vertex_radius, x, y, "white");
      vertices << fmt::format(v_label_template, x, y, 10, "black", f_label(v));
    } // else
  } // for

  // File writting
  of << fmt::format("{}\n", header.str());

  // Arrow head
  of << a_head;

  // Draw grid
  std::stringstream tiles;
  for (i64 x{}; x <= view_box_x; ++x)
  {
    for (i64 y{}; y <= view_box_y; ++y)
    {
      constexpr std::string_view tile
      {
        "<rect x='{}' y='{}' width='{}' height='{}' fill='none' stroke='gray' stroke-width='1'/>\n"
      };

      tiles << fmt::format(tile
        , x*tile_size
        , y*tile_size
        , tile_size
        , tile_size
      );
    } // for
  } // for

  of << fmt::format("{}\n", tiles.str());
  of << fmt::format("{}\n", vertices.str());
  of << fmt::format("{}\n", edges.str());
  of << fmt::format("{}\n", footer);
  of.close();
} // function: svg }}}

// // fn: run  {{{
// template<SignedIntegral S, typename L, String Str>
// decltype(auto) run(S root, Ops ops, L&& f_label, Str&& fn)
// {
//
// #if ! defined(NDEBUG) && defined(DEBUG_SHOW_ALG)
//   spdlog::set_level(spdlog::level::debug);
//   spdlog::debug("Algorithm: celaeno::graph::draw::svg");
// #endif
//
// #ifdef DEBUG_SVG_HPP
//   // Create logging sink
//   auto logger{spdlog::basic_logger_mt("celaeno::graph::draw::svg", "logs.txt")};
// #endif
//
//   //
//   // Pre-processing
//   //
//   ns_balance::paths::run(root,ops);
//   ns_balance::outgoing::run(root,ops);
//   ns_balance::paths::run(root,ops);
//   // ns_minimize::pseudo::run(root,ops);
//
//   //
//   // Layer ordering
//   //
//   // auto layers {ns_minimize::crossings::run(root,f_nop,f_succ,f_adj,f_link,f_unlink)};
//   auto depth_view{ns_views::depth::run(root, ops.preds, ops.succs).ln};
//   auto layers {fp::get_map_values(depth_view)};
//
//   //
//   // Tile Placement and Edge Routing
//   //
// #ifdef DEBUG_SVG_HPP
//   // Placement
//   auto [vertex_tile,time_placement]{runtime(
//     [&]{ return ns_representations::grid::run(root, ops, layers);
//   })};
//
//   // Routing
//   auto [paths,time_routing]{runtime(
//     [&,vertex_tile=vertex_tile]{return route(ops, vertex_tile, layers);}
//   )};
//
//   // Area
//   auto [x,y] = ns_representations::grid::area(vertex_tile);
//
//   // Logging
//   logger->info("File: {}", fn);
//   logger->info("Placement Time: {}", time_placement);
//   logger->info("Routing Time: {}", time_routing);
//   logger->info("Area: {}x{}", x,y);
// #else
//   auto vertex_tile {ns_representations::grid::run(root, ops, layers)};
//   auto paths{route(ops, vertex_tile, layers)};
// #endif
//
// #ifndef DEBUG_DISABLE_OUTPUT
//   // Write output .svg file
//   svg(fmt::format("{}.svg", fn), vertex_tile, paths, f_label);
// #endif // DEBUG_DISABLE_OUTPUT
//
// } // function: run }}}

} // namespace celaeno::graph::draw::svg }}}
