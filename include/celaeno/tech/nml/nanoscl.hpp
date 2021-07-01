// vim: set expandtab fdm=marker ts=2 sw=2 tw=80 et :
//
// @author      : Ruan E. Formigoni (ruanformigoni@gmail.com)
// @file        : nanoscl
// @created     : Wednesday Jun 23, 2021 17:40:07 UTC
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
#include <string_view>
#include <sstream>
#include <range/v3/all.hpp>
#include <fplus/fplus.hpp>

#include <celaeno/aliases.hpp>
#include <celaeno/concepts.hpp>
#include <celaeno/graph/reader/verilog.hpp>
#include <celaeno/tech/clocking-scheme/twoddwave.hpp>
#include <celaeno/graph/views/depth.hpp>
#include <celaeno/graph/operations/balance/paths.hpp>
#include <celaeno/graph/operations/balance/outgoing.hpp>
#include <celaeno/graph/representations/grid.hpp>

// TODO REMOVE
#include <celaeno/graph/draw/svg.hpp>
#include <celaeno/graph/search/bfs.hpp>

namespace celaeno::tech::nml::nanoscl
{

// Using declarations {{{
using Ops = celaeno::graph::Ops;
using GateType = celaeno::graph::reader::verilog::GateType;
// }}}

// using namespaces {{{
using namespace celaeno::aliases;
using namespace celaeno::concepts;
// }}}

// namespaces {{{
namespace rg = ranges;
namespace rv = ranges::views;
namespace fp = fplus;

namespace ns_scheme = celaeno::tech::clock_scheme;
namespace ns_search = celaeno::graph::search;
namespace ns_views = celaeno::graph::views;
namespace ns_grid = celaeno::graph::representations::grid;
namespace ns_balance = celaeno::graph::operations::balance;
namespace ns_representations = celaeno::graph::representations;
// }}}

// Constants {{{
i32 const TILE_SIZE{3};
// }}}

// Using declarations {{{
using Tile = celaeno::graph::representations::grid::Tile;
using TileType = celaeno::graph::representations::grid::TileType;
using MapVertexTile = celaeno::graph::representations::grid::MapVertexTile;
using Scheme = ns_scheme::TwoddWave;
// }}}

// namespace: cells {{{
namespace cells
{
  // fn: i_d {{{
  auto i_d(f64 x, f64 y)
  {
    return fmt::format(
      "    {{\n"
      "        \"clock_zone\": {6},\n"
      "        \"fixed_magnetization\": false,\n"
      "        \"id\": \"I\",\n"
      "        \"logic\": \"normal\",\n"
      "        \"magnetization\": 0.0,\n"
      "        \"type\": \"input\",\n"
      "        \"x\": {0},\n"
      "        \"y\": {1}\n"
      "    }},\n"
      "    {{\n"
      "        \"clock_zone\": {6},\n"
      "        \"fixed_magnetization\": false,\n"
      "        \"id\": \"\",\n"
      "        \"logic\": \"normal\",\n"
      "        \"magnetization\": 0.0,\n"
      "        \"type\": \"regular\",\n"
      "        \"x\": {2},\n"
      "        \"y\": {3}\n"
      "    }},\n"
      "    {{\n"
      "        \"clock_zone\": {6},\n"
      "        \"fixed_magnetization\": false,\n"
      "        \"id\": \"\",\n"
      "        \"logic\": \"normal\",\n"
      "        \"magnetization\": 0.0,\n"
      "        \"type\": \"regular\",\n"
      "        \"x\": {4},\n"
      "        \"y\": {5}\n"
      "    }},\n"
      , x+1, y, x+1, y+1, x+1, y+2, Scheme{}(x/TILE_SIZE,y/TILE_SIZE));
  } // }}}

