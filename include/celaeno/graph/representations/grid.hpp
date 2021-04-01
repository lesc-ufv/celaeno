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

#pragma once

#include <map>
#include <queue>
#include <set>
#include <range/v3/all.hpp>
#include <fplus/fplus.hpp>

#include <spdlog/spdlog.h>

#include <celaeno/aliases.hpp>
#include <celaeno/concepts.hpp>
#include <celaeno/graph/search/bfs.hpp>
#include <celaeno/graph/search/kahn.hpp>
#include <celaeno/graph/views/depth.hpp>
#include <celaeno/graph/operations/minimize/edge-length.hpp>
#include <celaeno/graph/operations/minimize/crossings.hpp>

#include "meta.hpp"


// namespace celaeno::graph::representations::grid {{{
namespace celaeno::graph::representations::grid
{

// Using declarations {{{
using namespace celaeno::aliases;
using namespace celaeno::concepts;
// }}}

// namespaces {{{
namespace rg = ranges;
namespace rv = ranges::views;
namespace ra = ranges::actions;
namespace fp = fplus;
namespace ns_minimize = celaeno::graph::operations::minimize;
namespace ns_search = celaeno::graph::search;
// }}}

// fn: initial {{{

//
// Crossing minimization oriented coordinates
//
template<SignedIntegral T, typename P, typename S, typename A, typename L, typename U>
decltype(auto) initial(T&& root, P&& f_pred, S&& f_succ, A&& f_adj, L&& f_link, U&& f_unlink)
{
  // Get layers
  auto layers {ns_minimize::crossings::run(root,f_pred,f_succ,f_adj,f_link,f_unlink)};

  // Save vertices positions
  std::map<i64,std::pair<i64,i64>> vertex_xy;

  // Find level with highest number of vertices
  auto const it_base {rg::max_element(layers, {}, [](auto e){ return e.size(); })};

  // Place vertex u in coordinate {x,y}
  auto f_place = [&](auto x, auto y, auto u) { vertex_xy[u] = std::make_pair(x,y); };

  // Index of it_base in layers container
  u64 idx_base{static_cast<u64>(std::distance(layers.begin(),it_base))};

  // Place level with higher number of vertices
  rg::for_each(*it_base, [&,x=0,y=idx_base](auto u) mutable { f_place(x++,y,u); });

  // Save occupation of x positions for each layer
  std::set<i64> occupation;

  // Calculate the mean of the predecessors/successors positions
  auto mean_of_pos = [&]<Range R>(R const& vs, auto node) -> std::optional<u64>
  {
    if (vs.empty())
    {
      spdlog::warn("Dangling node {} will be ignored!", node);
      return std::nullopt;
    } // if
    auto pos {rg::accumulate(vs,0,{},[&](auto u){return vertex_xy[u].first;}) / vs.size()};
    while( occupation.contains(pos) ) { ++pos; }
    return pos;
  };

  //
  // Place subsequent layers with respect to first positioned layers, e.g, given
  // [1..5] layers, if first positioned layer was 3, then subsequent layers
  // [3,4],[4,5] must be positioned sequentially in this order; and layers
  // [3,2],[2,1] must be positioned sequentially in this order.
  //

  // First half of positions for placement
  auto first_half {fp::numbers(u64{},idx_base)};

  // Reverse container and elements
  first_half = ra::reverse(first_half);

  // Second half of positions for placement
  auto second_half {fp::numbers(idx_base+1,layers.size())};

  for (auto y : fp::append(first_half,second_half))
  {
    for (auto u : layers.at(y))
    {
      // Set the x position to a mean of the predecessors positions
      auto x { (y > idx_base)? mean_of_pos(f_pred(u),u) : mean_of_pos(f_succ(u),u) };
      // Check if position is valid
      if( !x ){ continue; }
      // Mark position as used
      occupation.emplace(*x);
      // Position the vertex
      f_place(*x,y,u);
    } // for
    occupation.clear();
  } // for

  return std::make_pair(layers,vertex_xy);
} // function: initial }}}

// fn: overlap_nodes {{{

//
// Shifts nodes in layer to minimize long edges
//
template<SignedIntegral T, typename P, typename S, typename A, typename L, typename U>
decltype(auto) overlap_nodes(T&& root, P&& f_pred, S&& f_succ, A&& f_adj, L&& f_link, U&& f_unlink)
{
  auto grid {initial(root, f_pred, f_succ, f_adj, f_link, f_unlink)};
  auto& layers{grid.first};
  auto& vertex_xy{grid.second};

  auto f_dist = [&](auto u, auto v)
  {
    return std::abs(vertex_xy[u].first-vertex_xy[v].first);
  };

  //
  // Queue to keep elements with nodes that have incoming or outgoing edges with
  // distance greater than 1
  //

  std::queue<std::decay_t<T>> q;

  //
  // Use a topo search to find edges with dist greater than 1, and populate the queue
  //

  ns_search::kahn::run(root, f_pred, f_succ,
  [&](auto u)
  {
    for (auto v : f_succ(u))
    {
      auto ux{vertex_xy.at(u).first};
      auto& vx{vertex_xy.at(v).first};
      if( f_dist(u,v) > 1 )
      {
        q.push(v);
        vx = (vx > ux)? ux+1 : ux-1;
      }
    } // for
    return false;
  });

  //
  // Keep processing until all nodes of the graph have an edge distance of 1
  //

  while( ! q.empty() )
  {
    auto u{q.front()}; q.pop();

    for (auto v : f_succ(u))
    {
      auto ux{vertex_xy.at(u).first};
      auto& vx{vertex_xy.at(v).first};
      if ( f_dist(u,v) > 1 )
      {
        vx = ( vx > ux )? ( q.push(v), ux+1) : ( q.push(v), ux-1);
      } // if
    } // for

    for (auto v : f_pred(u))
    {
      auto ux{vertex_xy.at(u).first};
      auto& vx{vertex_xy.at(v).first};
      if ( f_dist(u,v) > 1 )
      {
        vx = ( vx > ux )? (q.push(v), ux+1) : (q.push(v), ux-1);
      } // if
    } // for

  } // while

  return grid;

} // function: overlap_nodes }}}

// fn: expand_grid {{{
template<typename G>
decltype(auto) expand_grid(G& grid, i64 multiplier)
{
  auto& vertex_xy{grid.second};

  for (auto& e : vertex_xy)
  {
    auto& [x,y] = e.second;
    x = x*multiplier + 1;
    y = y*multiplier + 1;
  } // for

  return grid;
} // function: expand_grid }}}

// fn: expand_nodes {{{

enum class Direction
{
  MIDDLE,
  LEFT,
  RIGHT,
}; // enum

template<SignedIntegral T,
  typename P,
  typename S,
  typename A,
  typename L,
  typename U,
  typename G>
decltype(auto) expand_nodes(T root,
  P&& f_pred,
  S&& f_succ,
  A&& f_adj,
  L&& f_link,
  U&& f_unlink,
  G& grid
)
{
  auto& [layers,vertex_xy] = grid;

  i64 offset{1};

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

  // // Initial nodes
  // auto i_vertex_xy{vertex_xy};

  // Get all pos xy
  auto positions{fp::get_map_keys(fp::swap_keys_and_values(vertex_xy))};

  for (auto pos : positions)
  {
    // Check if there is more than 1 node in xy
    std::vector<i64> overlapping_nodes;
    auto count{rg::count_if(vertex_xy,
    [&,pos=pos](auto e)
    {
      if(  e.second == pos )
      {
        overlapping_nodes.push_back(e.first);
        return true;
      }
      return false;
    })};

    if (count > 1)
    {
      for (auto u : overlapping_nodes)
      {

        //
        // Compute directions of predecessors and successors
        //
        std::vector<Direction> directions;

        for (auto v : fp::append(f_pred(u),f_succ(u)) )
        {
          if( vertex_xy[v].first < vertex_xy[u].first )
          {
            directions.push_back(Direction::LEFT);
          } // if
          else if ( vertex_xy[v].first == vertex_xy[u].first )
          {
            directions.push_back(Direction::MIDDLE);
          } // else
          else
          {
            directions.push_back(Direction::RIGHT);
          } // else
        } // for

        //
        // Count the occurrences for each direction
        //
        auto left_count{rg::count_if(directions,
          [](auto d){ return d == Direction::LEFT;
        })};

        auto middle_count{rg::count_if(directions,
          [](auto d){ return d == Direction::MIDDLE;
        })};

        auto right_count{rg::count_if(directions,
          [](auto d){ return d == Direction::RIGHT;
        })};

        //
        // Sort by occurrences
        //
        rg::sort(directions,{}, [&](auto e)
        {
          switch (e)
          {
            case Direction::LEFT: return left_count;
            case Direction::MIDDLE: return middle_count;
            case Direction::RIGHT: return right_count;
          } // switch
        });

        //
        // Reverse to get most common direction
        //

        directions =
          directions
          | rv::unique
          | rv::reverse
          | rg::to<std::vector<Direction>>;

        //
        // Move to another tile if primary choice is free, else choose the least
        // occupied tile
        //
        auto [x,y] = vertex_xy[u];

        // Remove node from current position
        vertex_xy.erase(u);

        bool primary{false};

        switch (directions.at(0))
        {
          case Direction::LEFT:
            primary = rg::count_if(vertex_xy,
            [&,x=x,y=y](auto e)
            {
              return e.second == std::make_pair(x-1,y);
            }) == 0;
            break;
          case Direction::MIDDLE:
            primary = rg::count_if(vertex_xy,
            [&,x=x,y=y](auto e)
            {
              return e.second == std::make_pair(x,y);
            }) == 0;
            break;
          case Direction::RIGHT:
            primary = rg::count_if(vertex_xy,
            [&,x=x,y=y](auto e)
            {
              return e.second == std::make_pair(x+1,y);
            }) == 0;
            break;
        } // switch


        if( primary )
        {
          switch (directions.at(0))
          {
            case Direction::LEFT:   vertex_xy[u] = std::make_pair(x-1,y); break;
            case Direction::MIDDLE: vertex_xy[u] = std::make_pair(x,y); break;
            case Direction::RIGHT:  vertex_xy[u] = std::make_pair(x+1,y); break;
          } // switch
        } // if
        else
        {
          // Get all available positions
          std::vector<std::pair<i64,i64>> positions { {x-1,y}, {x,y}, {x+1,y} };
          std::map<std::pair<i64,i64>,i64> position_occupation;

          // Get occupation count
          for (auto p : positions)
          {
            auto count{rg::count_if(vertex_xy,[&](auto e){ return e.second == p; })};
            position_occupation[p] = count;
          } // for

          // Sort by occupation count
          rg::sort(positions,{},[&](auto pos) { return position_occupation[pos]; });

          // Pick least occupied (first element)
          vertex_xy[u] = positions.at(0);
        } // else
      } // for

    } // if

  } // for

  for (auto [u,xy] : vertex_xy)
  {
    auto place_in_least_occupied =
    [&,vertex_xy=vertex_xy](i64 x, i64 y)
    {
      // Get all available positions
      std::vector<std::pair<i64,i64>> positions { {x-1,y}, {x,y}, {x+1,y} };

      // Map position → occupation (node count)
      std::map<std::pair<i64,i64>,i64> position_occupation;

      // Populate occupation count
      for (auto p : positions)
      {
        auto count{rg::count_if(vertex_xy,[&](auto e){ return e.second == p; })};
        position_occupation[p] = count;
      } // for

      // Sort by occupation count
      rg::sort(positions,{},[&](auto pos) { return position_occupation[pos]; });

      // Pick least occupied (first element)
      return positions.at(0);
    };

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
        auto count{rg::count_if(vertex_xy,[&,ux=ux,uy=uy](auto e)
        {
          return e.second == std::make_pair(ux,uy+1);
        })};
        if( count == 0 )
        {
          vertex_xy[new_node] = std::make_pair(ux,uy+1);
        }
        else
        {
          vertex_xy[new_node] = place_in_least_occupied(ux,uy+1);
        } // else
      } // if
      else if (vx > ux)
      {
        auto count{rg::count_if(vertex_xy,[&,ux=ux,uy=uy](auto e)
        {
          return e.second == std::make_pair(ux+1,uy+1);
        })};
        if( count == 0 )
        {
          vertex_xy[new_node] = std::make_pair(ux+1,uy+1);
        }
        else
        {
          vertex_xy[new_node] = place_in_least_occupied(ux,uy+1);
        } // else
      } // else if
      else
      {
        auto count{rg::count_if(vertex_xy,[&,ux=ux,uy=uy](auto e)
        {
          return e.second == std::make_pair(ux-1,uy+1);
        })};
        if( count == 0 )
        {
          vertex_xy[new_node] = std::make_pair(ux-1,uy+1);
        }
        else
        {
          vertex_xy[new_node] = place_in_least_occupied(ux,uy+1);
        } // else
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
        auto count{rg::count_if(vertex_xy,[&,ux=ux,uy=uy](auto e)
        {
          return e.second == std::make_pair(ux,uy-1);
        })};
        if( count == 0 )
        {
          vertex_xy[new_node] = std::make_pair(ux,uy-1);
        }
        else
        {
          vertex_xy[new_node] = place_in_least_occupied(ux,uy-1);
        } // else
      } // if
      else if (vx > ux)
      {
        auto count{rg::count_if(vertex_xy,[&,ux=ux,uy=uy](auto e)
        {
          return e.second == std::make_pair(ux+1,uy-1);
        })};
        if( count == 0 )
        {
          vertex_xy[new_node] = std::make_pair(ux+1,uy-1);
        }
        else
        {
          vertex_xy[new_node] = place_in_least_occupied(ux,uy-1);
        } // else
      } // else if
      else
      {
        auto count{rg::count_if(vertex_xy,[&,ux=ux,uy=uy](auto e)
        {
          return e.second == std::make_pair(ux-1,uy-1);
        })};
        if( count == 0 )
        {
          vertex_xy[new_node] = std::make_pair(ux-1,uy-1);
        }
        else
        {
          vertex_xy[new_node] = place_in_least_occupied(ux,uy-1);
        } // else
      } // else
    } // for
  } // for

  return grid;

} // function: expand_nodes }}}

