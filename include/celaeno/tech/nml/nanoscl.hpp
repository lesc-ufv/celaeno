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
#include <celaeno/graph/views/depth.hpp>
#include <celaeno/graph/operations/balance/paths.hpp>
#include <celaeno/graph/operations/balance/outgoing.hpp>
#include <celaeno/graph/representations/grid.hpp>

// TODO REMOVE
#include <celaeno/graph/draw/svg.hpp>

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

namespace ns_views = celaeno::graph::views;
namespace ns_grid = celaeno::graph::representations::grid;
namespace ns_balance = celaeno::graph::operations::balance;
namespace ns_representations = celaeno::graph::representations;
// }}}

// Using declarations {{{
using TileType = celaeno::graph::representations::grid::TileType;
using MapVertexTile = celaeno::graph::representations::grid::MapVertexTile;
// }}}

// namespace: cells {{{
namespace cells
{
  // fn: i_d {{{
  auto i_d(f64 x, f64 y)
  {
    return fmt::format(
      "    {{\n"
      "        \"magnetization\": 0.0,\n"
      "        \"clock_zone\": 0,\n"
      "        \"x\": {},\n"
      "        \"fixed_magnetization\": false,\n"
      "        \"y\": {},\n"
      "        \"logic\": \"normal\",\n"
      "        \"id\": \"I\",\n"
      "        \"type\": \"input\"\n"
      "    }},\n"
      "    {{\n"
      "        \"magnetization\": 0.0,\n"
      "        \"clock_zone\": 0,\n"
      "        \"x\": {},\n"
      "        \"fixed_magnetization\": false,\n"
      "        \"y\": {},\n"
      "        \"logic\": \"normal\",\n"
      "        \"id\": \"\",\n"
      "        \"type\": \"regular\"\n"
      "    }}\n"
      , x+1, y+1, x+1, y+2);
  } // }}}

  // fn: i_rd {{{
  auto i_rd(f64 x, f64 y)
  {
    return fmt::format(
      "    {{\n"
      "        \"magnetization\": 0.0,\n"
      "        \"clock_zone\": 0,\n"
      "        \"x\": {},\n"
      "        \"fixed_magnetization\": false,\n"
      "        \"y\": {},\n"
      "        \"logic\": \"normal\",\n"
      "        \"id\": \"I\",\n"
      "        \"type\": \"input\"\n"
      "    }},\n"
      "    {{\n"
      "        \"magnetization\": 0.0,\n"
      "        \"clock_zone\": 0,\n"
      "        \"x\": {},\n"
      "        \"fixed_magnetization\": false,\n"
      "        \"y\": {},\n"
      "        \"logic\": \"normal\",\n"
      "        \"id\": \"\",\n"
      "        \"type\": \"regular\"\n"
      "    }},\n"
      "    {{\n"
      "        \"magnetization\": 0.0,\n"
      "        \"clock_zone\": 0,\n"
      "        \"x\": {},\n"
      "        \"fixed_magnetization\": false,\n"
      "        \"y\": {},\n"
      "        \"logic\": \"normal\",\n"
      "        \"id\": \"\",\n"
      "        \"type\": \"regular\"\n"
      "    }}\n"
    , x+1, y+1, x+2, y+1, x+1, y+2);
  } // }}}

