//
// @author      : Ruan E. Formigoni (ruanformigoni@gmail.com)
// @file        : crossings
// @created     : sábado ago 15, 2020 13:23:38 -03
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

#include <range/v3/all.hpp>
#ifndef NDEBUG
  #include <spdlog/spdlog.h>
#endif

#include <celaeno/aliases.hpp>
#include <celaeno/concepts.hpp>
#include <celaeno/graph/operations/balance/paths.hpp>
#include <celaeno/graph/operations/minimize/crossings/impl.hpp>
#include <celaeno/graph/views/depth.hpp>

// namespace celaeno::graph::operations::minimize::crossings {{{
namespace celaeno::graph::operations::minimize::crossings
{

// using namespaces {{{
using namespace celaeno::concepts;
using namespace celaeno::aliases;
// }}}

// namespaces {{{
namespace balance = celaeno::graph::operations::balance::paths;
// }}}

// run {{{
template<SignedIntegral S, typename N1, typename N2, typename N3, typename E1, typename E2>
decltype(auto) run(S root, N1&& f_p, N2&& f_s, N3&& f_a, E1&& f_l, E2&& f_u)
  requires CallableWith<N1,i64>
  && CallableWith<N2,i64>
  && CallableWith<N3,i64,i64>
  && CallableWith<E1,std::pair<i64,i64>>
  && CallableWith<E2,std::pair<i64,i64>>
{

#ifndef NDEBUG
  spdlog::debug("Algorithm: celaeno::graph::operations::minimize::crossings");
  spdlog::set_level(spdlog::level::debug);
#endif
  //
  // @ Sugiyama algorithm requires a k-layered bipartite graph
  //
  balance::run(root,
    std::forward<N1>(f_p),
    std::forward<N2>(f_s),
    std::forward<E1>(f_l),
    std::forward<E2>(f_u)
  );

  //
  // @ Perform crossing minimization
  //
  return impl::run(root,
    std::forward<N1>(f_p),
    std::forward<N2>(f_s),
    std::forward<N3>(f_a)
  );

} // function: run }}}

} // celaeno::graph::operations::minimize::crossings }}}
