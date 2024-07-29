// vim: set expandtab fdm=marker ts=2 sw=2 tw=80 et :
//
// @author      : Ruan E. Formigoni (ruanformigoni@gmail.com)
// @file        : crossings
// @created     : Tuesday Sep 28, 2021 20:30:10 -03
//
// BSD 2-Clause License

// Copyright (c) 2021, Ruan Evangelista Formigoni
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

#include <ranges>
#include <map>

#include <range/v3/all.hpp>
#include <fplus/fplus.hpp>

#include <celaeno/aliases.hpp>
#include <celaeno/concepts.hpp>
#include <celaeno/literals.hpp>
#include <celaeno/fun/fun.hpp>
#include <celaeno/fun/macros.hpp>
#include <celaeno/log/log.hpp>

#include <celaeno/graph/io/dot.hpp>
#include <celaeno/graph/search/bfs.hpp>
#include <celaeno/graph/search/a-star.hpp>
#include <celaeno/graph/views/depth.hpp>
#include <celaeno/graph/operations/balance/paths.hpp>

// namespace celaeno::graph::operations::balance::crossings
namespace celaeno::graph::operations::balance::crossings
{

namespace
{

// using namespace
using namespace celaeno::aliases;
using namespace celaeno::concepts;
using namespace celaeno::fun::fn;

// namespaces
namespace rg = ranges;
namespace fp = fplus;
namespace fw = fplus::fwd;

namespace fun = celaeno::fun;
namespace ns_search = celaeno::graph::search;
namespace ns_log = celaeno::log;
namespace ns_io_dot = celaeno::graph::io::dot;

// Aliases
template<SignedIntegral T>
using Cross = std::map<T,T>;
using Position = i64;
using Positions = std::vector<Position>;
using Node = i64;
using Nodes = std::vector<Node>;

}

enum Direction
{
  LEFT,
  RIGHT,
};


// // fn: path_to_parent {{{
// template<typename Node, typename F1, typename F2>
// decltype(auto) path_to_parent(Node&& src, Node&& dst, F1&& f_next, F2&& f_position_in_layer)
// {
//   auto path = ns_search::a_star::run(src
//     , dst
//     , [&](auto&& e) { return ( e == src )? fn(f_next(e)).dif(Nodes{dst}).vec() : f_next(e); }
//     , [](auto&&){ return false; }
//     , [](auto&&, auto&&){ return 1; }
//   );
//
//   log::info()("Path from {} to {} is: ", src, dst);
//
//   if ( path )
//   {
//     log::info()("-- {}", *path);
//     path->erase(path->begin());
//     auto view_position = *path | std::views::transform(f_position_in_layer);
//     auto position_max = std::ranges::max_element(view_position);
//     auto position_src = f_position_in_layer(src);
//     return ( *position_max < position_src )? Direction::RIGHT : Direction::LEFT;
//   } // if
//
//   return ( f_position_in_layer(src) < f_position_in_layer(dst) )? Direction::RIGHT : Direction::LEFT;
//
//   // auto f_position_in_layer = [&](auto&& e)
//   // {
//   //   auto const& layer = map_layer_nodes.at(map_node_layer.at(e));
//   //   return std::distance(layer.begin(), std::find(layer.begin(), layer.end(), e));
//   // };
//   // path_to_parent(u, w, ops.preds, f_position_in_layer);
//   //
// } // }}}

// // fn: restructure_indices {{{
// void restructure_indices(Ops const& ops
//   , Range auto&& layers
//   , Range auto&& nodes_collapsed
//   , Range auto&& positions_collapsed)
// {
//   for (auto&& layer : layers)
//   {
//     for(auto window : layer | std::views::slide(2))
//     {
//       auto u = *window.begin();
//       auto v = *std::next(window.begin());
//       auto uv_pred = fn(ops.preds(u)).in(ops.preds(v)).vec();
//       // Check if uv have a common parent
//       if ( uv_pred.size() > 0 )
//       {
//         // Parent of both u and v
//         auto p = uv_pred.at(0);
//         // Find index of u, which is the node to the left of parent
//         auto index_u = std::distance(nodes_collapsed.begin()
//           , std::find(nodes_collapsed.begin(), nodes_collapsed.end(), u)
//         );
//         auto index_v = std::distance(nodes_collapsed.begin()
//           , std::find(nodes_collapsed.begin(), nodes_collapsed.end(), v)
//         );
//         // Decrease by 1 all positions previous to u (inclusive)
//         std::for_each(positions_collapsed.begin()
//           , positions_collapsed.begin()+index_u
//           , [](auto& e){ e -= 1; }
//         );
//         // Adjust index of parent
//         auto index_p = std::distance(nodes_collapsed.begin()
//           , std::find(nodes_collapsed.begin(), nodes_collapsed.end(), p)
//         );
//         positions_collapsed.at(index_p) = (positions_collapsed.at(index_u) + positions_collapsed.at(index_v)) / 2;
//       } // if
//       fmt::println("{} : {}", u, v);
//     } // for
//   } // for
// } // fn: restructure_indices }}}

// fn: create_crossing_layers {{{
template<Range R1, Range R2>
decltype(auto) create_crossing_layers(Ops const& ops, R1&& r1, R2&& rc)
{
  using Node = typename std::remove_cvref_t<R1>::value_type;

  // Return a map of layer to node vector
  std::map<i64,std::vector<Node>> map_layer_nodes;

  // Mark initial layer as visited
  std::set<Node> set_visited_node(r1.begin(), r1.end());

  // Transforms layer into predecessors
  auto f_as_predecessors = [&](Ops const& _1_ops, auto&& _1_r1, auto&& _1_rc)
  {
    return fn(_1_r1)
      // Transforms the current layer into the predecessor layer
      .as(LR(_1_ops.preds(_1))).squash().sort().unique()
      // Keep only crossing nodes
      .in(_1_rc)
      // Keep nodes which successors are all part of the set_visited_node set
      .keep(LR(fn(_1_ops.succs(_1)).dif(set_visited_node).vec().empty()))
      .vec();
  };

  // Create additional layers going down
  auto layer_crossings = f_as_predecessors(ops, r1, rc);

  // Insert current layer nodes into visited set
  set_visited_node.insert(layer_crossings.begin(), layer_crossings.end());

  // Keep going until it hits no predecessor crossings
  for (i64 idx_map_layer_nodes{}; ! layer_crossings.empty(); )
  {
    map_layer_nodes[++idx_map_layer_nodes] = layer_crossings;
    layer_crossings = f_as_predecessors(ops, layer_crossings, rc);
    set_visited_node.insert(layer_crossings.begin(), layer_crossings.end());
  } // for

  return map_layer_nodes;
} // function: create_crossing_layers }}}

// fn: abs_diff_mean_ceil {{{
inline double abs_diff_mean_ceil(double a, double b)
{
  return std::ceil(
    fp::abs_diff(
        static_cast<double>(a)
      , static_cast<double>(b)
    ) / 2.0);
} // }}}

// fn: abs_diff_mean_ceil {{{
inline double abs_diff_mean_floor(double a, double b)
{
  return std::floor(
    fp::abs_diff(
        static_cast<double>(a)
      , static_cast<double>(b)
    ) / 2.0);
} // }}}

// fn: push_nodes {{{
void push_nodes(Node node
  , Direction direction
  , Range auto& positions_collapsed
  , Range auto& nodes_collapsed)
{
  auto distance_node = std::distance(nodes_collapsed.begin(), std::find(nodes_collapsed.begin(), nodes_collapsed.end(), node));
  auto position_node = positions_collapsed.at(distance_node);

  if ( direction == Direction::LEFT )
  { 
    i64 offset = 1;
    std::for_each(positions_collapsed.begin()
      , positions_collapsed.begin() + distance_node + 1
      , [&](auto& e){ e -= offset; });
  } // if
  else
  {
    i64 offset = 1;
    std::for_each(positions_collapsed.begin() + distance_node
      , positions_collapsed.end()
      , [&](auto& e){ e += offset; });
  } // if
} // }}}

// fn: slide_backwards {{{
// Decrease distance until the stopper node is hit of a position less than
// 'position' is found
decltype(auto) slide_backwards(Node stopper
  , i64 distance
  , i64 position
  , Range auto&& nodes_collapsed
  , Range auto&& positions_collapsed
)
{
  // Keep going back until distance of u is equal or greater than w
  // Or hits p1
  while ( nodes_collapsed.at(distance) != stopper and position < positions_collapsed.at(distance) )
  {
    --distance;
  } // while
  // if ( nodes_collapsed.at(distance) == stopper )
  // {
  //   ++distance;
  // } // if
  return distance;
} // }}}

// // fn: slide_forwards {{{
// // Decrease distance until the stopper node is hit of a position less than
// // 'position' is found
// decltype(auto) slide_forwards(Node stopper
//   , i64 distance
//   , i64 position
//   , Range auto&& nodes_collapsed
//   , Range auto&& positions_collapsed
// )
// {
//   // Keep going forward until distance of u is less than w
//   // Or hits p1
//   while ( position < positions_collapsed.at(distance) )
//   {
//     ++distance;
//   } // while
//
//   return distance;
// } // }}}

// fn: insert_rightbefore {{{
template<typename Node>
void insert_rightbefore(Node u
  , Node p
  , Range auto&& layer
  , Range auto& positions_collapsed
  , Range auto& nodes_collapsed
  , std::optional<i64> opt_position = std::nullopt)
{
  auto distance_p = std::distance(nodes_collapsed.begin(), std::ranges::find(nodes_collapsed, p));
  auto position_p = (opt_position)? *opt_position : positions_collapsed.at(distance_p);
  nodes_collapsed.insert(nodes_collapsed.begin()+distance_p, u);
  positions_collapsed.insert(positions_collapsed.begin()+distance_p, position_p);
} // }}}

// fn: insert_rightafter {{{
template<typename Node>
void insert_rightafter(Node u
  , Node p
  , Range auto&& layer
  , Range auto& positions_collapsed
  , Range auto& nodes_collapsed
  , std::optional<i64> opt_position = std::nullopt)
{
  auto distance_p = std::distance(nodes_collapsed.begin(), std::ranges::find(nodes_collapsed, p));
  auto position_p = (opt_position)? *opt_position : positions_collapsed.at(distance_p);
  ++distance_p;
  nodes_collapsed.insert(nodes_collapsed.begin()+distance_p, u);
  positions_collapsed.insert(positions_collapsed.begin()+distance_p, position_p);
} // }}}

// fn: insert_before {{{
template<typename Node>
void insert_before(Node u
  , Node p
  , Range auto&& layer
  , Range auto& positions_collapsed
  , Range auto& nodes_collapsed
  , std::optional<i64> opt_position = std::nullopt)
{
  // Get x-coordinate position of p
  auto distance_p = std::distance(nodes_collapsed.begin(), std::ranges::find(nodes_collapsed, p));
  auto position_p = (opt_position)? *opt_position : positions_collapsed.at(distance_p);

  // Put as the leftmost element with the same position in the array
  // node: [-11,-10,-13]
  // pos : [ 1 , 2 , 2 ]
  //               < u
  //             u
  while (distance_p > 0 and positions_collapsed.at(distance_p) >= position_p)
  {
    --distance_p;
  } // while

  --position_p;

  // Skip past the previous (by layer ordering) nodes in the same layer
  // E.g. In layer {w,x,y,u,z}, {w,x,y} should be skipped for u to be after y
  while(std::ranges::find_first_of(
      std::ranges::subrange(nodes_collapsed.begin()+distance_p, nodes_collapsed.end())
    , std::ranges::subrange(layer.begin(), std::find(layer.begin(), layer.end(), u)))
    != std::ranges::end(nodes_collapsed))
  {
    ++distance_p;
  } // while

  nodes_collapsed.insert(nodes_collapsed.begin()+distance_p, u);
  positions_collapsed.insert(positions_collapsed.begin()+distance_p, position_p);
} // }}}

// fn: insert_after {{{
void insert_after(auto&& u
  , auto&& p
  , Range auto& layer
  , Range auto& positions_collapsed
  , Range auto& nodes_collapsed
  , std::optional<i64> position = std::nullopt)
{
  // Get x-coordinate position of p
  auto distance_p = std::distance(nodes_collapsed.begin(), std::ranges::find(nodes_collapsed, p));
  auto position_p = positions_collapsed.at(distance_p);

  // // Skip past the previous (by layer ordering) nodes in the same layer
  // // E.g. In layer {w,x,y,u,z}, {w,x,y} should be skipped for u to be after y
  // while(std::ranges::find_first_of(
  //     std::ranges::subrange(nodes_collapsed.begin()+index_insert, nodes_collapsed.end())
  //   , std::ranges::subrange(layer.begin(), std::find(layer.begin(), layer.end(), u)))
  //   != std::ranges::end(nodes_collapsed))
  // {
  //   ++index_insert;
  // } // while

  nodes_collapsed.insert(nodes_collapsed.begin()+distance_p+1, u);
  positions_collapsed.insert(positions_collapsed.begin()+distance_p+1
    , (position)? *position : position_p+1
  );
} // }}}

// fn: insert_between {{{
template<typename Node>
void insert_between(Node u
  , Node p1
  , Node p2
  , Range auto&& map_node_layer
  , Range auto& positions_collapsed
  , Range auto& nodes_collapsed
  , std::optional<i64> position = std::nullopt)
{
  // Get index of p1 and p2
  auto distance_p1 = std::distance(nodes_collapsed.begin(), std::ranges::find(nodes_collapsed, p1));
  auto distance_p2 = std::distance(nodes_collapsed.begin(), std::ranges::find(nodes_collapsed, p2));
  auto position_p1 = positions_collapsed.at(distance_p1);
  auto position_p2 = positions_collapsed.at(distance_p2);

  // Assume p1 as the node the appears first on nodes_collapsed
  if ( *std::ranges::find_first_of(nodes_collapsed, Nodes{p1,p2}) == p2 )
  {
    std::swap(p1, p2);
    std::swap(distance_p1, distance_p2);
    std::swap(position_p1, position_p2);
  } // if

  // Check if both parents are in the same position
  if ( position_p1 == position_p2 )
  {
    push_nodes((map_node_layer.at(p1) < map_node_layer.at(p2))? p1 : p2
      , Direction::RIGHT
      , positions_collapsed
      , nodes_collapsed);
    distance_p1 = std::distance(nodes_collapsed.begin(), std::ranges::find(nodes_collapsed, p1));
    distance_p2 = std::distance(nodes_collapsed.begin(), std::ranges::find(nodes_collapsed, p2));
    position_p1 = positions_collapsed.at(distance_p1);
    position_p2 = positions_collapsed.at(distance_p2);
  } // if

  // Insert right first second parent
  i64 position_u{};
  i64 distance_u{};

  // Check if parents are on distinct layers
  if ( map_node_layer.at(p2) > map_node_layer.at(p1) )
  {
    // Put right before p1
    if ( std::abs(position_p1 - position_p2) < 2 )
    {
      position_u = position_p1;
      distance_u = distance_p1;
      // distance_u = distance_p2;
      // distance_u = slide_backwards(p1, distance_u, position_u, nodes_collapsed, positions_collapsed);
    }
    // Put between p1 and p2
    else
    {
      position_u = std::ceil((static_cast<double>(position_p1) + position_p2) / 2.0);
      distance_u = distance_p2;
      distance_u = slide_backwards(p1, distance_u, position_u, nodes_collapsed, positions_collapsed);
    } // else
  } // if
  // Put to the left of p2
  else if ( map_node_layer.at(p2) < map_node_layer.at(p1) )
  {
    // Put on top of p2
    if ( std::abs(position_p1 - position_p2) < 2 )
    {
      position_u = position_p2;
      distance_u = distance_p2;
      distance_u = slide_backwards(p1, distance_u, position_u, nodes_collapsed, positions_collapsed);
    }
    // Put between p1 and p2
    else
    {
      position_u = std::ceil((static_cast<double>(position_p1) + position_p2) / 2.0);
      distance_u = distance_p2;
      distance_u = slide_backwards(p1, distance_u, position_u, nodes_collapsed, positions_collapsed);
    } // else
  } // else if 
  else
  {
    // Put on top of p2
    if ( std::abs(position_p1 - position_p2) < 2 )
    {
      push_nodes((map_node_layer.at(p1) < map_node_layer.at(p2))? p1 : p2
        , Direction::RIGHT
        , positions_collapsed
        , nodes_collapsed);
      distance_p1 = std::distance(nodes_collapsed.begin(), std::ranges::find(nodes_collapsed, p1));
      distance_p2 = std::distance(nodes_collapsed.begin(), std::ranges::find(nodes_collapsed, p2));
      position_p1 = positions_collapsed.at(distance_p1);
      position_p2 = positions_collapsed.at(distance_p2);
    }
    // Put between p1 and p2
    position_u = std::ceil((static_cast<double>(position_p1) + position_p2) / 2.0);
    distance_u = distance_p2;
    distance_u = slide_backwards(p1, distance_u, position_u, nodes_collapsed, positions_collapsed);
  } // else


  nodes_collapsed.insert(nodes_collapsed.begin()+distance_u, u);
  positions_collapsed.insert(positions_collapsed.begin()+distance_u
    , (position)? *position : position_u
  );
} // }}}

// fn: keep_sequential {{{
bool keep_sequential(bool is_left
  , Range auto& map_node_layer
  , Range auto& positions_collapsed
  , Range auto& nodes_collapsed)
{
  // 2.6 Adjust positions to only contain an increasing sequence
  auto zipped_position_node = std::views::zip(
      std::views::slide(positions_collapsed, 2)
    , std::views::slide(nodes_collapsed, 2)
  );

  // 2.6.1 Check if insertion caused the sequence to be non-increasing
  enum class NonSequentialType
  {
    LESS_THAN,
    EQUAL,
  };

  NonSequentialType non_sequential_type;
  auto it_not_sequential = std::ranges::find_if(zipped_position_node, [&](auto&& e)
  {
    auto rng_positions = std::get<0>(e);
    auto rng_nodes = std::get<1>(e);
    // Found a non-sequential pair
    if (rng_positions[1] < rng_positions[0])
    {
      non_sequential_type = NonSequentialType::LESS_THAN;
      return true;
    } // if
    // It is ok to be the same as long as it is not between nodes on the same layer
    if (rng_positions[1] == rng_positions[0] and map_node_layer.at(rng_nodes[0]) == map_node_layer.at(rng_nodes[1]))
    {
      non_sequential_type = NonSequentialType::EQUAL;
      return true;
    } // if
    return false;
  });

  if ( it_not_sequential == std::ranges::end(zipped_position_node) )
  {
    return false;
  } // if

  auto rng_positions = std::get<0>(*it_not_sequential);
  auto rng_nodes = std::get<1>(*it_not_sequential);

  // Found a non-sequential pair
  int64_t amount{};
  if ( non_sequential_type == NonSequentialType::LESS_THAN )
  {
    // Define the adjustment amount as the ceil of the mean of the absolute difference of positions
    amount = abs_diff_mean_ceil(rng_positions[0], rng_positions[1]);
  } // if
  // It is ok to be the same as long as it is not between nodes on the same layer
  else if ( non_sequential_type == NonSequentialType::EQUAL )
  {
    amount = 1;
  } // else if

  // 2.6.2 Check if should increase the sequence forwards or backwards
  // Decrease backwards if configuration is to the left
  // Increase forwards if configuration is to the right
  for(auto it=std::ranges::begin(zipped_position_node); it != std::next(it_not_sequential); ++it)
  {
    std::get<0>(*it)[0] -= amount;
  } // for
  for(auto it=it_not_sequential; it != std::ranges::end(zipped_position_node); ++it)
  {
    std::get<0>(*it)[1] += amount;
  } // for

  return true;
} // }}}

// fn: predecessor_single {{{
void predecessor_single(Node u
  , auto&& ops
  , Map auto&& map_node_layer
  , Map auto&& map_layer_nodes
  , Map auto&& map_node_direction
  , Range auto&& nodes_collapsed
  , Range auto&& positions_collapsed
  , Range auto&& layer)
{
  auto preds = ops.preds(u);

  Node p = preds.at(0);
  i64 distance_p = std::distance(nodes_collapsed.begin(), std::find(nodes_collapsed.begin(), nodes_collapsed.end(), p));
  i64 position_p = positions_collapsed.at(distance_p);

  // Get successor of p, v, that is not u
  //  u v
  //  | |
  //   p
  Node v = fn(ops.succs(p))
    .dif(std::vector<Node>{u})
    .test([&](auto&& e){ return e.size() == 1; }, "Size of successors of p is not 1")
    .vec()
    .at(0);

  // vpu or upv
  bool is_left = *std::ranges::find_first_of(nodes_collapsed, Nodes{p,v}) == p;

  // // The current node has only one predecessor
  // // So it must have two successors
  // // Use the successors with a bfs downwards (predecessors) the first node w
  // // contained in nodes_collapsed
  // // If w has a position smaller than p, put u before p, else put u after p
  // // s1 s2
  // //  | |
  // //   u
  // //   |
  // //   p
  // auto succs = ops.succs(u);
  // assert(succs.size() == 2);
  // // Successors of u
  // Nodes collapsed_predecessors;
  // auto f_collapsed_successor = [&](auto&& e)
  // {
  //   if ( std::ranges::find(nodes_collapsed, e) == std::ranges::end(nodes_collapsed) ) { return false; }
  //   collapsed_predecessors.push_back(e);
  //   return true;
  // };
  // auto f_preds = [&](auto&& e)
  // {
  //   return fn(ops.preds(e)).dif(Nodes{u}).vec();
  // };
  // // Populate collapsed_predecessors with the predecessors of successors
  // ns_search::bfs::run(succs.at(0), f_preds, f_preds, f_collapsed_successor);
  // ns_search::bfs::run(succs.at(1), f_preds, f_preds, f_collapsed_successor);
  // // Sort by layer index, the one in the highest layer will be at the end
  // std::ranges::sort(collapsed_predecessors, {}, [&](auto&& e){ return map_node_layer.at(e); });
  //
  // if ( not collapsed_predecessors.empty() )
  // {
  //   Node w = collapsed_predecessors.back();
  //   i64 distance_w = std::distance(nodes_collapsed.begin(), std::find(nodes_collapsed.begin(), nodes_collapsed.end(), w));
  //   i64 position_w = positions_collapsed.at(distance_w);
  //   is_left = position_w < position_p;
  // } // if

  if (is_left)
  {
    insert_before(u, p, layer, positions_collapsed, nodes_collapsed);
  } // if
  // Else if the order is vp, put u after p, i.e.: vpu
  else
  {
    insert_after(u, p, layer, positions_collapsed, nodes_collapsed);
  } // else

  // if ( std::ranges::find(nodes_collapsed, v) == std::ranges::end(nodes_collapsed) )
  // {
  //   auto f_position_in_layer = [&](auto&& e)
  //   {
  //     auto const& layer = map_layer_nodes.at(map_node_layer.at(e));
  //     return std::distance(layer.begin(), std::find(layer.begin(), layer.end(), e));
  //   };
  //   is_left = f_position_in_layer(u) < f_position_in_layer(v);
  // } // if


  while(keep_sequential(is_left, map_node_layer, positions_collapsed, nodes_collapsed)) {};
} // }}}

// fn: predecessor_immediate_single {{{
void predecessor_immediate_single(Node u
  , auto&& ops
  , Map auto&& map_node_layer
  , Map auto&& map_layer_nodes
  , Map auto&& map_node_direction
  , Range auto&& preds_in_layer
  , Range auto&& nodes_collapsed
  , Range auto&& positions_collapsed
  , Range auto&& layer)
{
  enum class Config
  {
    FST,
    SND,
  };

  // Current possibilities
  //        FST         SND
  //       p  |        |  p
  //          w        w

  // {p,w}
  auto preds = ops.preds(u);

  // {p}
  Node p = preds_in_layer.at(0);
  auto position_p = std::distance(nodes_collapsed.begin(), std::find(nodes_collapsed.begin(), nodes_collapsed.end(), p));

  // {p,w} - {p} == {w}
  Node w = fn(preds).dif(preds_in_layer).vec().at(0);
  auto position_w = std::distance(nodes_collapsed.begin(), std::find(nodes_collapsed.begin(), nodes_collapsed.end(), w));

  // Determine the current configuration of nodes
  Config config = (*std::ranges::find_first_of(nodes_collapsed, Nodes{p,w}) == p)? Config::FST : Config::SND;

  // If the order is pw, put u after p, i.e.: puw
  // if ( config == Config::FST )
  // {
  //   insert_rightafter(u, w, layer, positions_collapsed, nodes_collapsed);
  //   // insert_between(u, p, w, map_node_layer, positions_collapsed, nodes_collapsed);
  // }
  // else
  // {
  //   insert_rightbefore(u, w, layer, positions_collapsed, nodes_collapsed);
  // } // else
  // else
  // {
  //   insert_after(u, w, layer, positions_collapsed, nodes_collapsed);
  // } // else

  insert_between(u, p, w, map_node_layer, positions_collapsed, nodes_collapsed);

  while(keep_sequential(config == Config::FST, map_node_layer, positions_collapsed, nodes_collapsed)) {};
} // }}}

// fn: predecessor_multiple {{{
void predecessor_multiple(Node u
  , auto&& ops
  , Map auto&& map_node_layer
  , Range auto&& nodes_collapsed
  , Range auto&& positions_collapsed)
{
  auto preds = ops.preds(u);

  // 3.1 The position of u is k=(p1+p2)/2
  Node p1, p2;
  if ( *std::ranges::find_first_of(nodes_collapsed, preds) == preds.at(0) )
  {
    p1 = preds.at(0);
    p2 = preds.at(1);
  } // if
  else
  {
    p1 = preds.at(1);
    p2 = preds.at(0);
  } // else
  auto distance_p1 = std::distance(nodes_collapsed.begin(), std::ranges::find(nodes_collapsed, p1));
  auto distance_p2 = std::distance(nodes_collapsed.begin(), std::ranges::find(nodes_collapsed, p2));
  auto f_calculate_k = [&] -> double
  {
    auto position_p1 = positions_collapsed.at(distance_p1);
    auto position_p2 = positions_collapsed.at(distance_p2);
    return (static_cast<double>(position_p1) + position_p2) / 2.0;
  };
  double k = f_calculate_k();
  
  // 3.2 Find the position l that is greater or equal to k in positions_collapsed
  auto it = std::find_if(positions_collapsed.begin() + std::min(distance_p1, distance_p2)
    , positions_collapsed.end()
    , fun::unary::greater_equal(std::floor(k)));
  log::err({ it != positions_collapsed.end() })("Failed to find greate_equal position");
  auto index_insert = std::distance(positions_collapsed.begin(), it) + 1;

  // 3.5 Increment following positions by 1 if abs_diff(distance_p2, distance_p2) < 2
  // This is to set node 'u' in-between p1 and p2
  //      u
  //     | |
  //    p1  p2
  // bool is_left = true;
  auto position_p1 = positions_collapsed.at(distance_p1);
  auto position_p2 = positions_collapsed.at(distance_p2);
  if ( std::round(k) != k
    and map_node_layer.at(p1) == map_node_layer.at(p2)
    and std::abs(position_p1 - position_p2) < 2)
  {
    auto dist_fst_half = std::distance(positions_collapsed.begin(), positions_collapsed.begin() + std::max(distance_p1, distance_p2) + 1);
    auto dist_snd_half = std::distance(positions_collapsed.begin() + std::max(distance_p1, distance_p2), positions_collapsed.end());

    // i64 offset = (position_p1 == position_p2)? 2 : 1;
    if ( dist_fst_half < dist_snd_half )
    {
      std::for_each(positions_collapsed.begin() + distance_p2
        , positions_collapsed.end()
        , [&](auto& e){ e += 1; });
    } // if
    else
    {
      std::for_each(positions_collapsed.begin()
        , positions_collapsed.begin() + distance_p1 + 1 // inclusive for p1
        , [&](auto& e){ e -= 1; });
    } // else
    k = f_calculate_k();
    assert(std::round(k) == k);
  } // if

  // 3.3 Put k in position l of positions_collapsed
  positions_collapsed.insert(positions_collapsed.begin()+index_insert, std::floor(k));

  // 3.4 Put u in position l of nodes_collapsed
  nodes_collapsed.insert(nodes_collapsed.begin()+index_insert, u);
} // }}}

// fn: print_node_positions {{{
__attribute__ ((noinline)) auto print_node_positions(Range auto&& nodes_collapsed
    , Range auto&& positions_collapsed)
{
  for (auto&& zipped : std::views::zip(nodes_collapsed, positions_collapsed))
  {
    std::cout << "[" << std::get<0>(zipped) << "," << std::get<1>(zipped) << "]\n";
  } // for
} // }}}

// fn: write_dot {{{
__attribute__ ((noinline)) void write_dot(Ops const& ops
    , auto&& view
    , Range auto&& nodes_collapsed
    , Range auto&& positions_collapsed)
{
  auto map_node_position = fn(nodes_collapsed)
    .zip(positions_collapsed)
    .as([&](auto&& e){ return std::make_pair(e.first, std::make_pair(e.second, view.nl.at(e.first))); })
    .map();

  // Write to file
  ns_io_dot::Writer(view, ops, "out/test-out.dot", map_node_position);
} // }}}

// view_collapse {{{
template<typename V, typename Map>
decltype(auto) view_collapse(Ops const& ops
  , V&& view
  , Map&& map_node_direction)
// Requires ordered map
{
  auto const& map_layer_nodes  = view.ln;
  auto const& map_node_layer = view.nl;

  log::err( { ! map_layer_nodes.empty() } )("Empty view");

  auto f_is_leaf = [&](Node node)
  {
    return ops.succs(node).size() == 0 or ops.preds(node).size() == 0;
  };

  // Fetch lowest layer id
  i64 id_lowest_layer = fn(map_layer_nodes).key().min();

  // Assume that the first layer only contains inputs
  auto f_is_input = LR(ops.preds(_1).empty());
  log::err( { fn(map_layer_nodes.at(id_lowest_layer)).all(LR(f_is_input(_1))) } )
    ("First layer must only contain inputs");

  // Initialize collapse nodes and positions with first layer
  Nodes nodes_collapsed = map_layer_nodes.at(id_lowest_layer);
  Positions positions_collapsed = fp::numbers(i64{}, static_cast<i64>(nodes_collapsed.size()));

  // print_node_positions(nodes_collapsed, positions_collapsed);

  bool stop = false;
  for (auto it_entry{std::next(map_layer_nodes.begin())}; it_entry != map_layer_nodes.end(); ++it_entry)
  {
    auto layer = it_entry->second;

    if ( stop ) { break; }

    for (auto&& u : layer)
    {
      write_dot(ops, view, nodes_collapsed, positions_collapsed);

      // Skip inputs / outputs
      if ( f_is_leaf(u) )
      {
        continue;
      } // if

      // if ( u == -38 ) { stop = true; break; }

      // restructure_indices(std::ranges::subrange(map_layer_nodes.begin(), it_entry) | std::views::values);

      // 1. Check for predecessors of u in nodes collapsed
      auto preds = ops.preds(u);
      auto preds_in_layer = fn(preds).in(std::prev(it_entry)->second).vec();

      log::err({preds.size() != 0})("Empty preds for {}"_fmt(u));

      // 2. If there is only 1 predecessor p
      if ( preds.size() == 1 )
      {
        predecessor_single(u, ops, map_node_layer, map_layer_nodes, map_node_direction, nodes_collapsed, positions_collapsed, layer);
        continue;
      } // if

      // if (preds_in_layer.size() == 1 and ops.succs(preds_in_layer.at(0)).size() > 1)
      if (preds_in_layer.size() == 1)
      {
        predecessor_immediate_single(u
            , ops
            , map_node_layer
            , map_layer_nodes
            , map_node_direction
            , preds_in_layer
            , nodes_collapsed
            , positions_collapsed
            , layer);
        continue;
      } // if

      // 3. If there is 2 predecessors p1 and p2 in nodes_collapsed
      if ( preds.size() == 2 )
      {
        predecessor_multiple(u, ops, map_node_layer, nodes_collapsed, positions_collapsed);
        continue;
      } // if

      log::err()("Invalid preds size of {}"_fmt(preds.size()));
    } // for
  } // for

  return std::make_pair(nodes_collapsed, positions_collapsed);
} // function: view_collapse }}}

// fn: run {{{
template<SignedIntegral T = i64, Range R>
std::pair<std::map<T,Cross<T>>, std::map<Node,Direction>> run(i64 idx_l2, R&& l1, R&& l2, Ops const& ops)
{
  [[maybe_unused]] ns_log::Timer timer("celaeno::graph::operations::balance::crossings");

  // Return cross id → Crossed nodes [src,dest]
  std::map<i64, Cross<i64>> out;

  std::map<Node,Direction> map_node_direction;

  // Keep crossings in their layers
  std::map<i64, std::vector<i64>> map_layer_crossings;
  std::map<i64, i64> map_crossing_layer;

  // Keep track of crossing ids
  std::set<T> set_id_crossings;

  // Check if node is crossing
  auto f_is_crossing = [&](T u){ return set_id_crossings.contains(u); };

  // Dummy vertex with lowest value
  i64 idx{};

  // Define function to compare values lt 0
  auto f_lowest = [&idx](auto&& e) { if(e < idx){ idx=e; } return false; };

  // Get the dummy vertex with the lowest value
  ns_search::bfs::run(0,ops.preds,ops.succs,f_lowest);

  //
  // Index by vertices positions in layer l2
  //
  std::map<T,T> m_id_node;
  std::map<T,T> m_node_id;
  rg::for_each(fp::numbers({},l2.size()),[&](T n) mutable
  {
    m_id_node[n] = l2.at(n);
    m_node_id.emplace(l2.at(n),n);
  });


  //
  // Create vector of id ordered successors, for all vertices of l2
  //
  std::vector<std::vector<T>> succs;

  std::ranges::for_each(l1,
  [&](auto u)
  {
    // Get successors of u, sort and transform in m_node_id
    succs.emplace_back(fw::apply(ops.succs(u)
      , fw::sort_by([&](auto a, auto b){ return m_node_id.at(a) < m_node_id.at(b); })
      , fw::transform([&](auto v){ return static_cast<T>(m_node_id.at(v)); })
    ));
  });

  //
  // For each level 1..n, merge with level 0, to compute crossings
  //

  // Test for empty successors vec
  log::err({!succs.empty()})("Empty successors map!");

  // First vector accumulates all values
  auto& acc{*succs.begin()};

  // Save parents of nodes in acc
  std::vector<i64> parents;

  // Initialize parents of acc nodes
  for(size_t i=0; i < acc.size(); ++i)
  {
    parents.push_back(l1.at(0));
  } // for

  i64 i{1};
  for (auto it1{std::next(succs.begin())}; it1 != succs.end(); ++it1)
  {
    // For each value of current vector
    for (auto it2{it1->begin()}; it2 != it1->end(); ++it2)
    {
      // Find a position in the accumulator
      auto it_search{ std::ranges::find_if(acc,[&](auto e){ return *it2 < e; }) };

      // If pos != end, then the number of crossings equals the distance of
      // end - current position
      if ( it_search != std::ranges::end(acc) )
      {
        // Get parent
        i64 parent {l1.at(i)};

        // Get child id
        i64 id_child{*it2};

        // Get child node from id
        i64 child{m_id_node.at(id_child)};

        // Save direction of crossing
        map_node_direction[parent] = Direction::RIGHT;
        map_node_direction[child] = Direction::LEFT;

        /*  Insert child in acc
         *    o  p
         *     \
         *      \
         *    c  o
         */
        auto it_ins{acc.insert(it_search,id_child)};

        // Get position of child insertion
        i64 i_position {std::distance(acc.begin(),it_ins)};

        /*  Insert parent
         *    o  p
         *     \
         *      \
         *    c  o
         */
        parents.insert(parents.begin()+i_position, parent);

        /* Check crossing edges in acc
         *   o  p
         *    \/
         *    /\
         *   c  o
         */
        for (auto it3{std::prev(acc.end())}; it3 != it_ins; --it3)
        {
          // Create crossing map
          Cross<T> cross;

          // Create dummy
          --idx;

          /* Get child & parent of intersection
           *  np p
           *   \/
           *   /\
           *  c  nc
          */
          i64 n_id_child{*it3};
          i64 n_child{m_id_node.at(n_id_child)};
          i64 n_parent{parents.at(std::distance(acc.begin(),it3))};

          // Save direction of crossing
          map_node_direction[n_parent] = Direction::LEFT;
          map_node_direction[n_child] = Direction::RIGHT;

          // Link parent with dummy
          /*  np  p
           *  \\
           *   \d 
           *    \
           *  c   nc
          */
          ops.link(n_parent,idx);
          /* Link dummy with child
          *   np  p
          *   \\
          *    \d 
          *     \\
          *   c   nc
          */
          ops.link(idx,n_child);
          /* Unlink parent & child
           *  np  p
           *   \
           *    d 
           *     \
           *  c   nc
          */
          ops.unlink(n_parent,n_child);

          log::err({! f_is_crossing(n_child)})
            ("Set id crossings must not contain n_child");

          // Update children of previous crossing
          if( f_is_crossing(n_parent) )
          {
            for (auto& [u,v] : out[n_parent])
            {
              if( v == n_child ) { v = idx; } // if
            } // for
          } // if

          /* Update parent of child
           *     np     |  np 
           *     |\     |  |  
           *     d |    |  d  
           *     |/     |  | 
           *     nc     |  nc
          */
          parents.at(std::distance(acc.begin(),it3)) = idx;

          /* Link parent with dummy
           *  np  p
           *   \ //
           *    d/
           *    / \
           *  c   nc
          */
          ops.link(parent,idx);
          /* Link dummy with child
           *  np  p
           *   \ //
           *    d/
           *   // \
           *  c   nc
          */
          ops.link(idx,child);
          /* Unlink parent & child
           *  np  p
           *   \ /
           *    d
           *   / \
           *  c   nc
          */
          ops.unlink(parent,child);

          log::err({! f_is_crossing(n_child)})
            ("Set id crossings must not contain n_child");

          /* Update children in result
           *  np  p
           *   \ /
           *    d
           *   / \
           *  c   nc
          */
          if( f_is_crossing(parent) )
          {
            for (auto& [u,v] : out[parent])
            {
              if( v == child ) { v = idx; } // if
            } // for
          } // if

          // Save in crossing map
          cross[n_parent] = n_child;
          cross[parent] = child;

          // Save crossing in layer → crossing map
          auto f_next_layer = LR(map_crossing_layer.contains(_1)? map_crossing_layer.at(_1)-1 : idx_l2-1);
          map_layer_crossings[f_next_layer(parent)].push_back(idx);
          map_crossing_layer[idx] = f_next_layer(parent);

          // Update parent of child
          parents.at(i_position) = idx;
          parent = idx;

          // Save in out
          out[idx] = cross;

        } // for

      } // if
      else
      {
        // Push back parent
        parents.push_back(l1.at(i));
        // Push back node
        acc.push_back(*it2);
      } // else
    } // for
    ++i;
  } // for

  return std::make_pair(out,map_node_direction);
} // function: run }}}

// fn: run {{{
template<SignedIntegral T = i64, typename V>
decltype(auto) run(Ops const& ops, V& view, std::map<Node, Direction>& map_node_direction)
{
  // Create a depth view
  auto const& [ln,nl] = view;

  // Create overlapping layer indices
  auto layers = fp::overlapping_pairs(fp::numbers({},ln.size()));

  // Create new layers from crossings
  using MapLayerNodes = std::remove_cvref_t<decltype(ln)>;
  MapLayerNodes map_layer_nodes;

  // Push initial layer new view
  i64 idx_map_layer_nodes {};
  map_layer_nodes[idx_map_layer_nodes] = ln.at(idx_map_layer_nodes);

  for (auto const& [i,j] : layers)
  {
    // Fetch adjacent layers
    auto&& [n1,n2] = std::tie(ln.at(i), ln.at(j));

    // Calculate crossings
    auto result = run(j,n1,n2,ops);

    // Save crossings as new layers of the graph
    auto map_layer_crossings = create_crossing_layers(ops, n2, result.first);

    for(auto [k,v] : result.second)
    {
      map_node_direction[k] = v;
    } // for

    for (auto& e : map_layer_crossings) { std::ranges::reverse(e.second); } // for
    for (auto&& it = map_layer_crossings.rbegin(); it != map_layer_crossings.rend(); ++it)
    {
      map_layer_nodes[++idx_map_layer_nodes] = it->second;
    } // for
    map_layer_nodes[++idx_map_layer_nodes] = n2;
  } // for

  // Rebuild view
  view.ln = map_layer_nodes;
  view.nl.clear();
  rg::for_each(view.ln, LV( rg::for_each(_1.second, LV( view.nl[__1] = _1.first , 1, __ )) ));

  return view;
} // function: run }}}

} // namespace celaeno::graph::operations::balance::crossings
