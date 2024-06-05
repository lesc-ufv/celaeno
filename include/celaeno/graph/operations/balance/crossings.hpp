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
#include <celaeno/err/err.hpp>

#include <celaeno/graph/search/bfs.hpp>
#include <celaeno/graph/views/depth.hpp>
#include <celaeno/graph/operations/balance/paths.hpp>

// namespace celaeno::graph::operations::balance::crossings {{{
namespace celaeno::graph::operations::balance::crossings
{

// using namespace {{{
using namespace celaeno::aliases;
using namespace celaeno::concepts;
using namespace celaeno::fun::fn;
// }}}

// namespaces {{{
namespace rg = ranges;
namespace fp = fplus;
namespace fw = fplus::fwd;

namespace err = celaeno::err;
namespace fun = celaeno::fun;
namespace ns_search = celaeno::graph::search;
// }}}

// Aliases {{{
template<SignedIntegral T>
using Cross = std::map<T,T>;
// }}}

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
  , V1 map_layer_nodes
  , V2 const& map_node_layer)
// Requires ordered map
{
  using Key = typename std::remove_cvref_t<typename std::remove_cvref_t<V1>::key_type>;
  using Node = typename std::remove_cvref_t<typename std::remove_cvref_t<V1>::mapped_type>::value_type;
  using Position = i64;
  using Positions = std::vector<Position>;
  using Nodes = std::vector<Node>;
  using NodeSet = std::unordered_set<Node>;

  err::err( { ! map_layer_nodes.empty() } )("Empty view");

  // Fetch lowest layer id
  Key id_lowest_layer = fn(map_layer_nodes).key().min();

  // Assume that the first layer only contains inputs
  auto f_is_input = LR(ops.preds(_1).empty());
  err::err( { fn(map_layer_nodes.at(id_lowest_layer)).all(LR(f_is_input(_1))) } )
    ("First layer must only contain inputs");

  // Initialize collapse nodes and positions with first layer
  Nodes nodes_collapsed = map_layer_nodes.at(id_lowest_layer);
  Positions positions_collapsed = fp::numbers(i64{}, static_cast<i64>(nodes_collapsed.size()));

  // Erase first layer and start from second one
  map_layer_nodes.erase(id_lowest_layer);

  NodeSet nodes_visited;

  for (auto it_entry{map_layer_nodes.begin()}; it_entry != map_layer_nodes.end(); ++it_entry)
  {
    auto layer = it_entry->second;

    for (auto&& u : layer)
    {
      // 1. Check for predecessors of u in nodes collapsed
      auto preds = ops.preds(u);

      err::err({preds.size() != 0})("Empty preds for {}"_fmt(u));

      // 2. If there is only 1 predecessor p
      if ( preds.size() == 1 )
      {
        Node p = preds.at(0);

        // 2.1 Get successor of p, v, that is not u
        Nodes succs_p = fn(ops.succs(p)).dif(std::vector<Node>{u}).vec();
        err::err({succs_p.size() == 1})("Size of successors of p is not 1: {}"_fmt(succs_p));
        Node v = succs_p.at(0);

        // 2.2 If the order is pv, put u before p, i.e.: upv
        if (auto it_fst = std::ranges::find_first_of(nodes_collapsed, Nodes{p,v}); *it_fst == p )
        {
          auto distance = std::distance(nodes_collapsed.begin(), it_fst);
          auto position = positions_collapsed.at(distance);
          nodes_collapsed.insert(nodes_collapsed.begin()+distance, u);
          positions_collapsed.insert(positions_collapsed.begin()+distance, position-1);
        } // if
        // 2.3 Else if the order is vp, put u after p, i.e.: vpu
        else
        {
          auto distance = std::distance(nodes_collapsed.begin(), std::ranges::find(nodes_collapsed, p));
          auto position = positions_collapsed.at(distance);
          nodes_collapsed.insert(nodes_collapsed.begin()+distance, u);
          positions_collapsed.insert(positions_collapsed.begin()+distance, position-1);
        } // else
        // 2.6 Adjust positions to only contain an increasing sequence
        // 2.6.1 If the current position has an index less than the previous
        for(auto&& zipped_rngs : std::views::zip(std::views::slide(positions_collapsed, 2)
          , std::views::slide(nodes_collapsed, 2)))
        {
          auto rng_positions = std::get<0>(zipped_rngs);
          auto rng_nodes = std::get<1>(zipped_rngs);
          // It is ok to be the same as long as it is not between nodes on the
          // same layer
          if (rng_positions[1] < rng_positions[0]
          or (rng_positions[1] == rng_positions[0] and map_node_layer.at(rng_nodes[0]) == map_node_layer.at(rng_nodes[1])))
          {
            // The current position should have the previous index+1
            rng_positions[1] = rng_positions[0]+1;
          }
        } // for
        continue;
      } // if

      // 3. If there is 2 predecessors p1 and p2 in nodes_collapsed
      if ( preds.size() == 2 )
      {
        // 3.1 The position of u is k=(p1+p2)/2
        Node p1 = preds.at(0);
        Node p2 = preds.at(1);
        auto position_p1 = std::distance(nodes_collapsed.begin(), std::ranges::find(nodes_collapsed, p1));
        auto position_p2 = std::distance(nodes_collapsed.begin(), std::ranges::find(nodes_collapsed, p2));
        auto k = ( positions_collapsed.at(position_p1) + positions_collapsed.at(position_p2) ) / 2;
        
        // 3.2 Find the position l that is greater or equal to k in positions_collapsed
        auto it = std::ranges::find_if(positions_collapsed, fun::unary::greater_equal(k));
        err::err({ it != positions_collapsed.end() })("Failed to find greate_equal position");
        auto position_l = std::distance(positions_collapsed.begin(), it);

        // 3.3 Put k in position l of positions_collapsed
        positions_collapsed.insert(positions_collapsed.begin()+position_l, k);

        // 3.4 Put u in position l of nodes_collapsed
        nodes_collapsed.insert(nodes_collapsed.begin()+position_l, u);
        continue;
      } // if

      err::err()("Invalid preds size of {}"_fmt(preds.size()));
    } // for
  } // for

  return std::make_pair(nodes_collapsed, positions_collapsed);
} // function: view_collapse }}}

// fn: run {{{
template<SignedIntegral T = i64, Range R>
std::map<T,Cross<T>> run(i64 idx_l2, R&& l1, R&& l2, Ops const& ops)
{

#if ! defined(NDEBUG) && defined(DEBUG_SHOW_ALG)
  spdlog::set_level(spdlog::level::debug);
  spdlog::debug("Algorithm: celaeno::graph::operations::balance::crossings");
#endif
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
  err::err({!succs.empty()})("Empty successors map!");

  // First vector accumulates all values
  auto& acc{*succs.begin()};

  // Save parents of nodes in acc
  std::vector<i64> parents;

  // Parents of l2 nodes
  std::map<i64,i64> m_id_parent;

  // Initialize parents of acc nodes
  for( auto e : acc )
  {
    m_id_parent[e] = l1.at(0);
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

          // Save new crossing
          set_id_crossings.insert(idx);

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

          err::err({! f_is_crossing(n_child)})
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

          err::err({! f_is_crossing(n_child)})
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

} // namespace celaeno::graph::operations::balance::crossings }}}