  // fn: g_and {{{
  auto g_and(f64 x, f64 y)
  {
    return fmt::format(
      "    {{\n"
      "        \"magnetization\": 0.0,\n"
      "        \"clock_zone\": 0,\n"
      "        \"x\": {},\n"
      "        \"fixed_magnetization\": false,\n"
      "        \"y\": {},\n"
      "        \"logic\": \"normal\",\n"
      "        \"id\": \"O\",\n"
      "        \"type\": \"output\"\n"
      "    }},\n"
      "    {{\n"
      "        \"magnetization\": 0.0,\n"
      "        \"clock_zone\": 0,\n"
      "        \"x\": {},\n"
      "        \"fixed_magnetization\": false,\n"
      "        \"y\": {},\n"
      "        \"logic\": \"normal\",\n"
      "        \"id\": \"\",\n"
      "        \"type\": \"regular\"\n"
      "    }},\n"
      "    {{\n"
      "        \"magnetization\": -1.0,\n"
      "        \"clock_zone\": 0,\n"
      "        \"x\": {},\n"
      "        \"fixed_magnetization\": true,\n"
      "        \"y\": {},\n"
      "        \"logic\": \"normal\",\n"
      "        \"id\": \"\",\n"
      "        \"type\": \"regular\"\n"
      "    }},\n"
      "    {{\n"
      "        \"magnetization\": 0.0,\n"
      "        \"clock_zone\": 0,\n"
      "        \"x\": {},\n"
      "        \"fixed_magnetization\": false,\n"
      "        \"y\": {},\n"
      "        \"logic\": \"normal\",\n"
      "        \"id\": \"\",\n"
      "        \"type\": \"regular\"\n"
      "    }},\n"
      "    {{\n"
      "        \"magnetization\": 0.0,\n"
      "        \"clock_zone\": 0,\n"
      "        \"x\": {},\n"
      "        \"fixed_magnetization\": false,\n"
      "        \"y\": {},\n"
      "        \"logic\": \"normal\",\n"
      "        \"id\": \"\",\n"
      "        \"type\": \"regular\"\n"
      "    }},\n"
      , x+2, y+2, x, y, x+2, y, x+1, y+1, x, y+2);
  } // }}}

  // fn: g_or {{{
  auto g_or(f64 x, f64 y)
  {
    return fmt::format(
      "    {{\n"
      "        \"magnetization\": 0.0,\n"
      "        \"clock_zone\": 0,\n"
      "        \"x\": {},\n"
      "        \"fixed_magnetization\": false,\n"
      "        \"y\": {},\n"
      "        \"logic\": \"normal\",\n"
      "        \"id\": \"O\",\n"
      "        \"type\": \"output\"\n"
      "    }},\n"
      "    {{\n"
      "        \"magnetization\": 0.0,\n"
      "        \"clock_zone\": 0,\n"
      "        \"x\": {},\n"
      "        \"fixed_magnetization\": false,\n"
      "        \"y\": {},\n"
      "        \"logic\": \"normal\",\n"
      "        \"id\": \"\",\n"
      "        \"type\": \"regular\"\n"
      "    }},\n"
      "    {{\n"
      "        \"magnetization\": 1.0,\n"
      "        \"clock_zone\": 0,\n"
      "        \"x\": {},\n"
      "        \"fixed_magnetization\": true,\n"
      "        \"y\": {},\n"
      "        \"logic\": \"normal\",\n"
      "        \"id\": \"\",\n"
      "        \"type\": \"regular\"\n"
      "    }},\n"
      "    {{\n"
      "        \"magnetization\": 0.0,\n"
      "        \"clock_zone\": 0,\n"
      "        \"x\": {},\n"
      "        \"fixed_magnetization\": false,\n"
      "        \"y\": {},\n"
      "        \"logic\": \"normal\",\n"
      "        \"id\": \"\",\n"
      "        \"type\": \"regular\"\n"
      "    }},\n"
      "    {{\n"
      "        \"magnetization\": 0.0,\n"
      "        \"clock_zone\": 0,\n"
      "        \"x\": {},\n"
      "        \"fixed_magnetization\": false,\n"
      "        \"y\": {},\n"
      "        \"logic\": \"normal\",\n"
      "        \"id\": \"\",\n"
      "        \"type\": \"regular\"\n"
      "    }},\n"
      , x+2, y+2, x, y, x+2, y, x+1, y+1, x, y+2);
  } // }}}