// fn: fine {{{

//
// Shifts nodes in layer to minimize long edges
//
template<SignedIntegral T,
  typename P,
  typename S,
  typename A,
  typename L,
  typename U,
  typename G>
std::optional<G> fine(T&& root,
  P&& f_pred,
  S&& f_succ,
  A&& f_adj,
  L&& f_link,
  U&& f_unlink,
  G& grid
)
{
  grid = initial(root, f_pred, f_succ, f_adj, f_link, f_unlink);
  auto& layers{grid.first};
  auto& vertex_xy{grid.second};

  auto f_dist = [&](auto u, auto v)
  {
    return std::abs(vertex_xy[u].first-vertex_xy[v].first);
  };

  //
  // Queue to keep elements with nodes that have incoming or outgoing edges with
  // distance greater than 1
  //

  std::queue<std::decay_t<T>> q;

  //
  // Use a topo search to find edges with dist greater than 1, and populate the queue
  //

  ns_search::kahn::run(root, f_pred, f_succ,
  [&](auto u)
  {
    for (auto v : f_succ(u))
    {
      auto ux{vertex_xy.at(u).first};
      auto& vx{vertex_xy.at(v).first};
      if( f_dist(u,v) > 1 )
      {
        q.push(u);
        vx = (ux > vx)? ux-1 : ux+1;
      }
    } // for
    return false;
  });

  //
  // Keep processing until all nodes of the graph have an edge distance of 1
  //

  std::set<i64> visited;

  i64 i{};
  while( ! q.empty() && ++i )
  {
    auto u{q.front()}; q.pop();

    // if( visited.contains(u) )
    // {
    //   continue;
    // } // if
    // else
    // {
    //   visited.insert(u);
    // } // else
    //
    for (auto v : fp::append(f_succ(u),f_pred(u)) )
    {
      auto [ux,uy] {vertex_xy.at(u)};
      auto [vx,vy] {vertex_xy.at(v)};

      if ( f_dist(u,v) > 1 )
      {
        std::pair<i64,i64> pos;

        if( vx > ux )
        {
          pos = std::make_pair(ux+1,vy);
        } // if
        else if( vx == ux )
        {
          pos = std::make_pair(ux,vy);
        } // else
        else
        {
          pos = std::make_pair(ux-1,vy);
        } // else

        // Save all nodes in [x+1,y]
        std::set<i64> nodes_at_xy;
        // Check how many nodes are in [x+1,y]
        auto count{rg::count_if(vertex_xy,
        [&,pos=pos](auto e)
        {
          if (e.second == pos)
          {
            nodes_at_xy.insert(e.first);
            return true;
          } // if
          else
          {
            return false;
          } // else
        })};
        // If no nodes are in [x+1,y], move v to this position and enqueue
        if( count == 0 )
        {
          vertex_xy.at(v) = pos;
          q.push(v);
        } // if
        // If there is a node, then perform a swap, and enqueue both
        else if( count == 1 )
        {
          auto w{*nodes_at_xy.begin()};
          vertex_xy.at(w) = std::exchange(vertex_xy.at(v),vertex_xy.at(w));
          q.push(v);
          q.push(w);
        } // else
      } // if
    } // for
  } // while

  spdlog::info("i: {}", i);

  if(i == 100000) {return std::nullopt; } else {return grid;}

} // function: fine }}}