  // fn: i_rd {{{
  auto i_rd(f64 x, f64 y)
  {
    return fmt::format(
      "    {{\n"
      "        \"clock_zone\": {6},\n"
      "        \"fixed_magnetization\": false,\n"
      "        \"id\": \"I\",\n"
      "        \"logic\": \"normal\",\n"
      "        \"magnetization\": 0.0,\n"
      "        \"type\": \"input\",\n"
      "        \"x\": {0},\n"
      "        \"y\": {1}\n"
      "    }},\n"
      "    {{\n"
      "        \"clock_zone\": {6},\n"
      "        \"fixed_magnetization\": false,\n"
      "        \"id\": \"\",\n"
      "        \"logic\": \"normal\",\n"
      "        \"magnetization\": 0.0,\n"
      "        \"type\": \"regular\",\n"
      "        \"x\": {2},\n"
      "        \"y\": {3}\n"
      "    }},\n"
      "    {{\n"
      "        \"clock_zone\": {6},\n"
      "        \"fixed_magnetization\": false,\n"
      "        \"id\": \"\",\n"
      "        \"logic\": \"normal\",\n"
      "        \"magnetization\": 0.0,\n"
      "        \"type\": \"regular\",\n"
      "        \"x\": {4},\n"
      "        \"y\": {5}\n"
      "    }},\n"
    , x+1, y+1, x+2, y+1, x+1, y+2, Scheme{}(x/TILE_SIZE,y/TILE_SIZE));
  } // }}}

  // fn: g_and {{{
  auto g_and(f64 x, f64 y)
  {
    return fmt::format(
      "    {{\n"
      "        \"magnetization\": 0.0,\n"
      "        \"clock_zone\": {10},\n"
      "        \"x\": {0},\n"
      "        \"fixed_magnetization\": false,\n"
      "        \"y\": {1},\n"
      "        \"logic\": \"normal\",\n"
      "        \"id\": \"O\",\n"
      "        \"type\": \"output\"\n"
      "    }},\n"
      "    {{\n"
      "        \"magnetization\": 0.0,\n"
      "        \"clock_zone\": {10},\n"
      "        \"x\": {2},\n"
      "        \"fixed_magnetization\": false,\n"
      "        \"y\": {3},\n"
      "        \"logic\": \"normal\",\n"
      "        \"id\": \"\",\n"
      "        \"type\": \"regular\"\n"
      "    }},\n"
      "    {{\n"
      "        \"magnetization\": -1.0,\n"
      "        \"clock_zone\": {10},\n"
      "        \"x\": {4},\n"
      "        \"fixed_magnetization\": true,\n"
      "        \"y\": {5},\n"
      "        \"logic\": \"normal\",\n"
      "        \"id\": \"\",\n"
      "        \"type\": \"regular\"\n"
      "    }},\n"
      "    {{\n"
      "        \"magnetization\": 0.0,\n"
      "        \"clock_zone\": {10},\n"
      "        \"x\": {6},\n"
      "        \"fixed_magnetization\": false,\n"
      "        \"y\": {7},\n"
      "        \"logic\": \"normal\",\n"
      "        \"id\": \"\",\n"
      "        \"type\": \"regular\"\n"
      "    }},\n"
      "    {{\n"
      "        \"magnetization\": 0.0,\n"
      "        \"clock_zone\": {10},\n"
      "        \"x\": {8},\n"
      "        \"fixed_magnetization\": false,\n"
      "        \"y\": {9},\n"
      "        \"logic\": \"normal\",\n"
      "        \"id\": \"\",\n"
      "        \"type\": \"regular\"\n"
      "    }},\n"
      , x+2, y+2, x, y, x+2, y, x+1, y+1, x, y+2, Scheme{}(x/TILE_SIZE,y/TILE_SIZE));
  } // }}}

