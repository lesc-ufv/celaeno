// vim: set expandtab fdm=marker ts=2 sw=2 tw=80 et :
//
// @author      : Ruan E. Formigoni (ruanformigoni@gmail.com)
// @file        : proximity
// @created     : sexta jul 31, 2020 02:23:41 -03
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

#include <ranges>
#include <utility>

#include <range/v3/all.hpp>
#ifndef NDEBUG
  #include <spdlog/spdlog.h>
#endif

#include <celaeno/concepts.hpp>
#include <celaeno/graph/search/bfs.hpp>
#include <celaeno/fun/fun.hpp>

// namespace celaeno::graph::views::proximity {{{

namespace celaeno::graph::views::proximity
{

// Namespaces {{{
namespace rg = ranges;
namespace fun = celaeno::fun;
namespace ns_search = celaeno::graph::search;
// }}}

// Using namespaces {{{
using namespace celaeno::fun::fn;
using namespace celaeno::concepts;
using namespace celaeno::aliases;
// }}}

// Aliases {{{
using Layer = i64;
template<typename N> using LayerNodes = std::map<Layer,std::vector<N>>;
template<typename N> using NodeLayer = std::map<N,Layer>;
// }}}

// struct: Result {{{
template<typename N>
struct Result
{
  LayerNodes<N> ln;
  NodeLayer<N> nl;
  Result(LayerNodes<N>& _ln, NodeLayer<N>& _nl)
    : ln(std::move(_ln))
    , nl(std::move(_nl))
  {}
}; // struct }}}

// fn: run {{{
template<SignedIntegral T, typename P, typename S>
Result<T> run([[maybe_unused]] T root, P&& f_pred, S&& f_succ)
  requires CallableWith<P,i64>
  && CallableWith<S,i64>
{

#if ! defined(NDEBUG) && defined(DEBUG_SHOW_ALG)
  spdlog::set_level(spdlog::level::debug);
  spdlog::debug("Algorithm: celaeno::graph::views::proximity");
#endif

  // Create a depth-view
  NodeLayer<T> m_node_layer;

  struct Entry
  {
    T u; // Current node
    bool is_input; // If is input or output
  };

  // Visited nodes
  std::set<T> set_visited;

  // Nodes to visit
  std::queue<Entry> queue_visit;

  // Search from io to the rest of the graph
  auto f_search_by_root = [&](Entry entry)
  {
    i64 idx_current_layer{m_node_layer.at(entry.u)};

    // Mark as visited
    set_visited.insert(entry.u);

    // Set u as initial node
    std::vector<T> vec_node_curr{entry.u};

    while( ! vec_node_curr.empty() )
    {
      // Update m_node_layer
      rg::for_each(vec_node_curr, [&](auto&& _1)
      {
        // Prefer lowest layer (closer to inputs)
        if ( ! m_node_layer.contains(_1) )
        {
          m_node_layer[_1] = idx_current_layer;
        }
        else if ( entry.is_input && idx_current_layer > m_node_layer.at(_1) )
        {
          m_node_layer[_1] = idx_current_layer;
        }
        else if ( ! entry.is_input && idx_current_layer < m_node_layer.at(_1) )
        {
          m_node_layer[_1] = idx_current_layer;
        }
      });

      // If has input or output, and is not in visited set, push into queue
      auto f_is_input = [&](T _1){ return f_pred(_1).size() == 0; };
      auto f_is_output = [&](T _1){ return f_succ(_1).size() == 0; };
      rg::for_each(vec_node_curr, [&](auto&& _1)
      {
        if ( ! set_visited.contains(_1) )
        {
          if ( f_is_input(_1) )
          {
            queue_visit.push({_1, true});
          } // if
          else if ( f_is_output(_1) )
          {
            queue_visit.push({_1, false});
          } // else if
        }
      });

      // Update layer counter
      if ( entry.is_input ) { idx_current_layer++; } else { idx_current_layer--; }

      // Transform into next layer
      auto f_next = (entry.is_input)? f_succ : f_pred;
      vec_node_curr = fn(vec_node_curr)
        .as([&](auto&& _1){ return f_next(_1); })
        .squash()
        .sort()
        .unique()
        .vec();
    }
  };

  // Get an input and push into the queue, assume the initial layer to be 0
  ns_search::bfs::run(0
    , f_pred
    , f_succ
    , [&](auto&& _1){ return (f_pred(_1).size() == 0)? (queue_visit.push({_1, true}), true) : false;  });

  // Start with the assumption that initial input is at level 0
  m_node_layer[queue_visit.front().u] = 0;

  while ( ! queue_visit.empty() )
  {
    // Skip visited
    if ( set_visited.contains(queue_visit.front().u) ) { queue_visit.pop(); continue; }
    // Use node as search tree root
    f_search_by_root(queue_visit.front());
    // Go to next
    queue_visit.pop();
  } // while


  LayerNodes<T> m_layer_nodes;
  fn(m_node_layer).ply([&](auto&& e)
  {
    if ( ! m_layer_nodes.contains(e.second) )
    {
      m_layer_nodes.emplace(e.second, std::vector<T>{e.first});
    } // if
    else
    {
      m_layer_nodes.at(e.second).push_back(e.first);
    } // else
  });

  return Result<T>{ m_layer_nodes, m_node_layer };

} // function: run }}}

// fn: run {{{
template<SignedIntegral T>
void run(T root, Ops ops)
{
  run(root, ops.preds, ops.succs);
}
// }}}

} // namespace celaeno::graph::views::proximity }}}