// fn: h_balance {{{

//
// Inserts intra layer pseudo nodes for binary tree-like drawing
//
template<SignedIntegral T,
  typename P,
  typename S,
  typename A,
  typename L,
  typename U,
  typename G>
decltype(auto) h_balance(T&& root,
  P&& f_pred,
  S&& f_succ,
  A&& f_adj,
  L&& f_link,
  U&& f_unlink,
  G& grid
)
{
  auto layers{grid.first};
  auto vertex_xy{grid.second};

  auto f_dist = [&](auto u, auto v)
  {
    return std::abs(vertex_xy[u].first-vertex_xy[v].first);
  };

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

  // Find distances of each level
  for (i64 i{}; auto const& layer : layers)
  {

    if( static_cast<u64>(i+1) == layers.size() ){ break; }

    // Find max x distance
    i64 px_max{1};
    i64 sx_max{1};
    for (auto node : layer)
    {
      for (auto succ : f_succ(node))
      {
        if( auto dist{f_dist(node,succ)}; dist > sx_max ){ sx_max = dist; }
      } // for

      for (auto pred : f_pred(node))
      {
        if( auto dist{f_dist(pred,node)}; dist > px_max ){ px_max = dist; }
      } // for
    } // for

    if (sx_max > 1)
    {
      for (auto node : layers.at(i))
      {
        for (auto succ : f_succ(node))
        {
          auto new_node{make_pseudo.next()};
          f_link(std::make_pair(node,new_node));
          f_link(std::make_pair(new_node,succ));
          f_unlink(std::make_pair(node,succ));
        } // for
      } // for
    }
    if (px_max > 1)
    {
      for (auto node : layers.at(i))
      {
        for (auto pred : f_pred(node))
        {
          auto new_node{make_pseudo.next()};
          f_link(std::make_pair(pred,new_node));
          f_link(std::make_pair(new_node,node));
          f_unlink(std::make_pair(pred,node));
        } // for
      } // for
    } // if x_max > 1
    ++i;
  } // for

  return grid;
} // function: h_balance }}}