  // fn: g_or {{{
  auto g_or(f64 x, f64 y)
  {
    return fmt::format(
      "    {{\n"
      "        \"magnetization\": 0.0,\n"
      "        \"clock_zone\": {10},\n"
      "        \"x\": {0},\n"
      "        \"fixed_magnetization\": false,\n"
      "        \"y\": {1},\n"
      "        \"logic\": \"normal\",\n"
      "        \"id\": \"O\",\n"
      "        \"type\": \"output\"\n"
      "    }},\n"
      "    {{\n"
      "        \"magnetization\": 0.0,\n"
      "        \"clock_zone\": {10},\n"
      "        \"x\": {2},\n"
      "        \"fixed_magnetization\": false,\n"
      "        \"y\": {3},\n"
      "        \"logic\": \"normal\",\n"
      "        \"id\": \"\",\n"
      "        \"type\": \"regular\"\n"
      "    }},\n"
      "    {{\n"
      "        \"magnetization\": 1.0,\n"
      "        \"clock_zone\": {10},\n"
      "        \"x\": {4},\n"
      "        \"fixed_magnetization\": true,\n"
      "        \"y\": {5},\n"
      "        \"logic\": \"normal\",\n"
      "        \"id\": \"\",\n"
      "        \"type\": \"regular\"\n"
      "    }},\n"
      "    {{\n"
      "        \"magnetization\": 0.0,\n"
      "        \"clock_zone\": {10},\n"
      "        \"x\": {6},\n"
      "        \"fixed_magnetization\": false,\n"
      "        \"y\": {7},\n"
      "        \"logic\": \"normal\",\n"
      "        \"id\": \"\",\n"
      "        \"type\": \"regular\"\n"
      "    }},\n"
      "    {{\n"
      "        \"magnetization\": 0.0,\n"
      "        \"clock_zone\": {10},\n"
      "        \"x\": {8},\n"
      "        \"fixed_magnetization\": false,\n"
      "        \"y\": {9},\n"
      "        \"logic\": \"normal\",\n"
      "        \"id\": \"\",\n"
      "        \"type\": \"regular\"\n"
      "    }},\n"
      , x+2, y+2, x, y, x+2, y, x+1, y+1, x, y+2, Scheme{}(x/TILE_SIZE,y/TILE_SIZE));
  } // }}}

  // fn: w_ld {{{
  auto w_ld(f64 x, f64 y)
  {
    return fmt::format(
      "    {{\n"
      "        \"magnetization\": 0.0,\n"
      "        \"clock_zone\": {6},\n"
      "        \"x\": {0},\n"
      "        \"fixed_magnetization\": false,\n"
      "        \"y\": {1},\n"
      "        \"logic\": \"normal\",\n"
      "        \"id\": \"\",\n"
      "        \"type\": \"regular\"\n"
      "    }},\n"
      "    {{\n"
      "        \"magnetization\": 0.0,\n"
      "        \"clock_zone\": {6},\n"
      "        \"x\": {2},\n"
      "        \"fixed_magnetization\": false,\n"
      "        \"y\": {3},\n"
      "        \"logic\": \"normal\",\n"
      "        \"id\": \"\",\n"
      "        \"type\": \"regular\"\n"
      "    }},\n"
      "    {{\n"
      "        \"magnetization\": 0.0,\n"
      "        \"clock_zone\": {6},\n"
      "        \"x\": {4},\n"
      "        \"fixed_magnetization\": false,\n"
      "        \"y\": {5},\n"
      "        \"logic\": \"normal\",\n"
      "        \"id\": \"\",\n"
      "        \"type\": \"regular\"\n"
      "    }},\n"
      , x, y+1, x+1, y+1, x+1, y+2, Scheme{}(x/TILE_SIZE,y/TILE_SIZE));
  } // }}}

  // fn: w_lr {{{
  auto w_lr(f64 x, f64 y)
  {
    return fmt::format(
      "    {{\n"
      "        \"magnetization\": 0.0,\n"
      "        \"clock_zone\": {6},\n"
      "        \"x\": {0},\n"
      "        \"fixed_magnetization\": false,\n"
      "        \"y\": {1},\n"
      "        \"logic\": \"normal\",\n"
      "        \"id\": \"\",\n"
      "        \"type\": \"regular\"\n"
      "    }},\n"
      "    {{\n"
      "        \"magnetization\": 0.0,\n"
      "        \"clock_zone\": {6},\n"
      "        \"x\": {2},\n"
      "        \"fixed_magnetization\": false,\n"
      "        \"y\": {3},\n"
      "        \"logic\": \"normal\",\n"
      "        \"id\": \"\",\n"
      "        \"type\": \"regular\"\n"
      "    }},\n"
      "    {{\n"
      "        \"magnetization\": 0.0,\n"
      "        \"clock_zone\": {6},\n"
      "        \"x\": {4},\n"
      "        \"fixed_magnetization\": false,\n"
      "        \"y\": {5},\n"
      "        \"logic\": \"normal\",\n"
      "        \"id\": \"\",\n"
      "        \"type\": \"regular\"\n"
      "    }},\n"
      , x, y+1, x+1, y+1, x+2, y+1, Scheme{}(x/TILE_SIZE,y/TILE_SIZE));
  } // }}}

