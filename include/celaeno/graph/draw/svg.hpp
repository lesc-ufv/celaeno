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

#include <fmt/core.h>
#include <fmt/ranges.h>
#include <range/v3/all.hpp>
#ifndef NDEBUG
  #include <spdlog/spdlog.h>
#endif

#include <celaeno/aliases.hpp>
#include <celaeno/concepts.hpp>
#include <celaeno/graph/views/depth.hpp>
#include <celaeno/graph/operations/balance/paths.hpp>
#include <celaeno/graph/operations/balance/outgoing.hpp>
#include <celaeno/graph/operations/minimize/crossings.hpp>

// namespace celaeno::graph::draw::svg {{{
namespace celaeno::graph::draw::svg
{

// namespaces {{{
namespace rg = ranges;

namespace ns_balance = celaeno::graph::operations::balance;
namespace ns_minimize = celaeno::graph::operations::minimize;
// }}}

// Using namespaces {{{
using namespace celaeno::concepts;
using namespace celaeno::aliases;
// }}}

// Shapes {{{
constexpr std::string_view const h_template
{
  "<svg version='1.1' viewBox='0 0 {} {}'>\n"
  "<style>\n"
  "  circle {{\n"
  "    fill: white;\n"
  "    stroke: black;\n"
  "    stroke-width: 2px;\n"
  "  }}\n"
  "</style>\n"
};

constexpr std::string_view const v_label_template
{
  "<text x='{}' y='{}' text-anchor='middle' font-size='{}' fill='black'>{}</text>\n"
};

constexpr std::string_view const v_template
{
  "<circle r='{}' cx='{}' cy='{}' fill='white'/>\n",
};

const std::string_view e_template
{
  "<line x1 = '{}' y1 = '{}' x2 = '{}' y2 = '{}' "
  "stroke = 'black' stroke-width = '3'/>\n"
};

constexpr std::string_view const footer{"</svg>\n"};
// }}}

// fn: run {{{
template<SignedIntegral S,
  typename F1,
  typename F2,
  typename F3,
  typename F4,
  typename F5,
  typename F6,
  String Str>
decltype(auto) run(
  S root,
  F1&& f_pred,
  F2&& f_succ,
  F3&& f_adj,
  F4&& f_link,
  F5&& f_unlink,
  F6&& f_label,
  Str&& fn
)
  requires CallableWith<F1,i64>
  && CallableWith<F2,i64>
  && CallableWith<F3,i64,i64>
{

#ifndef NDEBUG
  spdlog::set_level(spdlog::level::debug);
  spdlog::debug("Algorithm: celaeno::graph::draw::svg");
#endif

  // @ Vertices settings {{{
  // Radius
  constexpr i32 const vertex_radius{50};
  // Horizontal space between vertices
  constexpr i32 const vertex_hspacing{vertex_radius*3};
  // Vertical space between vertices
  constexpr i32 const vertex_vspacing{vertex_radius*3};
  // Offset of circles from the edges
  constexpr i32 const circle_offset{vertex_radius};
  // }}}

  // @ Vertex Placement {{{

  // Save vertices positions
  std::map<S,std::pair<i64,i64>> vertex_pos;

  auto f_place =
  [&](auto x, auto y, auto u)
  {
      vertex_pos.emplace(
        std::make_pair(
          u,
          std::make_pair(x*vertex_hspacing+circle_offset,y*vertex_vspacing+circle_offset)
        )
      );
  }; // lamb: f_place

  // Edge minimization Oriented Drawing
  ns_balance::outgoing::run(root,f_pred,f_succ,f_link,f_unlink);
  ns_balance::paths::run(root,f_pred,f_succ,f_link,f_unlink);
  auto layers {ns_minimize::crossings::run(root,f_pred,f_succ,f_adj,f_link,f_unlink)};

  // Find level with higher number of vertices
  auto const it_base
    {rg::max_element(layers,[](auto a, auto b){ return a.size() < b.size(); })};

  // Save x coordinates of vertices
  std::map<i64,i64> vx;

  // Save occupation of x positions for each layer
  std::set<i64> occupation;

  // Place level with higher number of vertices
  rg::for_each(*it_base,
  [&,x=0,y=std::distance(layers.begin(),it_base)](auto u) mutable
  {
    f_place(x,y,u);
    vx[u] = x;
    ++x;
  });

  // Calculate the mean of the predecessors/successors positions
  auto mean_of_pos = [&](auto const& vs)
  {
    i64 pos
    {
      rg::accumulate(vs,0,[&](auto acc, auto u){ return acc + vx[u]; })
      /
      static_cast<i64>(vs.size())
    };
    while( occupation.contains(pos) ) { ++pos; }
    return pos;
  };

  // Place next levels
  {
    auto y{std::distance(layers.begin(),it_base)+1};
    for (auto it{std::next(it_base)}; it != layers.cend(); ++it)
    {
      // Place remaining levels after current
      for (i64 x{}; auto const& u : *it)
      {
        // Set the x position to a mean of the predecessors positions
        x = mean_of_pos(f_pred(u));
        // Save the position
        vx[u] = x;
        // Mark position as used
        occupation.emplace(x);
        // Position the vertex
        f_place(x,y,u);
      } // for
      // Increase layer counter
      ++y;
      // Reset occupation, for next layer
      occupation.clear();
    } // for
  }

  // Place previous levels
  {
    auto y{std::distance(layers.begin(),it_base)-1};
    for (auto it{std::prev(it_base)};; --it)
    {
      // Place remaining levels before current
      for (i64 x{}; auto const& u : *it)
      {
        // Set the x position to a mean of the predecessors positions
        x = mean_of_pos(f_succ(u));
        // Save the position
        vx[u] = x;
        // Mark position as used
        occupation.emplace(x);
        // Position the vertex
        f_place(x,y,u);
      } // for
      // Stop if current level == begin
      if(it == layers.begin()){ break; }
      // Increase layer counter
      --y;
      // Reset occupation, for next layer
      occupation.clear();
    } // for
  } // }}}

  // @ Output streams {{{

  // Output file
  std::ofstream of{fn};

  // Streams
  std::stringstream header;
  std::stringstream vertices;
  std::stringstream edges;

  // }}}

  // @ Edge Routing {{{
  rg::for_each(vertex_pos, [&](auto&& e)
  {
    auto succ { f_succ(e.first) };

    rg::for_each(succ, [&](auto&& v)
    {
      auto [ux,uy] = e.second;
      auto [vx,vy] = vertex_pos[v];
      edges << fmt::format(e_template, ux, uy, vx, vy);
    });
  }); // }}}

  // @ Get farthest vertices to build viewport {{{
  auto view_box_x {rg::max_element(vertex_pos,
  [](auto a, auto b)
  {
    return a.second.first < b.second.first;
  })->second.first+circle_offset};

  auto view_box_y {rg::max_element(vertex_pos,
  [](auto a, auto b)
  {
    return a.second.second < b.second.second;
  })->second.second+circle_offset}; // }}}

  // @ Stream/File writting {{{
  header << fmt::format(h_template,view_box_x,view_box_y);

  // Vertices and labels
  for (auto [v,pos] : vertex_pos)
  {
    auto [x,y] = pos;
    vertices << fmt::format(v_template, vertex_radius, x, y);
    vertices << fmt::format(v_label_template, x, y, 40, f_label(v));
  } // for

  // File writting
  of << fmt::format("{}\n", header.str());
  of << fmt::format("{}\n", edges.str());
  of << fmt::format("{}\n", vertices.str());
  of << fmt::format("{}\n", footer);
  of.close();
  // }}}

} // function: run }}}

} // namespace celaeno::graph::draw::svg }}}
