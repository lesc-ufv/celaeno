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

#include <celaeno/graph/search/bfs.hpp>
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

// Aliases
template<SignedIntegral T>
using Cross = std::map<T,T>;
using Position = i64;
using Positions = std::vector<Position>;
using Node = i64;
using Nodes = std::vector<Node>;

}

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

// fn: insert_before_parent {{{
template<typename Node>
void insert_before_parent(Node u
  , Node p
  , Range auto&& layer
  , Range auto& positions_collapsed
  , Range auto& nodes_collapsed)
{
  // Get x-coordinate position of p
  auto position_p = positions_collapsed.at(std::distance(nodes_collapsed.begin(), std::ranges::find(nodes_collapsed, p)));

  // Find an index in positions_collapsed that equals p
  auto index_insert = std::distance(positions_collapsed.begin(),
      std::find_if(positions_collapsed.begin()
    , positions_collapsed.end()
    , fun::unary::equal(position_p)));

  // Skip past the previous (by layer ordering) nodes in the same layer
  // E.g. In layer {w,x,y,u,z}, {w,x,y} should be skipped for u to be after y
  while(std::ranges::find_first_of(
      std::ranges::subrange(nodes_collapsed.begin()+index_insert, nodes_collapsed.end())
    , std::ranges::subrange(layer.begin(), std::find(layer.begin(), layer.end(), u)))
    != std::ranges::end(nodes_collapsed))
  {
    ++index_insert;
  } // while

  nodes_collapsed.insert(nodes_collapsed.begin()+index_insert, u);
  positions_collapsed.insert(positions_collapsed.begin()+index_insert, position_p-1);
} // }}}

// fn: insert_after_parent {{{
void insert_after_parent(auto&& u
  , auto&& p
  , Range auto& layer
  , Range auto& positions_collapsed
  , Range auto& nodes_collapsed)
{
  std::ranges::reverse(nodes_collapsed);
  std::ranges::reverse(positions_collapsed);

  // Retrieve position of p in positions_collapsed
  auto position_p = positions_collapsed.at(std::distance(nodes_collapsed.begin(), std::ranges::find(nodes_collapsed, p)));

  // Find the index in which to insert u
  auto index_insert = std::distance(positions_collapsed.begin(),
      std::find_if(positions_collapsed.begin()
        , positions_collapsed.end()
        , fun::unary::equal(position_p)
      )
  );

  // Skip past the previous (by layer ordering) nodes in the same layer
  // E.g. In layer {w,x,y,u,z}, {w,x,y} should be skipped for u to be after y
  std::ranges::reverse(layer);
  while(std::ranges::find_first_of(
      std::ranges::subrange(nodes_collapsed.begin()+index_insert, nodes_collapsed.end())
    , std::ranges::subrange(layer.begin(), std::find(layer.begin(), layer.end(), u)))
    != std::ranges::end(nodes_collapsed))
  {
    ++index_insert;
  } // while
  std::ranges::reverse(layer);

  nodes_collapsed.insert(nodes_collapsed.begin()+index_insert, u);
  positions_collapsed.insert(positions_collapsed.begin()+index_insert, position_p+1);

  std::ranges::reverse(positions_collapsed);
  std::ranges::reverse(nodes_collapsed);
} // }}}

// fn: abs_diff_mean_ceil {{{
double abs_diff_mean_ceil(double a, double b)
{
  return std::ceil(
    fp::abs_diff(
        static_cast<double>(a)
      , static_cast<double>(b)
    ) / 2.0);
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
  if ( is_left )
  {
    for(auto it=std::ranges::begin(zipped_position_node); it != std::next(it_not_sequential); ++it)
    {
      std::get<0>(*it)[0] -= amount;
    } // for
    return true;
  } // if
  else
  {
    for(auto it=it_not_sequential; it != std::ranges::end(zipped_position_node); ++it)
    {
      std::get<0>(*it)[1] += amount;
    } // for
    return true;
  } // else if

  return false;
} // }}}