// fn: sa {{{
template<SignedIntegral T,
  typename P,
  typename S,
  typename A,
  typename L,
  typename U,
  typename G>
decltype(auto) sa(T&& root,
  P&& f_pred,
  S&& f_succ,
  A&& f_adj,
  L&& f_link,
  U&& f_unlink,
  G& grid
)
{
  auto& layers{grid.first};
  auto& vertex_xy{grid.second};

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

    for (auto i{sz_dist(gen)}; i > 0; --i)
    {
      _vertex_xy.at(layer.at(sz_dist(gen))).first += r_moves.at(moves_dist(gen));
    } // for

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

  // grid = ns_representations::grid::run(root,f_pred,f_succ,f_adj,f_link,f_unlink);
  layers = ns_minimize::crossings::run(root,f_pred,f_succ,f_adj,f_link,f_unlink);

  for (i64 i{}; i != 1; ++i)
  {
    minimize_cost();
  } // for

  return grid;
} // function: sa }}}

// fn: run {{{
template<SignedIntegral T, typename P, typename S, typename A, typename L, typename U>
auto run(T&& root, P&& f_pred, S&& f_succ, A&& f_adj, L&& f_link, U&& f_unlink)
{
#ifndef NDEBUG
  spdlog::set_level(spdlog::level::debug);
  spdlog::debug("Algorithm: celaeno::graph::representations::grid");
#endif

  //
  // Minimize edge length by pseudo-node relinking
  //

  auto grid {initial(root, f_pred, f_succ, f_adj, f_link, f_unlink)};

  auto f_dist = [&](auto u, auto v)
  {
    grid = initial(root, f_pred, f_succ, f_adj, f_link, f_unlink);
    auto layers = grid.first;
    auto vertex_xy = grid.second;
    return std::abs(vertex_xy[u].first-vertex_xy[v].first);
  };

  ns_minimize::edge_length::run(root,f_pred,f_succ,f_link,f_unlink,f_dist);

  // grid = initial(root, f_pred, f_succ, f_adj, f_link, f_unlink);
  // grid = h_balance(root, f_pred, f_succ, f_adj, f_link, f_unlink,grid);
  // grid = overlap_nodes(root, f_pred, f_succ, f_adj, f_link, f_unlink);
  // grid = expand_grid(grid,3);
  // grid = expand_nodes(root,f_pred,f_succ,f_adj,f_link,f_unlink,grid);
  // grid = fine(root,f_pred,f_succ,f_adj,f_link,f_unlink,grid);
  // return sa(root,f_pred,f_succ,f_adj,f_link,f_unlink,grid);

  // while( ! fine(root,f_pred,f_succ,f_adj,f_link,f_unlink,grid)  )
  // {
  //   grid = initial(root, f_pred, f_succ, f_adj, f_link, f_unlink);
  //   grid = overlap_nodes(root, f_pred, f_succ, f_adj, f_link, f_unlink);
  //   grid = expand_grid(grid,3);
  //   grid = expand_nodes(root,f_pred,f_succ,f_adj,f_link,f_unlink,grid);
  // }

  grid = overlap_nodes(root, f_pred, f_succ, f_adj, f_link, f_unlink);
  grid = expand_grid(grid,3);
  grid = expand_nodes(root,f_pred,f_succ,f_adj,f_link,f_unlink,grid);
  grid = initial(root, f_pred, f_succ, f_adj, f_link, f_unlink);
  fine(root,f_pred,f_succ,f_adj,f_link,f_unlink,grid);
  // grid = overlap_nodes(root, f_pred, f_succ, f_adj, f_link, f_unlink);
  // grid = expand_grid(grid,3);
  // grid = expand_nodes(root,f_pred,f_succ,f_adj,f_link,f_unlink,grid);
  // grid = overlap_nodes(root, f_pred, f_succ, f_adj, f_link, f_unlink);

  return grid;
}
// }}}

} // namespace celaeno::graph::representations::grid }}}
