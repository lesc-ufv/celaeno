// vim: set expandtab fdm=marker ts=2 sw=2 tw=80 et :
//
// @author      : Ruan E. Formigoni (ruanformigoni@gmail.com)
// @file        : depth
// @created     : Sunday Feb 23, 2020 18:35:30 -03
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

#include <map>
#include <type_traits>

#include <range/v3/all.hpp>
#ifndef NDEBUG
  #include <spdlog/spdlog.h>
#endif

#include <celaeno/concepts.hpp>
#include <celaeno/graph/search/kahn.hpp>

// namespace celaeno::graph::views::depth {{{
namespace celaeno::graph::views::depth
{

// Namespaces {{{
namespace rg = ranges;
namespace rv = ranges::views;
namespace kahn = celaeno::graph::search::kahn;
// }}}

// Using namespaces {{{
using namespace celaeno::concepts;
using namespace celaeno::aliases;
// }}}

// Algorithm {{{
template<SignedIntegral T, typename P, typename S>
auto run(T root, P&& f_pred, S&& f_succ)
  -> std::pair< std::map<T,std::vector<T>>, std::map<T,T> >
  requires CallableWith<P,i64>
  && CallableWith<S,i64>
{

#if ! defined(NDEBUG) && defined(DEBUG_SHOW_ALG)
  spdlog::set_level(spdlog::level::debug);
  spdlog::debug("Algorithm: celaeno::graph::views::depth");
#endif

  // vertex -> layer
  std::map<T,T> vl;

  // layer -> vertices
  std::map<T,std::vector<T>> lv;

  // Emplace in lv and vl
  auto emplace = [&](auto&& l, auto&& v) { vl.emplace(v,l); lv[l].push_back(v); };

  // Perform Topological sorting
  auto topo {kahn::run(root,std::forward<P>(f_pred),std::forward<S>(f_succ))};

  // lambda to get all levels of a set vertices
  auto levels = [&](auto&& vs)
  {
    return rv::transform(vs,[&vl](auto&& u){ return vl.at(u); });
  };

  // lambda to get the predecessor with max layer
  auto max = [&](auto&& ps) { return std::ranges::max(levels(ps)); };

  // Split by layer
  rg::for_each(topo,[&](auto&& v)
  {
    auto preds {f_pred(v)};
    // If is in the first layer (has no predecessors), emplace 0
    if( preds.size() == 0 ) { emplace(0,v); }
    // else, emplace max layer of the predecessors + 1
    else { emplace(max(preds)+1,v); }
  });

  return { lv, vl };
} // function: run }}}

} // namespace celaeno::graph::view::depth

// }}}
