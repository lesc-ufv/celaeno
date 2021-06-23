// vim: set expandtab fdm=marker ts=2 sw=2 tw=100 et :
//
// @author      : Ruan E. Formigoni (ruanformigoni@gmail.com)
// @file        : kahn
// @created     : Wednesday Apr 08, 2020 11:42:02 -03
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

#include <vector>
#include <deque>
#include <unordered_map>

#include <fplus/fplus.hpp>
#ifndef NDEBUG
  #include <spdlog/spdlog.h>
#endif

#include <celaeno/aliases.hpp>
#include <celaeno/concepts.hpp>
#include <celaeno/graph/search/bfs.hpp>

// namespace celaeno::graph::search::kahn {{{
namespace celaeno::graph::search::kahn
{

// Namespaces {{{
namespace fp = fplus;
namespace bfs = celaeno::graph::search::bfs;
// }}}

// Using namespaces {{{
using namespace celaeno::concepts;
using namespace celaeno::aliases;
// }}}

// Algorithm {{{
template<SignedIntegral T, typename P, typename S, typename C = std::function<bool(int64_t)>>
std::vector<T> run(T root, P&& f_pred, S&& f_succ, C&& cb = [](auto&&){return false;})
  requires CallableWith<P,i64>
  && CallableWith<S,i64>
  && CallableWith<C,i64>
{

#if ! defined(NDEBUG) && defined(DEBUG_SHOW_ALG)
  spdlog::set_level(spdlog::level::debug);
  spdlog::debug("Algorithm: celaeno::graph::search::kahn");
#endif

  // Topologically sorted result
  std::vector<T> result;

  // Vertices with no incomming edges
  std::deque<T> deque;

  // Removed edges
  std::set<std::pair<T,T>> re;

  // Populate the deque
  auto has_pred = [&f_pred](auto&& v){ return ! f_pred(v).empty(); };
  bfs::run(root,f_pred,f_succ,[&has_pred,&deque](auto&& v)
    { if( ! has_pred(v) ){ deque.push_back(v); } return false; });

  while (! deque.empty() )
  {
    // Get the current vertex
    auto c{deque.front()}; deque.pop_front();

    // Include in the result
    result.push_back(c);

    // Perform the callback
    if( cb(c) ) return result;

    // Get successors
    for( auto s : f_succ(c) )
    {
      // remove edge c -> s
      re.insert({c,s});
      // If s has no more predecessors
      auto is_rm = [&s,&re](auto&& v){ return re.contains({v,s}); };
      auto preds_s {fp::drop_if(is_rm, f_pred(s))};
      // Insert s into the queue
      if( preds_s.empty() ) { deque.push_back(s); }
    }
  } // while: ! initial.empty()
  return result;
} // function: run }}}

} // namespace celaeno::graph::search::kahn }}}