  // fn: w_ud {{{
  auto w_ud(f64 x, f64 y)
  {
    return fmt::format(
        "    {{\n"
        "        \"magnetization\": 0.0,\n"
        "        \"clock_zone\": {6},\n"
        "        \"x\": {0},\n"
        "        \"fixed_magnetization\": false,\n"
        "        \"y\": {1},\n"
        "        \"logic\": \"normal\",\n"
        "        \"id\": \"\",\n"
        "        \"type\": \"regular\"\n"
        "    }},\n"
        "    {{\n"
        "        \"magnetization\": 0.0,\n"
        "        \"clock_zone\": {6},\n"
        "        \"x\": {2},\n"
        "        \"fixed_magnetization\": false,\n"
        "        \"y\": {3},\n"
        "        \"logic\": \"normal\",\n"
        "        \"id\": \"\",\n"
        "        \"type\": \"regular\"\n"
        "    }},\n"
        "    {{\n"
        "        \"magnetization\": 0.0,\n"
        "        \"clock_zone\": {6},\n"
        "        \"x\": {4},\n"
        "        \"fixed_magnetization\": false,\n"
        "        \"y\": {5},\n"
        "        \"logic\": \"normal\",\n"
        "        \"id\": \"\",\n"
        "        \"type\": \"regular\"\n"
        "    }},\n"
    , x+1, y, x+1, y+1, x+1, y+2, Scheme{}(x/TILE_SIZE,y/TILE_SIZE));
  } // }}}

  // fn: w_ur {{{
  auto w_ur(f64 x, f64 y)
  {
    return fmt::format(
    "{{\n"
    "    \"clock_zone\": {6},\n"
    "    \"fixed_magnetization\": false,\n"
    "    \"id\": \"\",\n"
    "    \"logic\": \"normal\",\n"
    "    \"magnetization\": 0.0,\n"
    "    \"type\": \"regular\",\n"
    "    \"x\": {0},\n"
    "    \"y\": {1}\n"
    "}},\n"
    "{{\n"
    "    \"clock_zone\": {6},\n"
    "    \"fixed_magnetization\": false,\n"
    "    \"id\": \"\",\n"
    "    \"logic\": \"normal\",\n"
    "    \"magnetization\": 0.0,\n"
    "    \"type\": \"regular\",\n"
    "    \"x\": {2},\n"
    "    \"y\": {3}\n"
    "}},\n"
    "{{\n"
    "    \"clock_zone\": {6},\n"
    "    \"fixed_magnetization\": false,\n"
    "    \"id\": \"\",\n"
    "    \"logic\": \"normal\",\n"
    "    \"magnetization\": 0.0,\n"
    "    \"type\": \"regular\",\n"
    "    \"x\": {4},\n"
    "    \"y\": {5}\n"
    "}},\n"
    , x+1, y, x+1, y+1, x+2, y+1, Scheme{}(x/TILE_SIZE,y/TILE_SIZE));
  } // }}}

