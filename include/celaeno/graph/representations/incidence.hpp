// vim: set expandtab fdm=marker ts=2 sw=2 tw=80 et :
//
// @author      : Ruan E. Formigoni (ruanformigoni@gmail.com)
// @file        : incidence
// @created     : domingo jun 14, 2020 12:17:32 -03
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
#include <concepts>
#include <iterator>

#ifndef NDEBUG
  #include <spdlog/spdlog.h>
#endif

#include <celaeno/aliases.hpp>
#include <celaeno/concepts.hpp>
#include <celaeno/graph/representations/incidence/impl.hpp>
#include <celaeno/graph/views/depth.hpp>

// namespace celaeno::graph::representations::incidence {{{
namespace celaeno::graph::representations::incidence
{


// Macros {{{
#define assertm(exp, msg) assert(((void)msg, exp))
// }}}

// Namespaces {{{
namespace depth = celaeno::graph::views::depth;
namespace incidence = celaeno::graph::representations::incidence::impl;
// }}}

// Using namespaces {{{
using namespace celaeno::concepts;
using namespace celaeno::aliases;
// }}}

// fn: run {{{

//
// @ Given an input integral value, a function P to return the predecessors of a
// vertex, a function S to return the successors of a vertex, and a function A
// to return if two nodes are adjacent, the function returns the incidence
// matrix of a graph
//
template<SignedIntegral T, typename P, typename S, typename A>
auto run(T root, P&& f_pred, S&& f_succ, A&& f_adj)
  requires CallableWith<P,i64>
  &&
  CallableWith<S,i64>
  &&
  CallableWith<A,i64,i64>
{

#ifndef NDEBUG
  spdlog::set_level(spdlog::level::debug);
  spdlog::debug("Algorithm: celaeno::graph::representations::incidence");
#endif

    // depth view: @level → vertices && @vertex → level
    auto [lvs,_] = depth::run(root,f_pred,f_succ);

    // Number of levels must be > 1
    assertm(lvs.size() > 1, "Number of layers of the input graph is less 2");

    // Return the incidence matrix
    return incidence::all(
      std::forward<decltype(lvs)>(lvs),
      std::forward<decltype(f_adj)>(f_adj)
    );

} // function: run }}}

// fn: run {{{

template<Range R, typename A>
auto run(R&& l1, R&& l2, A&& f_adj)
  requires CallableWith<A,i64,i64>
{

#ifndef NDEBUG
  spdlog::set_level(spdlog::level::debug);
  spdlog::debug("Algorithm: celaeno::graph::representations::incidence");
#endif

  return incidence::single(
    std::forward<R>(l1),
    std::forward<R>(l2),
    std::forward<A>(f_adj)
  );

} // function: run }}}

} // namespace celaeno::graph::representations::incidence }}}
