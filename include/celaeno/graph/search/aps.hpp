// vim: set expandtab fdm=marker ts=2 sw=2 tw=80 et :
//
// @author      : Ruan E. Formigoni (ruanformigoni@gmail.com)
// @file        : aps
// @created     : Friday Oct 01, 2021 20:52:54 -03
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

#include <optional>

#include <range/v3/all.hpp>
#include <fplus/fplus.hpp>

#include <celaeno/aliases.hpp>
#include <celaeno/concepts.hpp>
#include <celaeno/fun/fun.hpp>

#include <celaeno/graph/search/dfs.hpp>


// celaeno::graph::search::aps {{{
namespace celaeno::graph::search::aps
{

// Namespaces {{{
namespace fp = fplus;
namespace rg = ranges;

namespace ns_search = celaeno::graph::search;
// }}}

// Using namespaces {{{
using namespace celaeno::fun::fn;
using namespace celaeno::concepts;
using namespace celaeno::aliases;
// }}}

// Algorithm {{{
template<SignedIntegral T, typename P, typename S>
std::vector<T> run(T root, P&& pred, S&& succ)
requires CallableWith<P,i64> && CallableWith<S,i64>
{

#if ! defined(NDEBUG) && defined(DEBUG_SHOW_ALG)
  spdlog::set_level(spdlog::level::debug);
  spdlog::debug("Algorithm: celaeno::graph::search::aps");
#endif

  // Create adjacent helper
  auto f_nb = [&](auto&& u){ return fp::append(pred(u),succ(u)); };

  // Visited nodes
  std::set<T> visited;

  // Map node -> parent
  std::map<T,T> m_parent;

  // Get nodes in dfs order
  // Save parents of each node
  auto dfs{ns_search::dfs::run(root, pred, succ,
    [&,v=root,st=std::stack<T>()](T e) mutable
    {
      if( e == root )
      {
        st.push(e);
      } // if
      else
      {
        auto neighbors{fn(f_nb(e)).set()};

        while( ! neighbors.contains(v) )
        {
          v = st.top(); st.pop();
        } // while

        st.push(v);

        m_parent[e] = v;

      } // else

      st.push(e);

      return false;
    })
  };

  // Discovery times
  auto dts{fp::numbers({},static_cast<T>(dfs.size()))};

  // Map node -> discovery time
  auto m_ndt{fp::pairs_to_map<std::map<T,T>>(fp::zip(dfs,dts))};

  // Low-link values
  std::vector<T> low_link{dts};

  // Stack used to find parents of nodes in dfs order
  std::stack<T> st;

  // Push first elem to st
  st.push(root);

  // Updates the low_value of a node
  auto f_update_low_value =
  [
      &visited = std::as_const(visited)
    , &m_parent = std::as_const(m_parent)
    , &low_link = std::as_const(low_link)
    , &m_ndt = std::as_const(m_ndt)
    , &f_nb = std::as_const(f_nb)
  ]
  (T u) -> std::optional<T>
  {
    if( visited.contains(u) ){ return std::nullopt; }

    // Parent low_link value
    T dt_u{low_link.at(m_ndt.at(u))};

    // Fetch children
    auto children{f_nb(u)};

    // Exclude parent
    if( m_parent.contains(u) )
    {
      children = fp::keep_if([&](T v){ return v != m_parent.at(u); }, children);
    } // if

    // Sort children by smallest low-link value
    rg::sort(children,{},[&](T v){ return low_link.at(m_ndt.at(v)); });

    // Return child with smallest low_link
    if( ! children.empty() )
    {
      // Child with smallest low-link value
      T v{children.at(0)};

      // Low link value of child v
      T dt_v{low_link.at(m_ndt.at(v))};

      // If low-link value of child is lt parent, parent gets child llv
      if( dt_v < dt_u )
      {
        return low_link.at(m_ndt.at(v));
      } // if
    } // if

    return std::nullopt;
  };

  for (size_t i{}; i < dfs.size(); ++i)
  {
    T u{dfs.at(i)};

    if( auto v{f_update_low_value(u)} )
    {
      low_link.at(m_ndt.at(u)) = *v;
      for( T w : f_nb(u) ){ st.push(w); }
      visited.insert(u);
    } // if
  } // for

  while ( ! st.empty() )
  {
    T u{st.top()}; st.pop();

    if( auto v{f_update_low_value(u)} )
    {
      low_link.at(m_ndt.at(u)) = *v;

      for( T w : f_nb(u) ){ st.push(w); }

      visited.insert(u);
    } // if
  } // while

  // Compute result
  std::vector<T> out;

  for (auto [v,u] : m_parent)
  {
    if( u == root ){ continue; }

    // Node u is ap, if llk[v] >= dist[u]
    if( low_link.at(m_ndt.at(v)) >= m_ndt.at(u) )
    {
      fmt::print("u: {} v: {}\n", u, v);
      fmt::print("ll(v): {} dist(u): {}\n", low_link.at(m_ndt.at(v)), m_ndt.at(u) );
      out.push_back(u);
    } // if
  } // for

  fmt::print("dts: {}\n", dts);
  fmt::print("dfs: {}\n", dfs);
  fmt::print("llk: {}\n", low_link);

  return out;
} // function: run }}}

} // namespace celaeno::graph::search::aps }}}
