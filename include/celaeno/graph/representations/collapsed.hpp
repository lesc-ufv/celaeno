// vim: set expandtab fdm=marker ts=2 sw=2 tw=80 et :
//
// @author      : Ruan E. Formigoni (ruanformigoni@gmail.com)
// @file        : unbalance
// @created     : Friday Oct 15, 2021 07:55:49 -03
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

#include <celaeno/aliases.hpp>
#include <celaeno/fun/fun.hpp>
#include <celaeno/err/err.hpp>
#include <celaeno/graph/graph.hpp>

namespace celaeno::graph::representations::collapsed
{

namespace ns_graph = celaeno::graph;
namespace ns_err = celaeno::err;

using namespace celaeno::fun::fn;

// Result is a vector of pairs
// // First element is the node
// // Second element is the column
struct Entry
{
  i64 node;
  i64 idx;
  operator std::string();
};
using Entries = std::vector<Entry>;

inline Entry::operator std::string()
{
  std::stringstream os;
  os << "{ node: " << node <<  ", idx: " << idx << " }";
  return os.str();
}

// if a node is to the left or to the right
enum class Orientation
{
  LEFT,
  RIGHT,
};

// namespace
// {
//
// double entry_mean(std::vector<Entries::iterator> const& entries)
// {
//   auto indices = fn(entries).as([&](auto&& e){ return e->idx; }).vec();
//   return std::reduce(indices.begin(), indices.end()) / static_cast<double>(entries.size());
// } // entry mean
//
// } // namespace

// collapse() {{{
template<typename V, Range R, typename F>
Entries& collapse(ns_graph::Ops const& ops
  , Entries& entries
  , V&& view
  , R&& layer
  , F&& f_orientation)
{
  ns_err::Logger logger;

  for (auto&& node : layer)
  {
    logger.info()("Node is: {}", node);
    for(auto entry : entries)
    {
      logger.info()("Entry: .node={}, .idx={}", entry.node, entry.idx);
    } // for

    // Get predecessors of node in the entries vector
    auto vec_it_preds = fn(ops.preds(node))
      .as([&](auto&& e){ return std::ranges::find_if(entries, [&](auto&& f){ return f.node == e; }); })
      .vec();

    // Remove predecessors not on layer-1
    std::erase_if(vec_it_preds, [&](auto&& e){ return view.nl.at(e->node) != view.nl.at(node)-1; });
    std::ranges::for_each(vec_it_preds, [&](auto&& e){ logger.info()("Pred: {}", e->operator std::string()); });

    // Has two predecessors
    if ( vec_it_preds.size() == 2 )
    {
      // Sort by index
      std::ranges::sort(vec_it_preds, {}, [&](auto&& e){ return e->idx; });

      // Get iterator to node with smaller index
      auto it_lesser = vec_it_preds.at(0);
      auto it_greater = vec_it_preds.at(1);

      // Check if the entry with a greater index is less than or equal new entry
      // If it is, starting from std::next(it_lesser) onwards, adjust indices to it_inserted+n
      if ( it_greater->idx <= it_lesser->idx+1 )
      {
        std::for_each(std::next(it_lesser), entries.end(),[idx=it_lesser->idx+1](auto&& e) mutable
        { 
          e.idx = ++idx;
        });
      } // if

      // Insert entry
      // This invalidates the vec_it_preds entries iterators
      auto it_inserted = entries.insert(it_greater, Entry{ .node = node, .idx = it_lesser->idx+1, });

      continue;
    } // if


    if ( vec_it_preds.size() == 1 )
    {
      auto it_parent = vec_it_preds.at(0);

      // Check if should be placed to the left or to the right of predecessor
      Orientation orientation = f_orientation(it_parent->node, node);

      // // Adjust indices
      // if ( orientation == Orientation::RIGHT
      //   and std::next(it_parent) != entries.end()
      //   and std::next(it_parent)->idx < it_parent->idx+1 )
      // {
      //   std::for_each(std::next(it_parent), entries.end(),[idx=it_parent->idx+1](auto&& e) mutable
      //   { 
      //     e.idx = ++idx;
      //   });
      // } // if
      //
      // if ( orientation == Orientation::LEFT
      //   and it_parent != entries.begin()
      //   and std::prev(it_parent)->idx > it_parent->idx-1 )
      // {
      //   std::for_each(it_parent, entries.end(),[idx=std::prev(it_parent)->idx+1](auto&& e) mutable
      //   { 
      //     e.idx = ++idx;
      //   });
      // } // if


      // Place it to the right, with idx == parent.idx + 1
      // Place it to the left, with idx == parent.idx - 1
      // Iterator to parent is invalidated
      auto it_inserted = ( orientation == Orientation::RIGHT )?
          entries.insert(std::next(it_parent), Entry{ .node = node, .idx = it_parent->idx+1, })
        : entries.insert(it_parent, Entry{ .node = node, .idx = it_parent->idx-1, });

      continue;
    } // else if

    throw std::runtime_error("Invalid parents size");
  } // for

  return entries;
} // collapse() }}}

// collapse() {{{
template<typename V, typename F>
decltype(auto) collapse(ns_graph::Ops const& ops, V& view_depth, F&& f_orientation)
{
  ns_err::Logger logger;

  Entries entries;

  // Get layer vector
  std::vector<i64> indices_layer = fn(view_depth.ln).key().sort().unique().vec();

  // Check if has at least two layers to collapse
  ns_err::err({ indices_layer.size() >= 2 })("Trying to collapse graph with less than 2 layers");
  
  // Initialize first layer with sequential indices
  entries = fn(view_depth.ln.at(indices_layer.at(0)))
    .zip_with_ints(i64{})
    .as([&](auto&& e){ return Entry{ .node = e.first, .idx = e.second, }; })
    .vec();

  // Drop first layer 
  indices_layer.erase(indices_layer.begin());

  // Start collapsing from the second layer onwards
  for( auto idx_layer : indices_layer  )
  {

    entries = collapse(ops, entries, view_depth, view_depth.ln.at(idx_layer), f_orientation);
  } // for

  // Create map from entries
  return fn(entries)
    .as([&](auto&& entry){ return std::make_pair(entry.node, entry.idx); })
    .map();
} // collapse() }}}

} // namespace 