  // fn: w_ld {{{
  auto w_ld(f64 x, f64 y)
  {
    return fmt::format(
      "    {{\n"
      "        \"magnetization\": 0.0,\n"
      "        \"clock_zone\": 0,\n"
      "        \"x\": {},\n"
      "        \"fixed_magnetization\": false,\n"
      "        \"y\": {},\n"
      "        \"logic\": \"normal\",\n"
      "        \"id\": \"\",\n"
      "        \"type\": \"regular\"\n"
      "    }},\n"
      "    {{\n"
      "        \"magnetization\": 0.0,\n"
      "        \"clock_zone\": 0,\n"
      "        \"x\": {},\n"
      "        \"fixed_magnetization\": false,\n"
      "        \"y\": {},\n"
      "        \"logic\": \"normal\",\n"
      "        \"id\": \"\",\n"
      "        \"type\": \"regular\"\n"
      "    }},\n"
      "    {{\n"
      "        \"magnetization\": 0.0,\n"
      "        \"clock_zone\": 0,\n"
      "        \"x\": {},\n"
      "        \"fixed_magnetization\": false,\n"
      "        \"y\": {},\n"
      "        \"logic\": \"normal\",\n"
      "        \"id\": \"\",\n"
      "        \"type\": \"regular\"\n"
      "    }},\n"
      , x, y, x+1, y, x+1, y+1);
  } // }}}

  // fn: w_lr {{{
  auto w_lr(f64 x, f64 y)
  {
    return fmt::format(
      "    {{\n"
      "        \"magnetization\": 0.0,\n"
      "        \"clock_zone\": 0,\n"
      "        \"x\": {},\n"
      "        \"fixed_magnetization\": false,\n"
      "        \"y\": {},\n"
      "        \"logic\": \"normal\",\n"
      "        \"id\": \"\",\n"
      "        \"type\": \"regular\"\n"
      "    }},\n"
      "    {{\n"
      "        \"magnetization\": 0.0,\n"
      "        \"clock_zone\": 0,\n"
      "        \"x\": {},\n"
      "        \"fixed_magnetization\": false,\n"
      "        \"y\": {},\n"
      "        \"logic\": \"normal\",\n"
      "        \"id\": \"\",\n"
      "        \"type\": \"regular\"\n"
      "    }},\n"
      "    {{\n"
      "        \"magnetization\": 0.0,\n"
      "        \"clock_zone\": 0,\n"
      "        \"x\": {},\n"
      "        \"fixed_magnetization\": false,\n"
      "        \"y\": {},\n"
      "        \"logic\": \"normal\",\n"
      "        \"id\": \"\",\n"
      "        \"type\": \"regular\"\n"
      "    }},\n"
      , x, y, x+1, y, x+2, y);
  } // }}}

  // fn: w_ud {{{
  auto w_ud(f64 x, f64 y)
  {
    return fmt::format(
        "    {{\n"
        "        \"magnetization\": 0.0,\n"
        "        \"clock_zone\": 0,\n"
        "        \"x\": {},\n"
        "        \"fixed_magnetization\": false,\n"
        "        \"y\": {},\n"
        "        \"logic\": \"normal\",\n"
        "        \"id\": \"\",\n"
        "        \"type\": \"regular\"\n"
        "    }},\n"
        "    {{\n"
        "        \"magnetization\": 0.0,\n"
        "        \"clock_zone\": 0,\n"
        "        \"x\": {},\n"
        "        \"fixed_magnetization\": false,\n"
        "        \"y\": {},\n"
        "        \"logic\": \"normal\",\n"
        "        \"id\": \"\",\n"
        "        \"type\": \"regular\"\n"
        "    }},\n"
        "    {{\n"
        "        \"magnetization\": 0.0,\n"
        "        \"clock_zone\": 0,\n"
        "        \"x\": {},\n"
        "        \"fixed_magnetization\": false,\n"
        "        \"y\": {},\n"
        "        \"logic\": \"normal\",\n"
        "        \"id\": \"\",\n"
        "        \"type\": \"regular\"\n"
        "    }},\n"
    , x+1, y, x+1, y+1, x+1, y+2);
  } // }}}

