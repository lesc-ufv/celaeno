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

// view_collapse {{{
template<typename V1, typename V2>
decltype(auto) view_collapse(Ops const& ops
  , V1&& map_layer_nodes
  , V2&& map_node_layer)
// Requires ordered map
{
  using Key = typename std::remove_cvref_t<typename std::remove_cvref_t<V1>::key_type>;
  using Node = typename std::remove_cvref_t<typename std::remove_cvref_t<V1>::mapped_type>::value_type;
  using Position = i64;
  using Positions = std::vector<Position>;
  using Nodes = std::vector<Node>;
  using NodeSet = std::unordered_set<Node>;

  log::err( { ! map_layer_nodes.empty() } )("Empty view");

  // Fetch lowest layer id
  Key id_lowest_layer = fn(map_layer_nodes).key().min();

  // Assume that the first layer only contains inputs
  auto f_is_input = LR(ops.preds(_1).empty());
  log::err( { fn(map_layer_nodes.at(id_lowest_layer)).all(LR(f_is_input(_1))) } )
    ("First layer must only contain inputs");

  // Initialize collapse nodes and positions with first layer
  Nodes nodes_collapsed = map_layer_nodes.at(id_lowest_layer);
  Positions positions_collapsed = fp::numbers(i64{}, static_cast<i64>(nodes_collapsed.size()));

  // auto f_print_node_positions = [&]
  // {
  //   for (auto&& zipped : std::views::zip(nodes_collapsed, positions_collapsed))
  //   {
  //     std::cout << "[" << std::get<0>(zipped) << "," << std::get<1>(zipped) << "]\n";
  //   } // for
  // };
  // f_print_node_positions();

  for (auto it_entry{std::next(map_layer_nodes.begin())}; it_entry != map_layer_nodes.end(); ++it_entry)
  {
    auto layer = it_entry->second;

    for (auto&& u : layer)
    {
      // 1. Check for predecessors of u in nodes collapsed
      auto preds = ops.preds(u);
      auto preds_in_layer = fn(preds).in(std::prev(it_entry)->second).vec();

      log::err({preds.size() != 0})("Empty preds for {}"_fmt(u));

      // 2. If there is only 1 predecessor p
      if ( preds.size() == 1
      or (preds_in_layer.size() == 1 and ops.succs(preds_in_layer.at(0)).size() > 1))
      {
        Node p = (preds_in_layer.size() == 1)? preds_in_layer.at(0) : preds.at(0);

        // 2.1 Get successor of p, v, that is not u
        Nodes succs_p = fn(ops.succs(p)).dif(std::vector<Node>{u}).vec();
        log::err({succs_p.size() == 1})("Size of successors of p is not 1: {}"_fmt(succs_p));
        Node v = succs_p.at(0);

        enum class Direction
        {
          LEFT,
          RIGHT,
        };

        std::optional<Direction> opt_direction = std::nullopt;
        if ( preds.size() == 2 and preds_in_layer.size() == 1 )
        {
          // If first pred found in nodes_collapsed is parent, should position
          // to the right, else if is predecessor w should position to the left
          //          u
          //        |  | -> Put u to the right of p if is first match
          //        p  |
          //           w
          //          u
          //        |  | -> Put u to the left of p if w is first match
          //        |  p
          //        w
          if ( *std::ranges::find_first_of(nodes_collapsed, preds) == p )
          {
            opt_direction = Direction::RIGHT;
          } // if
          else
          {
            opt_direction = Direction::LEFT;
          } // else
        } // if

        // 2.2 If the order is pv, put u before p, i.e.: upv
        if (auto it_fst = std::ranges::find_first_of(nodes_collapsed, Nodes{p,v});
           (not opt_direction and *it_fst == p)
        or (opt_direction and *opt_direction == Direction::LEFT))
        {
          opt_direction = Direction::LEFT;
          auto distance_p = std::distance(nodes_collapsed.begin(), std::ranges::find(nodes_collapsed, p));
          auto position_p = positions_collapsed.at(distance_p);
          // Find a position in positions_collapsed that is equal or greater than p+1
          auto it_positions_collapsed = std::find_if(positions_collapsed.begin()
            , positions_collapsed.end()
            , fun::unary::greater_equal(position_p));
          auto distance_insert = std::distance(positions_collapsed.begin(), it_positions_collapsed);

          // Skip past the previous (by layer ordering) nodes in the same layer
          // E.g. In layer {w,x,y,u,z}, {w,x,y} should be skipped for u to be after y
          while(std::ranges::find_first_of(
              std::ranges::subrange(nodes_collapsed.begin()+distance_insert, nodes_collapsed.end())
            , std::ranges::subrange(layer.begin(), std::find(layer.begin(), layer.end(), u)))
            != std::ranges::end(nodes_collapsed))
          {
            ++distance_insert;
          } // while

          // Put node in-between predecessors if there is two
          if ( preds.size() == 2 )
          {
            while(std::count_if(nodes_collapsed.begin()+distance_insert
              , nodes_collapsed.end()
              , [&](auto&& e){ return e == preds.at(0) or e == preds.at(1); })
              >= 2)
            {
              ++distance_insert;
            } // while
          } // if

          nodes_collapsed.insert(nodes_collapsed.begin()+distance_insert, u);
          positions_collapsed.insert(positions_collapsed.begin()+distance_insert, position_p-1);
        } // if
        // 2.3 Else if the order is vp, put u after p, i.e.: vpu
        else if ( (not opt_direction and *it_fst == v) or ( opt_direction and *opt_direction == Direction::RIGHT ))
        {
          opt_direction = Direction::RIGHT;
          // Find distance to p in nodes_collapsed
          auto distance_p = std::distance(nodes_collapsed.begin(), std::ranges::find(nodes_collapsed, p));
          // Retrieve position of p in positions_collapsed
          auto position_p = positions_collapsed.at(distance_p);
          // Find a position in positions_collapsed that is equal or greater than p+1
          auto it_positions_collapsed = std::find_if(positions_collapsed.rbegin()
            , positions_collapsed.rend()
            , fun::unary::less_equal(position_p));
          auto distance_positions_collapsed = std::distance(positions_collapsed.begin(), (it_positions_collapsed+1).base());
          nodes_collapsed.insert(nodes_collapsed.begin()+distance_positions_collapsed+1, u);
          positions_collapsed.insert(positions_collapsed.begin()+distance_positions_collapsed+1, position_p+1);
        } // else if
        else
        {
          log::err()("Could not bind parent nor child");
        } // else

        // 2.6 Adjust positions to only contain an increasing sequence
        auto zipped_position_node = std::views::zip(
            std::views::slide(positions_collapsed, 2)
          , std::views::slide(nodes_collapsed, 2)
        );

        // 2.6.1 Check if insertion caused the sequence to be non-increasing
        int64_t amount{};
        auto it_non_increasing = std::ranges::find_if(zipped_position_node
        , [&,i=-1](auto&& e) mutable
        {
          i++;
          auto rng_positions = std::get<0>(e);
          auto rng_nodes = std::get<1>(e);
          // It is ok to be the same as long as it is not between nodes on the
          // same layer
          if (rng_positions[1] < rng_positions[0])
          {
            // Define the adjustment amount as the neighboring difference
            amount = std::ceil(
              fp::abs_diff(
                  static_cast<double>(rng_positions[0])
                , static_cast<double>(rng_positions[1])
              ) / 2.0
            );
            // Check if there is a previous node on nodes_collapsed that is on the same layer
            auto reverse_nodes_collapsed = std::ranges::subrange(nodes_collapsed.begin(), nodes_collapsed.begin()+i+1)
              | std::views::reverse;
            if ( auto it_rev = std::ranges::find_if(reverse_nodes_collapsed
              , [&](auto&& f){ return map_node_layer.at(f) == map_node_layer.at(rng_nodes[1]); });
              it_rev != std::ranges::end(reverse_nodes_collapsed))
            {
              auto distance = std::distance(nodes_collapsed.begin(), std::ranges::find(nodes_collapsed, *it_rev));
              if ( positions_collapsed.at(distance) >= rng_positions[1] )
              {
                amount = std::ceil(
                  fp::abs_diff(
                      static_cast<double>(positions_collapsed.at(distance))
                    , static_cast<double>(rng_positions[1])
                  ) / 2.0
                ) + 1;
              } // if
            } // if
            return true;
          } // if
          else if (rng_positions[1] == rng_positions[0] and map_node_layer.at(rng_nodes[0]) == map_node_layer.at(rng_nodes[1]))
          {
            amount = 1;
            return true;
          } // if
          return false;
        });

        if ( it_non_increasing == std::ranges::end(zipped_position_node) )
        {
          continue;
        } // if

        // 2.6.2 Check if should increase the sequence forwards or backwards
        // Decrease backwards if configuration is to the left
        // Increase forwards if configuration is to the right
        assert(opt_direction);
        if ( *opt_direction == Direction::LEFT )
        {
          for(auto it=std::ranges::begin(zipped_position_node); it != std::next(it_non_increasing); ++it)
          {
            auto rng_positions = std::get<0>(*it);
            rng_positions[0] -= amount;
          } // for
        } // if
        else if ( *opt_direction == Direction::RIGHT )
        {
          for(auto it=it_non_increasing; it != std::ranges::end(zipped_position_node); ++it)
          {
            auto rng_positions = std::get<0>(*it);
            rng_positions[1] += amount;
          } // for
        } // else if

        continue;
      } // if

      // 3. If there is 2 predecessors p1 and p2 in nodes_collapsed
      if ( preds.size() == 2 )
      {
        // 3.1 The position of u is k=(p1+p2)/2
        Node p1 = preds.at(0);
        Node p2 = preds.at(1);
        auto distance_p1 = std::distance(nodes_collapsed.begin(), std::ranges::find(nodes_collapsed, p1));
        auto distance_p2 = std::distance(nodes_collapsed.begin(), std::ranges::find(nodes_collapsed, p2));
        auto position_p1 = positions_collapsed.at(distance_p1);
        auto position_p2 = positions_collapsed.at(distance_p2);
        double k = (static_cast<double>(position_p1) + position_p2) / 2.0;
        
        // 3.2 Find the position l that is greater or equal to k in positions_collapsed
        auto it = std::find_if(positions_collapsed.begin() + std::min(distance_p1, distance_p2)
          , positions_collapsed.end()
          , fun::unary::greater_equal(std::floor(k)));
        log::err({ it != positions_collapsed.end() })("Failed to find greate_equal position");
        auto position_l = std::distance(positions_collapsed.begin(), it) + 1;

        // 3.5 Increment following positions by 1 if abs_diff(distance_p2, distance_p2) < 2
        // This is to set node 'u' in-between p1 and p2
        //      u
        //     | |
        //    p1  p2
        if ( std::round(k) != k
          and (map_node_layer.at(p1) == map_node_layer.at(p2))
          and map_node_layer.at(u) == map_node_layer.at(p1)+1)
        {
          std::for_each(positions_collapsed.begin() + std::max(distance_p1, distance_p2)
            , positions_collapsed.end()
            , [](auto& e){ e += 1; });
          k = std::ceil(k);
        } // if

        // 3.3 Put k in position l of positions_collapsed
        positions_collapsed.insert(positions_collapsed.begin()+position_l, std::floor(k));

        // 3.4 Put u in position l of nodes_collapsed
        nodes_collapsed.insert(nodes_collapsed.begin()+position_l, u);

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
  for(int i=0; i < acc.size(); ++i)
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