// fn: restructure_indices {{{
void restructure_indices(Ops const& ops
  , Range auto&& layers
  , Range auto&& nodes_collapsed
  , Range auto&& positions_collapsed)
{
  for (auto&& layer : layers)
  {
    for(auto window : layer | std::views::slide(2))
    {
      auto u = *window.begin();
      auto v = *std::next(window.begin());
      auto uv_pred = fn(ops.preds(u)).in(ops.preds(v)).vec();
      // Check if uv have a common parent
      if ( uv_pred.size() > 0 )
      {
        // Parent of both u and v
        auto p = uv_pred.at(0);
        // Find index of u, which is the node to the left of parent
        auto index_u = std::distance(nodes_collapsed.begin()
          , std::find(nodes_collapsed.begin(), nodes_collapsed.end(), u)
        );
        auto index_v = std::distance(nodes_collapsed.begin()
          , std::find(nodes_collapsed.begin(), nodes_collapsed.end(), v)
        );
        // Decrease by 1 all positions previous to u (inclusive)
        std::for_each(positions_collapsed.begin()
          , positions_collapsed.begin()+index_u
          , [](auto& e){ e -= 1; }
        );
        // Adjust index of parent
        auto index_p = std::distance(nodes_collapsed.begin()
          , std::find(nodes_collapsed.begin(), nodes_collapsed.end(), p)
        );
        positions_collapsed.at(index_p) = (positions_collapsed.at(index_u) + positions_collapsed.at(index_v)) / 2;
      } // if
      fmt::println("{} : {}", u, v);
    } // for
  } // for
} // fn: restructure_indices }}}

// fn: predecessor_single {{{
void predecessor_single(Node u
  , auto&& ops
  , Map auto&& map_node_layer
  , Range auto&& nodes_collapsed
  , Range auto&& positions_collapsed
  , Range auto&& layer)
{
  auto preds = ops.preds(u);

  Node p = preds.at(0);

  // Get successor of p, v, that is not u
  //  u v
  //  | |
  //   p
  Node v = fn(ops.succs(p))
    .dif(std::vector<Node>{u})
    .test([&](auto&& e){ return e.size() == 1; }, "Size of successors of p is not 1")
    .vec()
    .at(0);

  // If the order is pv, put u before p, i.e.: upv
  bool is_left = true;
  if (*std::ranges::find_first_of(nodes_collapsed, Nodes{p,v}) == p)
  {
    insert_before_parent(u, p, layer, positions_collapsed, nodes_collapsed);

    auto distance_u = std::distance(nodes_collapsed.begin(), std::find(nodes_collapsed.begin(), nodes_collapsed.end(), u));
    auto position_u = positions_collapsed.at(distance_u);
    auto distance_p = std::distance(nodes_collapsed.begin(), std::find(nodes_collapsed.begin(), nodes_collapsed.end(), p));
    auto position_p = positions_collapsed.at(distance_p);

    if ( position_u > position_p )
    {
      i64 offset = 1;
      if ( position_p != position_u ) { offset = abs_diff_mean_ceil(position_p, position_u); } // if
      std::for_each(positions_collapsed.begin()
        , positions_collapsed.begin() + distance_u + 1
        , [&](auto& e){ e -= offset; });
    } // if

    if ( auto it = std::find(nodes_collapsed.begin(), nodes_collapsed.end(), v); it != nodes_collapsed.end() )
    {
      auto distance_v = std::distance(nodes_collapsed.begin(), it);
      auto position_v = positions_collapsed.at(distance_v);

      if ( position_v < position_p )
      {
        i64 offset = 1;
        if ( position_p != position_v ) { offset = abs_diff_mean_ceil(position_p, position_v); } // if
        std::for_each(positions_collapsed.begin() + distance_v
          , positions_collapsed.end()
          , [&](auto& e){ e += offset; });
      } // if

    } // if

  } // if
  // Else if the order is vp, put u after p, i.e.: vpu
  else
  {
    is_left = false;
    insert_after_parent(u, p, layer, positions_collapsed, nodes_collapsed);

    auto distance_u = std::distance(nodes_collapsed.begin(), std::find(nodes_collapsed.begin(), nodes_collapsed.end(), u));
    auto position_u = positions_collapsed.at(distance_u);
    auto distance_p = std::distance(nodes_collapsed.begin(), std::find(nodes_collapsed.begin(), nodes_collapsed.end(), p));
    auto position_p = positions_collapsed.at(distance_p);

    if ( auto it = std::find(nodes_collapsed.begin(), nodes_collapsed.end(), v); it != nodes_collapsed.end() )
    {
      auto distance_v = std::distance(nodes_collapsed.begin(), it);
      auto position_v = positions_collapsed.at(distance_v);

      if ( position_v >= position_p )
      {
        i64 offset = 1;
        if ( position_p != position_v ) { offset = abs_diff_mean_ceil(position_p, position_v); } // if
        std::for_each(positions_collapsed.begin()
          , positions_collapsed.begin() + distance_v + 1
          , [&](auto& e){ e -= offset; });
      } // if
    } // if

    if ( position_u <= position_p )
    {
      i64 offset = 1;
      if ( position_p != position_u ) { offset = abs_diff_mean_ceil(position_p, position_u); } // if
      std::for_each(positions_collapsed.begin() + distance_u
        , positions_collapsed.end()
        , [&](auto& e){ e += offset; });
    } // if


  } // else

  while(keep_sequential(is_left, map_node_layer, positions_collapsed, nodes_collapsed)) {};
} // }}}