  // fn: w_urd {{{
  auto w_urd(f64 x, f64 y)
  {
    return fmt::format(
      "    {{\n"
      "        \"magnetization\": 0.0,\n"
      "        \"clock_zone\": {8},\n"
      "        \"x\": {0},\n"
      "        \"fixed_magnetization\": false,\n"
      "        \"y\": {1},\n"
      "        \"logic\": \"normal\",\n"
      "        \"id\": \"\",\n"
      "        \"type\": \"regular\"\n"
      "    }},\n"
      "    {{\n"
      "        \"magnetization\": 0.0,\n"
      "        \"clock_zone\": {8},\n"
      "        \"x\": {2},\n"
      "        \"fixed_magnetization\": false,\n"
      "        \"y\": {3},\n"
      "        \"logic\": \"normal\",\n"
      "        \"id\": \"\",\n"
      "        \"type\": \"regular\"\n"
      "    }},\n"
      "    {{\n"
      "        \"magnetization\": 0.0,\n"
      "        \"clock_zone\": {8},\n"
      "        \"x\": {4},\n"
      "        \"fixed_magnetization\": false,\n"
      "        \"y\": {5},\n"
      "        \"logic\": \"normal\",\n"
      "        \"id\": \"\",\n"
      "        \"type\": \"regular\"\n"
      "    }},\n"
      "    {{\n"
      "        \"magnetization\": 0.0,\n"
      "        \"clock_zone\": {8},\n"
      "        \"x\": {6},\n"
      "        \"fixed_magnetization\": false,\n"
      "        \"y\": {7},\n"
      "        \"logic\": \"normal\",\n"
      "        \"id\": \"\",\n"
      "        \"type\": \"regular\"\n"
      "    }},\n"
    , x+1, y, x+1, y+1, x+2, y+1, x+1, y+2, Scheme{}(x/TILE_SIZE,y/TILE_SIZE));
  } // }}}
}
// }}}

// class: NanoScl {{{
class NanoScl
{
  private:
    std::stringstream buf;
  public:
    template<typename F, String Str>
    NanoScl(MapVertexTile const& m_vertex_tile, F&& f_gate_type, Str&& fn);
}; // class: NanoScl

template<typename F, String Str>
NanoScl::NanoScl(MapVertexTile const& m_vertex_tile, F&& f_gate_type, Str&& fn)
  : buf({})
{
  auto [area_x,area_y] = ns_grid::area(m_vertex_tile);

  this->buf <<
    fmt::format(
    "[\n"
    "    {{\n"
    "        \"numberY\": {},\n"
    "        \"numberX\": {}\n"
    "    }},\n"
    , area_x*TILE_SIZE*2, area_y*TILE_SIZE*2);

  for (auto const& [u,tile] : m_vertex_tile)
  {
    auto [x,y] = std::make_pair(tile.x*TILE_SIZE,tile.y*TILE_SIZE);

    // Is gate
    if( u >= 0 )
    {
      switch (f_gate_type(u))
      {
        case GateType::INPUT:
          if( tile.type == TileType::ID )
          {
            this->buf << cells::i_d(x,y);
          } // if
          else if( tile.type == TileType::IDR )
          {
            this->buf << cells::i_rd(x,y);
          } // else
          else
          {
            spdlog::error("{}@{}: Invalid tile type for node {}\n", __FILE__, __LINE__, u);
            exit(1);
          } // else
          break;
        case GateType::AND:
          this->buf << cells::g_and(x,y);
          break;
        case GateType::OR:
          this->buf << cells::g_or(x,y);
          break;
        case GateType::NAND:
        case GateType::NOR:
        case GateType::XOR:
        case GateType::XNOR:
        case GateType::MAJ3:
          break;
      } // switch
    } // if
    // Is wire
    else
    {
      switch (tile.type)
      {
        case TileType::LD:
          this->buf << cells::w_ld(x,y);
          break;
        case TileType::LR:
          this->buf << cells::w_lr(x,y);
          break;
        case TileType::UD:
          this->buf << cells::w_ud(x,y);
          break;
        case TileType::UR:
          this->buf << cells::w_ur(x,y);
          break;
        case TileType::URD:
          this->buf << cells::w_urd(x,y);
          break;
        case TileType::ND:
        case TileType::ID:
        case TileType::IDR:
          spdlog::error("{}@{}: Network nodes must not have a negative id", __FILE__, __LINE__);
          exit(1);
      } // switch
    } // else
  } // for

  this->buf << "\n]";

  // Write file
  if (std::ofstream of{fn,std::ios::trunc}; ! of.good())
  {
    spdlog::error("{}@{} Not possible to create file {}", __FILE__,__LINE__,fn);
    exit(1);
  } // if
  else
  {
    of << this->buf.str();
    of.close();
  } // else

}
// }}}

// fn: run {{{
template<SignedIntegral S, typename L, String Str>
decltype(auto) run(S root, Ops ops, L&& f_gate_type, Str&& fn)
{

#if ! defined(NDEBUG) && defined(DEBUG_SHOW_ALG)
  spdlog::set_level(spdlog::level::debug);
  spdlog::debug("Algorithm: celaeno::tech::nml::nanoscl");
#endif

#ifdef DEBUG_SVG_HPP
  // Create logging sink
  auto logger{spdlog::basic_logger_mt("celaeno::tech::nml::nanoscl", "logs.txt")};
#endif

  //
  // Pre-processing
  //
  ns_balance::paths::run(root,ops);
  ns_balance::outgoing::run(root,ops);
  ns_balance::paths::run(root,ops);
  // ns_minimize::pseudo::run(root,ops);

  //
  // Layer ordering
  //
  // auto layers {ns_minimize::crossings::run(root,f_nop,f_succ,f_adj,f_link,f_unlink)};
  auto depth_view{ns_views::depth::run(root, ops.preds, ops.succs).first};
  auto layers = depth_view
    | rv::transform([](auto&& e){ return e.second; })
    | rg::to<std::vector<std::vector<i64>>>;

  //
  // Tile Placement and Edge Routing
  //
  auto m_vertex_tile {ns_representations::grid::run(root, ops, layers)};

  // TODO Remove
  // Pseudo number index
  i64 counter{};

  // Define function to compare values lt 0
  auto f_lowest = [&counter](auto&& e) { if(e < counter){ counter=e; } return false; };

  // Get the dummy vertex with the lowest value
  ns_search::bfs::run(root,ops.preds,ops.succs,f_lowest);

  // A struct to control the value of pseudo nodes
  auto f_make_pseudo = [counter]
  {
    struct Pseudo
    {
      private:
        i64 c;
      public:
        Pseudo(i64 _c) : c(_c) {}
        i64 next(){ return --c; }
        i64 curr(){ return c; }
    };
    return Pseudo{counter};
  }();

  std::map<i64,Tile> m;

  auto m_tiles_route {ns_representations::grid::route(ops, m_vertex_tile, layers)};

  for (std::set<Tile> visited; auto const& [tiles,route] : m_tiles_route)
  {
    for (auto it{route.begin()}; it != route.end(); ++it)
    {
      // Ignore first and last positions
      if( it == route.begin() || std::next(it) == route.end() ){ continue; }

      // Get current and adjacent positions
      auto [x,y] = *it;
      auto [px,py] = *std::prev(it);
      auto [nx,ny] = *std::next(it);

      // Check which have changed

      // Set TileType
      // Vertical wire
      if( (px == x) && (x == nx ) )
      {
        m_vertex_tile.emplace(f_make_pseudo.next(),Tile(x,y,TileType::UD));
      } // else if

      // Horizontal wire
      else if( (py == y) && (y == ny) )
      {
        m_vertex_tile.emplace(f_make_pseudo.next(),Tile(x,y,TileType::LR));
      } // else if

      // Up to right wire
      else if( (py < y) && (nx > x) )
      {
        m_vertex_tile.emplace(f_make_pseudo.next(),Tile(x,y,TileType::UR));
      } // else if

      else
      {
        m_vertex_tile.emplace(f_make_pseudo.next(),Tile(x,y,TileType::LD));
      } // else
      // Left to down wire

    } // for
  } // for


  //
  // Tile Mapping
  //
  NanoScl mapping(m_vertex_tile,f_gate_type,fmt::format("{}.json", fn));

  celaeno::graph::draw::svg::svg(fmt::format("{}.svg", fn)
    , m_vertex_tile
    , m_tiles_route
    , [](auto){ return " "; }
  );

} // function: run }}}

} // namespace celaeno::tech::nml::nanoscl
