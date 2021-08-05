// vim: set expandtab fdm=marker ts=2 sw=2 tw=80 et :
//
// @author      : Ruan E. Formigoni (ruanformigoni@gmail.com)
// @file        : zig-zag
// @created     : Tuesday Jul 27, 2021 21:02:56 UTC
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

#include <stack>
#include <vector>
#include <set>
#include <unordered_map>
#include <map>

#include <spdlog/spdlog.h>
#include <fmt/core.h>
#include <fplus/fplus.hpp>
#include <range/v3/all.hpp>

#include <celaeno/concepts.hpp>
#include <celaeno/aliases.hpp>
#include <celaeno/graph/graph.hpp>

namespace celaeno::graph::search::zig_zag
{
// Macros {{{
#define assertm(exp, msg) assert(((void)msg, exp))
// }}}

// Using declarations {{{
using Ops = celaeno::graph::Ops;
// }}}

// Namespaces {{{
namespace fp = fplus;
namespace rg = ranges;
// }}}

// Using namespaces {{{
using namespace celaeno::concepts;
using namespace celaeno::aliases;
// }}}

// enum: DIR {{{
enum class DIR
{
  OUT_IN,
  IN_OUT,
};
// }}}

// fn: run {{{
template<SignedIntegral T, typename C = std::function<bool(i64)>>
std::vector<T> run(T root,
  Ops ops,
  std::vector<std::pair<T,T>>& path = {},
  std::vector<T>& cycles = {},
  C&& f_cb = [](auto){ return false; }
)
{
#if ! defined(NDEBUG) && defined(DEBUG_SHOW_ALG)
  spdlog::set_level(spdlog::level::debug);
  spdlog::debug("Algorithm: celaeno::graph::search::zig_zag");
#endif
  // Keep track of current direction
  DIR dir{DIR::OUT_IN};

  // Zig-Zag ordered result
  std::vector<T> out;

  // Stack of nodes with unvisited edges
  std::stack<std::pair<T,T>> stack_unvisited;

  // Keep track of visited edges
  std::set<T> set_visited;

  // Push starting node to stack
  stack_unvisited.push(std::make_pair(root,root));

  // Execute algorithm while there are unvisited nodes
  while( ! stack_unvisited.empty() )
  {
    // Retrieve top of stack
    auto [p,u] {stack_unvisited.top()}; stack_unvisited.pop();

    if( set_visited.contains(u) )
    {
      continue;
    } // if
    else
    {
      out.push_back(u);
    } // else

    // Perform callback
    if( f_cb(u) ){ break; }

    // Save curr edge to path
    if ( p != u ) { path.emplace_back(p,u); } // if

    // Mark u as visited
    set_visited.emplace(u);

    // Get preds and succs
    auto preds{ops.preds(u)};
    auto succs{ops.succs(u)};

    // Adjust direction accordingly
    if ( dir == DIR::OUT_IN)
    {
      if( succs.size() == 2 ){ dir = DIR::IN_OUT; }
    } // if
    else
    {
      if( preds.size() == 2 ){ dir = DIR::OUT_IN; }
    } // else

    // Check cycles
    if(auto cycle{fp::keep_if([&,u=u,p=p](auto e)
      {
        if( e != p && set_visited.contains(e) )
        {
          path.emplace_back(u,e);
          return true;
        } // if
        return false;
      }, fp::append(preds,succs))};
      ! cycle.empty()
    )
    {
      rg::copy(cycle,std::back_inserter(cycles));
    } // if

    // Filter visited nodes
    preds = fp::keep_if([&](auto e){ return ! set_visited.contains(e); }, preds);
    succs = fp::keep_if([&](auto e){ return ! set_visited.contains(e); }, succs);

    // Push to stack based on direction
    if( dir == DIR::OUT_IN )
    {
      rg::for_each(succs,[&,u=u](auto e){ stack_unvisited.push({u,e}); });
      rg::for_each(preds,[&,u=u](auto e){ stack_unvisited.push({u,e}); });
    } // if
    else
    {
      rg::for_each(preds,[&,u=u](auto e){ stack_unvisited.push({u,e}); });
      rg::for_each(succs,[&,u=u](auto e){ stack_unvisited.push({u,e}); });
    } // else

  } // while

  cycles = {4,6};

  return out;
} // }}}

} // namespace celaeno::graph::search::zig_zag
