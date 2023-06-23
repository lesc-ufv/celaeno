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
#include <celaeno/fun/fun.hpp>
#include <celaeno/err/err.hpp>

#include <celaeno/graph/search/bfs.hpp>

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
namespace ns_search = celaeno::graph::search;
// }}}

// Aliases {{{
template<SignedIntegral T>
using Cross = std::map<T,T>;
// }}}

// fn: run {{{
template<SignedIntegral T = i64, Range R>
std::map<T,Cross<T>> run(R&& l1, R&& l2, Ops const& ops)
{

#if ! defined(NDEBUG) && defined(DEBUG_SHOW_ALG)
  spdlog::set_level(spdlog::level::debug);
  spdlog::debug("Algorithm: celaeno::graph::operations::balance::crossings");
#endif
  // Return cross id → Crossed nodes [src,dest]
  std::map<i64, Cross<i64>> out;

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

        // Insert element
        auto it_ins{acc.insert(it_search,id_child)};

        // Get position of insertion
        i64 i_position {std::distance(acc.begin(),it_ins)};

        // Insert parent
        parents.insert(parents.begin()+i_position, parent);

        // Check crossing edges in acc
        for (auto it3{std::prev(acc.end())}; it3 != it_ins; --it3)
        {
          // Create crossing map
          Cross<T> cross;

          // Create dummy
          --idx;

          // Save new crossing
          set_id_crossings.insert(idx);

          // Get child & parent of intersection
          i64 n_id_child{*it3};
          i64 n_child{m_id_node.at(n_id_child)};
          i64 n_parent{parents.at(std::distance(acc.begin(),it3))};

          // Link parent with dummy
          ops.link(n_parent,idx);
          // Link dummy with child
          ops.link(idx,n_child);
          // Unlink parent & child
          ops.unlink(n_parent,n_child);

          err::err({! f_is_crossing(n_child)})
            ("Set id crossings must not contain n_child");

          // Update children in result
          if( f_is_crossing(n_parent) )
          {
            if( f_is_crossing(n_parent) )
            {
              for (auto& [u,v] : out[n_parent])
              {
                if( v == n_child ) { v = idx; } // if
              } // for
            } // if
          } // if

          // Save in crossing map
          cross[n_parent] = n_child;

          // Update parent of child
          parents.at(std::distance(acc.begin(),it3)) = idx;

          // Link parent with dummy
          ops.link(parent,idx);
          // Link dummy with child
          ops.link(idx,child);
          // Unlink parent & child
          ops.unlink(parent,child);
          // Save in crossing map

          err::err({! f_is_crossing(n_child)})
            ("Set id crossings must not contain n_child");

          // Update children in result
          if( f_is_crossing(parent) )
          {
            for (auto& [u,v] : out[parent])
            {
              if( v == child ) { v = idx; } // if
            } // for
          } // if

          // Save in crossing map
          cross[parent] = child;

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
std::map<i64,Cross<i64>> run(Ops const& ops, V& view_proximity)
{
  // Crossings
  std::map<i64, Cross<i64>> out;

  // Create a proximity view
  auto const& [ln,nl] = view_proximity;

  // Create overlapping layer indices
  auto [it_min, it_max] {std::ranges::minmax_element(ln, {}, [](auto _1){ return _1.first; })};
  auto layers{fp::overlapping_pairs(fp::numbers(it_min->first,it_max->first))};

  for (auto const& [i,j] : layers)
  {
    auto const& n1{ln.at(i)};
    auto const& n2{ln.at(j)};

    auto result{run(n1,n2,ops)};

    for (auto const& [k,v] : result)
    {
      out.emplace(k,v);
    } // for
  } // for


  //
  // Passtrough balancing dummies
  //

  // Check if a node is a balancing dummy
  auto f_is_balancing_dummy =
  [&](T u)
  {
    return ops.preds(u).size() == 1 && ops.succs(u).size() == 1;
  };

  // Get nodes until they are not a balancing dummy
  auto f_get_not_balancing_dummy =
  [&]<typename F>(T u, F f)
  {
    while( f_is_balancing_dummy(u) )
    {
      u = f(u).at(0);
    }
    return u;
  };

  for (auto& [dummy_cross,m_cross] : out)
  {
    Cross<i64> m_new_cross;

    for (auto& [parent,child] : m_cross)
    {
      i64 new_parent{parent};
      i64 new_child{child};

      if( f_is_balancing_dummy(parent) )
      {
        // fmt::print("Parent {} is balancing dummy\n", parent);
        new_parent = f_get_not_balancing_dummy(parent,ops.preds);
        // fmt::print("new_parent: {}\n", new_parent);
      } // if

      if( f_is_balancing_dummy(child) )
      {
        // fmt::print("Child {} is balancing dummy", child);
        new_child = f_get_not_balancing_dummy(child,ops.succs);
        // fmt::print("new_child: {}\n", new_child);
      } // if

      m_new_cross[new_parent] = new_child;
    } // for

    m_cross = m_new_cross;
  } // for


  // for (auto e : out)
  // {
  //   fmt::print("Crossing: {}\n", e);
  // } // for

  return out;
} // function: run }}}

} // namespace celaeno::graph::operations::balance::crossings }}}
