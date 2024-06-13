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

#include <set>
#include <stack>
#include <algorithm>

#include <celaeno/aliases.hpp>
#include <celaeno/concepts.hpp>
#include <celaeno/fun/fun.hpp>

#include <celaeno/graph/graph.hpp>

#include <celaeno/graph/search/bfs.hpp>


namespace celaeno::graph::operations::balance::unbalance
{

namespace
{

using namespace celaeno::aliases;
using namespace celaeno::concepts;

namespace ns_log = celaeno::log;
namespace ns_search = celaeno::graph::search;

using Ops = celaeno::graph::Ops;
using celaeno::fun::fn::fn;

}

// fn: run {{{
template<SignedIntegral I, typename V>
V& run(I root, Ops const& ops, V& view)
{
  [[maybe_unused]] ns_log::Timer timer("celaeno::graph::operations::balance::unbalance");

  auto bfs{ns_search::bfs::run(root,ops)};

  auto f_is_dummy = [](I u){ return u < 0; };

  std::set<I> visited;

  std::stack<I> st;

  auto f_rm = [&](I u)
  {
    if( not f_is_dummy(u) ) { return; }

    auto preds{ops.preds(u)};
    auto succs{ops.succs(u)};

    // Middle
    if( preds.size() == 1 && succs.size() == 1 )
    {
      auto [parent,child] = std::tie(preds.at(0),succs.at(0));

      // Erase from graph
      ops.unlink(parent,u);
      ops.unlink(u,child);
      ops.link(parent,child);

      // Erase from view
      if ( view.nl.contains(u) ){ view.nl.erase(u); }

      st.push(parent);
      st.push(child);
    } // if
    // Dangling
    else if (preds.size() == 1 && succs.size() == 0)
    {
      auto parent{preds.at(0)};

      // Erase from graph
      ops.unlink(parent,u);

      // Erase from view
      if ( view.nl.contains(u) ){ view.nl.erase(u); }

      st.push(parent);
    } // else if
    else if (preds.size() == 0 && succs.size() == 1)
    {
      auto child{succs.at(0)};

      // Erase from graph
      ops.unlink(u,child);

      // Erase from view
      if ( view.nl.contains(u) ){ view.nl.erase(u); }

      st.push(child);
    } // else if
  };

  for (I u : bfs)
  {
    f_rm(u);
  } // for

  while (! st.empty())
  {
    I u{st.top()}; st.pop();
    f_rm(u);
  } // while

  // Remove deleted nodes from layer -> nodes map
  auto layers = fn(view.nl).val().sort().vec();

  // Check if a layer has been completely erased
  std::erase_if(view.ln, [&](auto&& ln){ return std::ranges::find_if(layers, [&](auto&& n){ return ln.first == n; }) == layers.end(); });

  // Remove nodes from remaining layers, while keeping the original ordering
  for( auto&& i : layers )
  {
    auto& nodes = view.ln.at(i);
    auto it = std::stable_partition(nodes.begin(), nodes.end(), [&](auto&& e){ return view.nl.contains(e); });
    if ( it != nodes.end() )
    {
      nodes.erase(it, nodes.end());
    } // if
  } // for

  return view;
} // fn: run }}}

} // namespace celaeno::graph::operations::balance::unbalance