  // fn: w_ur {{{
  auto w_ur(f64 x, f64 y)
  {
    return fmt::format(
      "    {{\n"
      "        \"magnetization\": 0.0,\n"
      "        \"clock_zone\": 0,\n"
      "        \"x\": {},\n"
      "        \"fixed_magnetization\": false,\n"
      "        \"y\": {},\n"
      "        \"logic\": \"normal\",\n"
      "        \"id\": \"\",\n"
      "        \"type\": \"regular\"\n"
      "    }},\n"
      "    {{\n"
      "        \"magnetization\": 0.0,\n"
      "        \"clock_zone\": 0,\n"
      "        \"x\": {},\n"
      "        \"fixed_magnetization\": false,\n"
      "        \"y\": {},\n"
      "        \"logic\": \"normal\",\n"
      "        \"id\": \"\",\n"
      "        \"type\": \"regular\"\n"
      "    }},\n"
      "    {{\n"
      "        \"magnetization\": 0.0,\n"
      "        \"clock_zone\": 0,\n"
      "        \"x\": {},\n"
      "        \"fixed_magnetization\": false,\n"
      "        \"y\": {},\n"
      "        \"logic\": \"normal\",\n"
      "        \"id\": \"\",\n"
      "        \"type\": \"regular\"\n"
      "    }},\n"
    , x+1, y, x+1, y+1, x+2, y+1);
  } // }}}

  // fn: w_urd {{{
  auto w_urd(f64 x, f64 y)
  {
    return fmt::format(
      "    {{\n"
      "        \"magnetization\": 0.0,\n"
      "        \"clock_zone\": 0,\n"
      "        \"x\": {},\n"
      "        \"fixed_magnetization\": false,\n"
      "        \"y\": {},\n"
      "        \"logic\": \"normal\",\n"
      "        \"id\": \"\",\n"
      "        \"type\": \"regular\"\n"
      "    }},\n"
      "    {{\n"
      "        \"magnetization\": 0.0,\n"
      "        \"clock_zone\": 0,\n"
      "        \"x\": {},\n"
      "        \"fixed_magnetization\": false,\n"
      "        \"y\": {},\n"
      "        \"logic\": \"normal\",\n"
      "        \"id\": \"\",\n"
      "        \"type\": \"regular\"\n"
      "    }},\n"
      "    {{\n"
      "        \"magnetization\": 0.0,\n"
      "        \"clock_zone\": 0,\n"
      "        \"x\": {},\n"
      "        \"fixed_magnetization\": false,\n"
      "        \"y\": {},\n"
      "        \"logic\": \"normal\",\n"
      "        \"id\": \"\",\n"
      "        \"type\": \"regular\"\n"
      "    }},\n"
      "    {{\n"
      "        \"magnetization\": 0.0,\n"
      "        \"clock_zone\": 0,\n"
      "        \"x\": {},\n"
      "        \"fixed_magnetization\": false,\n"
      "        \"y\": {},\n"
      "        \"logic\": \"normal\",\n"
      "        \"id\": \"\",\n"
      "        \"type\": \"regular\"\n"
      "    }},\n"
    , x+1, y, x+1, y+1, x+2, y+1, x+1, y+2);
  } // }}}
}
// }}}

// class: NanoScl {{{
class NanoScl
{
  private:
    std::stringstream buf;
  public:
    template<typename F>
    NanoScl(MapVertexTile const& m_vertex_tile, F&& f_gate_type);
}; // class: NanoScl

template<typename F>
NanoScl::NanoScl(MapVertexTile const& m_vertex_tile, F&& f_gate_type)
{

  this->buf <<
    R"(
      [
          {
              "numberY": 300,
              "numberX": 1000
          },
    )";

  for (auto const& [u,tile] : m_vertex_tile)
  {
    auto [x,y] = std::make_pair(tile.x*3,tile.y*3);

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

  fmt::print("{}\n", this->buf.str());
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
  auto vertex_tile {ns_representations::grid::run(root, ops, layers)};

  //
  // Tile Mapping
  //
  NanoScl mapping(vertex_tile,f_gate_type);

  // TODO Remove
  auto routes {ns_representations::grid::route(ops, vertex_tile, layers)};
  celaeno::graph::draw::svg::svg(fmt::format("{}.svg", fn),
      vertex_tile,
      routes,
      [](auto){ return " "; }
  );

} // function: run }}}

} // namespace celaeno::tech::nml::nanoscl
