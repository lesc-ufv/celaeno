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
#include <celaeno/aliases.hpp>
#include <celaeno/concepts.hpp>
#include <celaeno/functional.hpp>
#include <celaeno/graph/concepts.hpp>
#include <celaeno/graph/operations/balance/paths.hpp>
#include <celaeno/graph/operations/minimize/crossings/impl.hpp>
#include <celaeno/graph/representations/incidence.hpp>
#include <celaeno/graph/views/depth.hpp>

// namespace celaeno::graph::operations::minimize::crossings {{{
namespace celaeno::graph::operations::minimize::crossings
{

// namespaces {{{
namespace fun = celaeno::functional;
namespace balance = celaeno::graph::operations::balance::paths;
namespace incidence = celaeno::graph::representations::incidence;
namespace minimize = celaeno::graph::operations::minimize::crossings::impl;
namespace depth = celaeno::graph::views::depth;
// }}}

// Using namespaces {{{
using namespace celaeno::concepts;
using namespace celaeno::graph::concepts;
// }}}

// run {{{
template<SignedIntegral T, typename N1, typename N2, typename E1, typename E2>
decltype(auto) run(T root, N1&& f_p, N2&& f_s, E1&& f_l, E2&& f_u)
{
  //
  // @ Sugiyama algorithm requires a k-layered bipartite graph
  //
  balance::run(root,
    std::forward<N1>(f_p), std::forward<N2>(f_s),
    std::forward<E1>(f_l), std::forward<E2>(f_u)
  );

  //
  // @ Check if two vertices are adjacent
  //
  auto f_adjacent = [&f_s]<typename V>(V _u, V _v) -> bool
  {
    auto successors{f_s(_u)};
    return std::ranges::find(successors,_v) != std::ranges::end(successors);
  };

  //
  // @ Create layers vector
  //
  auto topo_view{depth::run(root,std::forward<N1>(f_p),std::forward<N2>(f_s)).first};
  auto layers{fun::values(topo_view)};

  //
  // @ Minimize crossings of the matrix realization
  //
  return minimize::phase_1(layers,f_adjacent);

} // function: run

// }}}

} // celaeno::graph::operations::minimize::crossings }}}