// fn: predecessor_immediate_single {{{
void predecessor_immediate_single(Node u
  , auto&& ops
  , Map auto&& map_node_layer
  , Range auto&& preds_in_layer
  , Range auto&& nodes_collapsed
  , Range auto&& positions_collapsed
  , Range auto&& layer)
{
  // Current possibilities
  //         u           u
  //        | |         | |
  //       |  |  or    |  |
  //       p  |        w  |
  //          w           p

  // {p,w}
  auto preds = ops.preds(u);

  // {p}
  Node p = preds_in_layer.at(0);

  // {p,w} - {p} == {w}
  Node w = fn(preds).dif(preds_in_layer).vec().at(0);

  // If the order is pw, put u after p, i.e.: puw
  bool is_left = true;
  if (*std::ranges::find_first_of(nodes_collapsed, Nodes{p,w}) == p)
  {
    is_left = false;
    insert_after_parent(u, p, layer, positions_collapsed, nodes_collapsed);
    // If index of w is equal or greater than u, decrease all nodes from w
    // backwards
    auto distance_w = std::distance(nodes_collapsed.begin(), std::find(nodes_collapsed.begin(), nodes_collapsed.end(), w));
    auto position_w = positions_collapsed.at(distance_w);
    auto distance_u = std::distance(nodes_collapsed.begin(), std::find(nodes_collapsed.begin(), nodes_collapsed.end(), u));
    auto position_u = positions_collapsed.at(distance_u);
    auto distance_p = std::distance(nodes_collapsed.begin(), std::find(nodes_collapsed.begin(), nodes_collapsed.end(), p));
    auto position_p = positions_collapsed.at(distance_p);
    if ( position_p > position_u )
    {
      std::for_each(positions_collapsed.begin()
        , positions_collapsed.begin() + distance_p + 1
        , [](auto& e){ e -= 1; });
    } // if
    // If index of p is equal or less than u, increase all nodes from p
    // backwards
    if ( position_w < position_u )
    {
      std::for_each(positions_collapsed.begin() + distance_w
        , positions_collapsed.end()
        , [](auto& e){ e += 1; });
    } // if
  } // if
  // Else if the order is wp, put u before p, i.e.: wup
  else
  {
    insert_before_parent(u, p, layer, positions_collapsed, nodes_collapsed);
    // If index of w is equal or greater than u, decrease all nodes from w
    // backwards
    auto distance_w = std::distance(nodes_collapsed.begin(), std::find(nodes_collapsed.begin(), nodes_collapsed.end(), w));
    auto position_w = positions_collapsed.at(distance_w);
    auto distance_u = std::distance(nodes_collapsed.begin(), std::find(nodes_collapsed.begin(), nodes_collapsed.end(), u));
    auto position_u = positions_collapsed.at(distance_u);
    auto distance_p = std::distance(nodes_collapsed.begin(), std::find(nodes_collapsed.begin(), nodes_collapsed.end(), p));
    auto position_p = positions_collapsed.at(distance_p);
    if ( position_w > position_u )
    {
      std::for_each(positions_collapsed.begin()
        , positions_collapsed.begin() + distance_w + 1
        , [](auto& e){ e -= 1; });
    } // if
    // If index of p is equal or less than u, increase all nodes from p
    // backwards
    if ( position_p < position_u )
    {
      std::for_each(positions_collapsed.begin() + distance_p
        , positions_collapsed.end()
        , [](auto& e){ e += 1; });
    } // if
  } // else

  while(keep_sequential(is_left, map_node_layer, positions_collapsed, nodes_collapsed)) {};
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
  bool is_left = true;
  auto position_p1 = positions_collapsed.at(distance_p1);
  auto position_p2 = positions_collapsed.at(distance_p2);
  if ( std::round(k) != k
    and map_node_layer.at(p1) == map_node_layer.at(p2)
    and std::abs(position_p1 - position_p2) < 2)
  {
    auto dist_fst_half = std::distance(positions_collapsed.begin(), positions_collapsed.begin() + std::max(distance_p1, distance_p2) + 1);
    auto dist_snd_half = std::distance(positions_collapsed.begin() + std::max(distance_p1, distance_p2), positions_collapsed.end());

    if ( dist_snd_half < dist_fst_half  )
    {
      is_left = false;
      i64 offset = (position_p1 == position_p2)? 2 : 1;
      std::for_each(positions_collapsed.begin() + std::max(distance_p1, distance_p2)
        , positions_collapsed.end()
        , [&](auto& e){ e += offset; });
    } // if
    else
    {
      i64 offset = (position_p1 == position_p2)? 2 : 1;
      std::for_each(positions_collapsed.begin()
        , positions_collapsed.begin() + std::max(distance_p1, distance_p2)
        , [&](auto& e){ e -= offset; });
    } // else
    // k should now be an integer
    k = f_calculate_k();
    assert(std::round(k) == k);
  } // if

  // 3.3 Put k in position l of positions_collapsed
  positions_collapsed.insert(positions_collapsed.begin()+index_insert, std::floor(k));

  // 3.4 Put u in position l of nodes_collapsed
  nodes_collapsed.insert(nodes_collapsed.begin()+index_insert, u);

  distance_p1 = std::distance(nodes_collapsed.begin(), std::find(nodes_collapsed.begin(), nodes_collapsed.end(), p1));
  position_p1 = positions_collapsed.at(distance_p1);
  distance_p2 = std::distance(nodes_collapsed.begin(), std::find(nodes_collapsed.begin(), nodes_collapsed.end(), p2));
  position_p2 = positions_collapsed.at(distance_p2);
  auto distance_u = std::distance(nodes_collapsed.begin(), std::find(nodes_collapsed.begin(), nodes_collapsed.end(), u));
  auto position_u = positions_collapsed.at(distance_u);
  if ( position_p1 > position_u )
  {
    i64 offset = 1;
    if ( position_p1 != position_u ) { offset = abs_diff_mean_ceil(position_p1, position_u); } // if
    std::for_each(positions_collapsed.begin()
      , positions_collapsed.begin() + distance_p1 + 1
      , [&](auto& e){ e -= offset; });
  } // if
  // If index of p is equal or less than u, increase all nodes from p
  // backwards
  if ( position_p2 < position_u )
  {
    i64 offset = 1;
    if ( position_p2 != position_u ) { offset = abs_diff_mean_ceil(position_p2, position_u); } // if
    std::for_each(positions_collapsed.begin() + distance_p2
      , positions_collapsed.end()
      , [&](auto& e){ e += offset; });
  } // if

  while(keep_sequential(is_left, map_node_layer, positions_collapsed, nodes_collapsed)) {};
} // }}}

