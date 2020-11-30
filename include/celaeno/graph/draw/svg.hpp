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

#include <celaeno/aliases.hpp>
#include <celaeno/concepts.hpp>
#include <celaeno/graph/operations/minimize/crossings.hpp>
#include <celaeno/graph/views/depth.hpp>

// namespace celaeno::graph::draw::svg {{{
namespace celaeno::graph::draw::svg
{

// namespaces {{{
namespace rg = ranges;
namespace rv = ranges::views;

namespace depth = celaeno::graph::views::depth;
namespace minimize = celaeno::graph::operations::minimize::crossings;
// }}}

// Using namespaces {{{
using namespace celaeno::concepts;
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
  "<text x='{}' y='{}' font-size='{}' fill='black'>{}</text>\n"
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
  String Str>
decltype(auto) run(
  S root,
  F1&& f_pred,
  F2&& f_succ,
  F3&& f_adj,
  F4&& f_link,
  F5&& f_unlink,
  Str&& fn,
  bool optimize = true
)
{

  // @ Variables {{{

  // Output file
  std::ofstream of{fn};

  // Edge minimization Oriented Drawing
  std::vector<std::vector<i64>> layers;
  if ( optimize )
  {
    layers = minimize::run(root,f_pred,f_succ,f_adj,f_link,f_unlink);
  } // if
  else
  {
    auto depth_view {depth::run(root,f_pred,f_succ).first};
    layers = depth_view
      | rv::transform([](auto&& e){ return e.second; })
      | rg::to<std::vector<std::vector<i64>>>;
  } // else

  // Streams
  std::stringstream header;
  std::stringstream vertices;
  std::stringstream edges;

  // Viewbox size
  i64 view_box_x {};
  i64 view_box_y {};

  // Vertices settings
  auto vertex_radius{30};
  auto vertex_hspacing{vertex_radius+80};
  auto vertex_vspacing{vertex_radius+80};

  // Vertices positions
  std::map<S,std::pair<i64,i64>> vertex_pos;

  // }}}

  // @ Vertices positioning {{{

  //
  // @ Vertex Placement
  //
  rg::for_each(layers,[&,y=1](auto&& layer) mutable
  {
    rg::for_each(layer,[&,x=1](auto&& vertex) mutable
    {

      if ( x*vertex_hspacing > view_box_x ) { view_box_x = x*vertex_hspacing; }

      if ( y*vertex_vspacing > view_box_y ) { view_box_y = y*vertex_vspacing; }

      vertices <<
        fmt::format(
            v_template
          , vertex_radius
          , x*vertex_hspacing
          , y*vertex_vspacing
        );

      vertices <<
        fmt::format(
            v_label_template
          , x*vertex_hspacing-15
          , y*vertex_vspacing+15
          , 40
          , vertex
        );

        vertex_pos.emplace(
          std::make_pair(
            vertex,
            std::make_pair(x*vertex_hspacing-15,y*vertex_vspacing+15)
          )
        );

      ++x;
    });
    ++y;
  });
  // }}}

  // @ Header {{{
  header <<
    fmt::format(h_template,view_box_x+2*vertex_radius,view_box_y+2*vertex_radius);
  // }}}

  // @ Edge Routing {{{
  rg::for_each(vertex_pos, [&](auto&& e)
  {
    auto u { e.first };
    auto pos_parent { e.second };
    auto succ { f_succ(u) };

    rg::for_each(succ, [&](auto&& v)
    {
      auto pos_child{vertex_pos[v]};

      edges <<
        fmt::format(
            e_template
          , pos_parent.first+(vertex_radius/2)
          , pos_parent.second-(vertex_radius/2)
          , pos_child.first+(vertex_radius/2)
          , pos_child.second-(vertex_radius/2)
        );
    });
  });
  // }}}

  // @ File output {{{
  of << fmt::format("{}\n", header.str());
  of << fmt::format("{}\n", edges.str());
  of << fmt::format("{}\n", vertices.str());
  of << fmt::format("{}\n", footer);
  of.close();
  // }}}

} // function: run }}}

} // namespace celaeno::graph::draw::svg }}}
