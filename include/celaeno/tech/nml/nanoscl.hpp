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
#include <celaeno/graph/views/depth.hpp>
#include <celaeno/graph/operations/balance/paths.hpp>
#include <celaeno/graph/operations/balance/outgoing.hpp>
#include <celaeno/graph/representations/grid.hpp>

namespace celaeno::tech::nml::nanoscl
{

// Using declarations {{{
using Ops = celaeno::graph::Ops;
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
  // fn: wld {{{
  auto wld(f64 x, f64 y)
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

  // fn: wlr {{{
  auto wlr(f64 x, f64 y)
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

  // fn: wud {{{
  auto wud(f64 x, f64 y)
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

  // fn: wur {{{
  auto wur(f64 x, f64 y)
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

  // fn: wurd {{{
  auto wurd(f64 x, f64 y)
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

    // Is node
    if( u >= 0 )
    {

    } // if
    // Is wire
    else
    {
      switch (tile.type)
      {
        case TileType::LD:
          this->buf << cells::wld(x,y);
          break;
        case TileType::LR:
          this->buf << cells::wlr(x,y);
          break;
        case TileType::ND:
          // this->buf << cells::nd(x,y);
          break;
        case TileType::UD:
          this->buf << cells::wud(x,y);
          break;
        case TileType::UR:
          this->buf << cells::wur(x,y);
          break;
        case TileType::URD:
          this->buf << cells::wurd(x,y);
          break;
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

} // function: run }}}

} // namespace celaeno::tech::nml::nanoscl
