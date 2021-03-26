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
#include <celaeno/heuristics/manhattan.hpp>
#include <celaeno/graph/views/depth.hpp>
#include <celaeno/graph/search/bfs.hpp>
#include <celaeno/graph/search/a-star.hpp>
#include <celaeno/graph/operations/balance/paths.hpp>
#include <celaeno/graph/operations/balance/outgoing.hpp>
#include <celaeno/graph/operations/minimize/crossings.hpp>
#include <celaeno/graph/operations/minimize/pseudo.hpp>
#include <celaeno/graph/operations/minimize/edge-length.hpp>
#include <celaeno/graph/representations/grid.hpp>

#include "meta.hpp"

// namespace celaeno::graph::draw::svg {{{
namespace celaeno::graph::draw::svg
{

// Macros {{{
#define assertm(exp, msg) assert(((void)msg, exp))
// }}}

// namespaces {{{
namespace rg = ranges;
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
// }}}

// Shapes {{{
constexpr std::string_view const h_template
{
  "<svg version='1.1' viewBox='0 0 {} {}'>\n"
  "<style>\n"
  "  circle {{\n"
  "    stroke: black;\n"
  "    stroke-width: 2px;\n"
  "  }}\n"
  "</style>\n"
};

constexpr std::string_view const v_label_template
{
  "<text x='{}' y='{}' text-anchor='middle' font-size='{}' fill='{}'>{}</text>\n"
};

constexpr std::string_view const v_template
{
  "<circle r='{}' cx='{}' cy='{}' fill='{}'/>\n",
};

constexpr std::string_view const e_template
{
  "<line x1 = '{}' y1 = '{}' x2 = '{}' y2 = '{}' "
  "stroke = 'white' stroke-width = '1'/>\n"
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

// fn: black_white {{{
template<String S, typename Map, typename F1, typename F2>
void black_white(S&& filename, Map&& vertex_xy, F1&& f_succ, F2&& f_label)
{
  // Output file
  std::ofstream of{filename};

  // Streams
  std::stringstream header;
  std::stringstream vertices;
  std::stringstream edges;

  //Get farthest vertices to build viewport
  auto view_box_x {rg::max_element(vertex_xy,{},
  [](auto e) { return e.second.first; })->second.first};

  auto view_box_y {rg::max_element(vertex_xy,{},
  [](auto e) { return e.second.second; })->second.second};

  // Adjust positions for drawing
  for (auto& e : vertex_xy)
  {
    auto& pos = e.second;
    auto& [x,y] = pos;
    x = x*tile_size+circle_offset;
    y = y*tile_size+circle_offset;
  } // for

  // Edge Routing
  rg::for_each(vertex_xy, [&](auto&& e)
  {
    auto succ { f_succ(e.first) };

    rg::for_each(succ, [&](auto&& v)
    {
      auto [ux,uy] = e.second;
      auto [vx,vy] = vertex_xy[v];
      edges << fmt::format(e_template, ux, uy, vx, vy);
    });
  });

  // @ Stream/File writting
  header << fmt::format(h_template,view_box_x*tile_size+circle_offset*2, view_box_y*tile_size+circle_offset*2);


  // Vertices and labels
  for (auto [v,pos] : vertex_xy)
  {
    auto [x,y] = pos;
    // Draw pseudo-nodes with black filling
    if (v < 0)
    {
      vertices << fmt::format(v_template, vertex_radius, x, y, "black");
      vertices << fmt::format(v_label_template, x, y, 10, "white", f_label(v));
    } // if
    else
    {
      vertices << fmt::format(v_template, vertex_radius, x, y, "white");
      vertices << fmt::format(v_label_template, x, y, 10, "black", f_label(v));
    } // else
  } // for

  // File writting
  of << fmt::format("{}\n", header.str());

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
  of << fmt::format("{}\n", edges.str());
  of << fmt::format("{}\n", vertices.str());
  of << fmt::format("{}\n", footer);
  of.close();
} // function: black_white }}}

// fn: occupation_map {{{
template<String S, typename Map, typename F1, typename F2>
void occupation_map(S&& filename, Map&& vertex_xy, F1&& f_succ, F2&& f_label)
{

  // Output file
  std::ofstream of{filename};

  // Streams
  std::stringstream header;
  std::stringstream vertices;
  std::stringstream edges;


  //Get farthest vertices to build viewport
  auto view_box_x {rg::max_element(vertex_xy,{},
  [](auto e) { return e.second.first; })->second.first};

  auto view_box_y {rg::max_element(vertex_xy,{},
  [](auto e) { return e.second.second; })->second.second};

  // Adjust positions for drawing
  for (auto& e : vertex_xy)
  {
    auto& pos = e.second;
    auto& [x,y] = pos;
    x = x*tile_size+circle_offset;
    y = y*tile_size+circle_offset;
  } // for

  // Edge Routing
  rg::for_each(vertex_xy, [&](auto&& e)
  {
    auto succ { f_succ(e.first) };

    rg::for_each(succ, [&](auto&& v)
    {
      auto [ux,uy] = e.second;
      auto [vx,vy] = vertex_xy[v];
      edges << fmt::format(e_template, ux, uy, vx, vy);
    });
  });

  // @ Stream/File writting
  header << fmt::format(h_template,view_box_x*tile_size+circle_offset*2, view_box_y*tile_size+circle_offset*2);

  // Vertices and labels
  for (auto [v,pos] : vertex_xy)
  {
    // Decompose position
    auto [x,y] = pos;
    // Check how many nodes are in [x,y]
    auto count{rg::count_if(vertex_xy,[&,pos=pos](auto e){ return e.second == pos; })};
    // Draw pseudo-nodes with occupation coloring
    switch (count)
    {
      case 1:
      {
        vertices << fmt::format(v_template, vertex_radius, x, y, "forestgreen");
        break;
      }
      case 2:
      {
        vertices << fmt::format(v_template, vertex_radius, x, y, "orangered");
        break;
      }
      case 3:
      {
        vertices << fmt::format(v_template, vertex_radius, x, y, "darkred");
        break;
      }
      case 4:
      {
        vertices << fmt::format(v_template, vertex_radius, x, y, "darkmagenta");
        break;
      }
      default:
        vertices << fmt::format(v_template, vertex_radius, x, y, "black");
    } // switch
  } // for

  // File writting
  of << fmt::format("{}\n", header.str());

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
  of << fmt::format("{}\n", edges.str());
  of << fmt::format("{}\n", vertices.str());
  of << fmt::format("{}\n", footer);
  of.close();

} // function: occupation_map }}}


enum class Direction
{
  MIDDLE,
  LEFT,
  RIGHT,
}; // enum

// fn: run 
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


  // @ Pre-processing {{{
  // Remove dangling nodes
  auto remove_dangling =
  [&]
  {
    auto view_depth{ns_views::depth::run(root, f_pred, f_succ).first};
    for (auto it{view_depth.begin()}; it != std::prev(view_depth.end()); ++it)
    {
      for (auto parent : it->second)
      {
        if (f_succ(parent).empty() && parent < 0)
        {
          std::queue<i64> q;
          q.emplace(parent);
          while( ! q.empty() )
          {
            auto node{q.front()}; q.pop();
            spdlog::warn("Node {} is dangling and will be removed", node);
            for (auto pred : f_pred(node))
            {
              f_unlink(std::make_pair(pred,node));
              if( pred < 0 && f_succ(pred).empty() ){ q.push(pred); }
            } // for
          } // while
        } // if
      } // for
    } // for
  };

  remove_dangling();

  // Edge minimization Oriented Drawing
  ns_balance::paths::run(root,f_pred,f_succ,f_link,f_unlink);
  ns_balance::outgoing::run(root,f_pred,f_succ,f_link,f_unlink);
  ns_minimize::pseudo::run(root,f_pred,f_succ,f_link,f_unlink);
  // }}}


  // @ Vertex Placement
  auto grid{ns_representations::grid::run(root,f_pred,f_succ,f_adj,f_link,f_unlink)};
  auto& layers = grid.first;
  auto& vertex_xy = grid.second;

  // Pseudo number index
  i64 counter{};

  // Define function to compare values lt 0
  auto f_lowest = [&](auto e)
  {
    if(e < counter){ counter=e; } return false;
  };

  // Get the dummy vertex with the lowest value
  ns_search::bfs::run(root,f_pred,f_succ,f_lowest);

  // A struct to control the value of pseudo nodes
  auto make_pseudo = [counter]
  {
    struct Pseudo
    {
      private:
        i64 c;
      public:
        Pseudo(i64 c) : c(c) {}
        i64 next(){ return --c; }
        i64 curr(){ return c; }
    };
    return Pseudo{counter};
  }();

  auto expand_grid = [&](i64 multiplier)
  {
    for (auto& e : vertex_xy)
    {
      auto& [x,y] = e.second;
      x = x*multiplier + 1;
      y = y*multiplier + 1;
    } // for
  };

  auto rearrange_nodes =
  [&](i64 offset)
  {
    // Initial nodes
    auto i_vertex_xy{vertex_xy};

    // Get all pos xy
    auto positions{fp::get_map_keys(fp::swap_keys_and_values(vertex_xy))};

    for (auto pos : positions)
    {
      // Check if there is more than 1 node in xy
      std::vector<i64> nodes;
      auto count{rg::count_if(vertex_xy,
        [&,pos=pos](auto e)
        {
          if(  e.second == pos )
          {
            nodes.push_back(e.first);
            return true;
          }
          return false;
        }
      )};

      if (count == 2)
      {
        //
        // Upper nodes
        //

        auto n0{nodes.at(0)};
        auto n1{nodes.at(1)};

        auto n0_dir{Direction::LEFT};
        auto n1_dir{Direction::LEFT};

        for (auto pred : f_pred(n0))
        {
          if( vertex_xy[pred].first < vertex_xy[n0].first )
          {
            n0_dir = Direction::LEFT;
          } // if
          else
          {
            n0_dir = Direction::RIGHT;
          } // else
        } // for

        for (auto pred : f_pred(n1))
        {
          if( vertex_xy[pred].first < vertex_xy[n1].first )
          {
            n1_dir = Direction::LEFT;
          } // if
          else
          {
            n1_dir = Direction::RIGHT;
          } // else
        } // for

        if( n0_dir == n1_dir )
        {
          // auto& [x,y] = vertex_xy.at(nodes.at(0));
          // x -= 1;
          continue;
        }

        for (i32 i{}; auto node : nodes)
        {
          // Create new dummy
          auto new_node{make_pseudo.next()};
          // Perform dummy insertion
          for (auto pred : f_pred(node))
          {
            f_link(std::make_pair(pred,new_node));
            f_unlink(std::make_pair(pred,node));
          } // for
          f_link(std::make_pair(new_node,node));
          auto [x,y] = vertex_xy[node];

          // Position dummy
          y -= offset;
          if( i == 0 )
          {
            x = (n0_dir == Direction::LEFT)? x : x+offset;
          } // if
          else
          {
            x = (n1_dir == Direction::LEFT)? x : x+offset;
          } // else

          vertex_xy[new_node] = std::make_pair(x,y);

          ++i;
        } // for

        for (auto node : nodes)
        {
          // Remove from initial nodes
          i_vertex_xy.erase(node);

          std::vector<Direction> common_dir;

          for (auto succ : f_succ(node))
          {
            auto new_node{make_pseudo.next()};
            // Perform linking
            f_link(std::make_pair( node,new_node ));
            f_link(std::make_pair( new_node,succ ));
            f_unlink(std::make_pair( node,succ ));
            // Define position
            auto [ux,uy] = vertex_xy[node];
            auto [vx,vy] = vertex_xy[succ];
            if( vx == ux )
            {
              vertex_xy[new_node] = std::make_pair(ux,uy+1);
              common_dir.push_back(Direction::MIDDLE);
            } // if
            else if (vx > ux)
            {
              vertex_xy[new_node] = std::make_pair(ux+1,uy+1);
              common_dir.push_back(Direction::RIGHT);
            } // else if
            else
            {
              vertex_xy[new_node] = std::make_pair(ux-1,uy+1);
              common_dir.push_back(Direction::LEFT);
            } // else
          } // for

          // Find most common pos
          if( common_dir.size() == 1 )
          {
            auto node_pos{common_dir.at(0)};

            switch (node_pos)
            {
              case Direction::MIDDLE: break;
              case Direction::LEFT: vertex_xy[node].first -= 1; break;
              case Direction::RIGHT: vertex_xy[node].first += 1; break;
            } // switch
          } // if
          else if(
            rg::contains(common_dir, Direction::MIDDLE)
            && rg::contains(common_dir,Direction::LEFT)
          )
          {
            auto pos{vertex_xy[node]};
            pos.first -= 1;
            // Check how many nodes are in [x,y]
            auto count{rg::count_if(vertex_xy,[&,pos=pos](auto e){ return e.second == pos; })};
            // If there are none, assign node
            if( count == 0 )
            {
              vertex_xy[node].first -= 1;
            }
            // Else keep position
          } // else if
          else if(
            rg::contains(common_dir, Direction::MIDDLE)
            && rg::contains(common_dir,Direction::RIGHT)
          )
          {
            auto pos{vertex_xy[node]};
            pos.first += 1;
            // Check how many nodes are in [x,y]
            auto count{rg::count_if(vertex_xy,[&,pos=pos](auto e){ return e.second == pos; })};
            // If there are none, assign node
            if( count == 0 )
            {
              vertex_xy[node].first += 1;
            }
            // Else keep position
          }

        } // for

      } // if

    } // for

    for (auto [u,xy] : i_vertex_xy)
    {
      for (auto succ : f_succ(u))
      {
        auto new_node{make_pseudo.next()};
        // Perform linking
        f_link(std::make_pair( u,new_node ));
        f_link(std::make_pair( new_node,succ ));
        f_unlink(std::make_pair( u,succ ));
        // Define position
        auto [ux,uy] = vertex_xy[u];
        auto [vx,vy] = vertex_xy[succ];
        if( vx == ux )
        {
          vertex_xy[new_node] = std::make_pair(ux,uy+1);
        } // if
        else if (vx > ux)
        {
          vertex_xy[new_node] = std::make_pair(ux+1,uy+1);
        } // else if
        else
        {
          vertex_xy[new_node] = std::make_pair(ux-1,uy+1);
        } // else
      } // for

      for (auto pred : f_pred(u))
      {
        auto new_node{make_pseudo.next()};
        // Perform linking
        f_link(std::make_pair( pred,new_node ));
        f_link(std::make_pair( new_node,u ));
        f_unlink(std::make_pair( pred,u ));
        // Define position
        auto [ux,uy] = vertex_xy[u];
        auto [vx,vy] = vertex_xy[pred];
        if( vx == ux )
        {
          vertex_xy[new_node] = std::make_pair(ux,uy-1);
        } // if
        else if (vx > ux)
        {
          vertex_xy[new_node] = std::make_pair(ux+1,uy-1);
        } // else if
        else
        {
          vertex_xy[new_node] = std::make_pair(ux-1,uy-1);
        } // else
      } // for
    } // for
  };

  expand_grid(3);
  rearrange_nodes(1);

  // // Get all pos xy
  // auto positions{fp::get_map_keys(fp::swap_keys_and_values(vertex_xy))};
  //
  // for (auto pos : positions)
  // {
  //   // Check if there is more than 1 node in xy
  //   std::vector<i64> nodes;
  //   auto count{rg::count_if(vertex_xy,
  //     [&,pos=pos](auto e)
  //     {
  //       if(  e.second == pos )
  //       {
  //         nodes.push_back(e.first);
  //         return true;
  //       }
  //       return false;
  //     }
  //   )};
  //
  //   if (count == 2)
  //   {
  //     //
  //     // Upper nodes
  //     //
  //
  //     auto n0{nodes.at(0)};
  //     auto n1{nodes.at(1)};
  //
  //     auto n0_dir{Direction::LEFT};
  //     auto n1_dir{Direction::LEFT};
  //
  //     for (auto pred : f_pred(n0))
  //     {
  //       if( vertex_xy[pred].first < vertex_xy[n0].first )
  //       {
  //         n0_dir = Direction::LEFT;
  //       } // if
  //       else
  //       {
  //         n0_dir = Direction::RIGHT;
  //       } // else
  //     } // for
  //
  //     for (auto pred : f_pred(n1))
  //     {
  //       if( vertex_xy[pred].first < vertex_xy[n1].first )
  //       {
  //         n1_dir = Direction::LEFT;
  //       } // if
  //       else
  //       {
  //         n1_dir = Direction::RIGHT;
  //       } // else
  //     } // for
  //
  //     if( n0_dir == n1_dir )
  //     {
  //       // auto& [x,y] = vertex_xy.at(nodes.at(0));
  //       // x -= 1;
  //       continue;
  //     }
  //
  //     if(n0_dir == Direction::LEFT )
  //     {
  //       auto [x,y] = vertex_xy[n0];
  //       vertex_xy[n0] = std::make_pair(x+1,y);
  //       std::tie(x,y) = vertex_xy[n1];
  //       vertex_xy[n1] = std::make_pair(x-1,y);
  //     } // if
  //     else
  //     {
  //       auto [x,y] = vertex_xy[n0];
  //       vertex_xy[n0] = std::make_pair(x-1,y);
  //       std::tie(x,y) = vertex_xy[n1];
  //       vertex_xy[n1] = std::make_pair(x+1,y);
  //     } // else
  //   } // if
  // } // for

  // grid = ns_representations::grid::run(root,f_pred,f_succ,f_adj,f_link,f_unlink);
  //
  using S1 = typename std::decay_t<decltype(vertex_xy)>;
  using S2 = typename std::decay_t<typename std::decay_t<decltype(layers)>::value_type>;
  using Sol = std::pair<S1,S2>;
  using Cost = f64;

  auto f_layer_cost =
  [&](Sol const& s)
  {
    auto _vertex_xy{s.first};
    auto layer{s.second};

    f64 total_cost{};

    //
    // Cost of overlapping nodes as 2^cost
    //

    for (auto u : layer)
    {
      // Get current node xy
      auto pos = _vertex_xy.at(u);
      // Calculate how many nodes are in xy
      auto cost{rg::count_if(_vertex_xy,
      [&,pos=pos](auto e)
      {
        auto [v,xy] = e;
        return xy == pos;
      })};
      // Save in total_cost
      if( cost > 1 ) { total_cost += 10; }
    }
    //
    // Cost of edges with len > 1
    //

    // Absolute x-dist of nodes u and v
    auto f_dist = [&](auto u, auto v)
    {
      return std::abs(_vertex_xy[u].first-_vertex_xy[v].first);
    };

    for (auto u : layer)
    {
      // Calculate number of edges with len > 1
      for (auto v : f_succ(u))
      {
        if( auto dist{f_dist(u,v)}; dist > 1 ){ total_cost += 10*dist; }
      } // for

      for (auto v : f_pred(u))
      {
        if( auto dist{f_dist(u,v)}; dist > 1 ){ total_cost += 10*dist; }
      } // for
    } // for

    return total_cost;
  };

  auto perturb =
  [&](Sol const& s, f64 temperature)
  {
    auto _vertex_xy{s.first};
    auto layer{s.second};

    std::mt19937 gen(std::random_device{}());
    std::uniform_int_distribution moves_dist(0,4);
    std::uniform_int_distribution sz_dist(0,static_cast<i32>(layer.size()-1));
    std::uniform_int_distribution moves(0,static_cast<i32>(layer.size()));

    constexpr std::array<i8,5> r_moves{-2,-1,0,1,2};

    _vertex_xy.at(layer.at(sz_dist(gen))).first += r_moves.at(moves_dist(gen));

    return std::make_pair(_vertex_xy,layer);
  };

  auto minimize_cost =
  [&]
  {
    meta::sa::I_SimulatedAnnealing<Cost,Sol> sa;

    sa.cost = f_layer_cost;
    sa.perturb = perturb;
    for (i64 i{}; i < layers.size(); ++i)
    {
      auto solution{sa.run(std::make_pair(vertex_xy,layers.at(i)))};
      vertex_xy = solution.first;
      layers.at(i) = solution.second;
      spdlog::info("Cost: {}", f_layer_cost(solution));
    } // for
  };

  for (i64 i{}; i != 1; ++i)
  {
    minimize_cost();
  } // for

  expand_grid(3);
  // rearrange_nodes(1);

  occupation_map(fn, vertex_xy, f_succ, f_label);
} // function: run 

} // namespace celaeno::graph::draw::svg }}}