// view_collapse {{{
template<typename V1, typename V2>
decltype(auto) view_collapse(Ops const& ops
  , V1&& map_layer_nodes
  , V2&& map_node_layer)
// Requires ordered map
{
  using Key = typename std::remove_cvref_t<typename std::remove_cvref_t<V1>::key_type>;

  log::err( { ! map_layer_nodes.empty() } )("Empty view");

  auto f_is_leaf = [&](Node node)
  {
    return ops.succs(node).size() == 0 or ops.preds(node).size() == 0;
  };

  // Fetch lowest layer id
  Key id_lowest_layer = fn(map_layer_nodes).key().min();

  // Assume that the first layer only contains inputs
  auto f_is_input = LR(ops.preds(_1).empty());
  log::err( { fn(map_layer_nodes.at(id_lowest_layer)).all(LR(f_is_input(_1))) } )
    ("First layer must only contain inputs");

  // Initialize collapse nodes and positions with first layer
  Nodes nodes_collapsed = map_layer_nodes.at(id_lowest_layer);
  Positions positions_collapsed = fp::numbers(i64{}, static_cast<i64>(nodes_collapsed.size()));

  auto f_print_node_positions = [&]
  {
    for (auto&& zipped : std::views::zip(nodes_collapsed, positions_collapsed))
    {
      std::cout << "[" << std::get<0>(zipped) << "," << std::get<1>(zipped) << "]\n";
    } // for
  };
  f_print_node_positions();

  for (auto it_entry{std::next(map_layer_nodes.begin())}; it_entry != map_layer_nodes.end(); ++it_entry)
  {
    auto layer = it_entry->second;

    for (auto&& u : layer)
    {
      // Skip inputs / outputs
      if ( f_is_leaf(u) )
      {
        continue;
      } // if

      // restructure_indices(std::ranges::subrange(map_layer_nodes.begin(), it_entry) | std::views::values);

      // 1. Check for predecessors of u in nodes collapsed
      auto preds = ops.preds(u);
      auto preds_in_layer = fn(preds).in(std::prev(it_entry)->second).vec();

      log::err({preds.size() != 0})("Empty preds for {}"_fmt(u));

      // 2. If there is only 1 predecessor p
      if ( preds.size() == 1 )
      {
        predecessor_single(u, ops, map_node_layer, nodes_collapsed, positions_collapsed, layer);
        continue;
      } // if

      // if (preds_in_layer.size() == 1 and ops.succs(preds_in_layer.at(0)).size() > 1)
      if (preds_in_layer.size() == 1)
      {
        predecessor_immediate_single(u, ops, map_node_layer, preds_in_layer, nodes_collapsed, positions_collapsed, layer);
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
std::map<T,Cross<T>> run(i64 idx_l2, R&& l1, R&& l2, Ops const& ops)
{
  [[maybe_unused]] ns_log::Timer timer("celaeno::graph::operations::balance::crossings");

  // Return cross id → Crossed nodes [src,dest]
  std::map<i64, Cross<i64>> out;

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

  return out;

} // function: run }}}

// fn: run {{{
template<SignedIntegral T = i64, typename V>
decltype(auto) run(Ops const& ops, V& view)
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
    auto map_layer_crossings = create_crossing_layers(ops, n2, result);
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
