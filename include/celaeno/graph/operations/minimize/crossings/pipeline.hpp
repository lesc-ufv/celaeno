//
// @author      : Ruan E. Formigoni (ruanformigoni@gmail.com)
// @file        : pipeline
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
#include <fplus/fplus.hpp>
#include <celaeno/aliases.hpp>
#include <celaeno/concepts.hpp>
#include <celaeno/graph/concepts.hpp>
#include <celaeno/graph/views/proximity.hpp>
#include <celaeno/graph/operations/balance/paths.hpp>
#include <celaeno/graph/operations/minimize/crossings.hpp>
#include <celaeno/graph/operations/count/crossings.hpp>
#include <celaeno/graph/representations/incidence.hpp>

// namespace celaeno::graph::operations::minimize::crossings::pipeline {{{
namespace celaeno::graph::operations::minimize::crossings::pipeline
{

// namespaces {{{
namespace fw = fplus::fwd;
namespace proximity = celaeno::graph::views::proximity;
namespace balance = celaeno::graph::operations::balance::paths;
namespace incidence = celaeno::graph::representations::incidence;
namespace minimize = celaeno::graph::operations::minimize::crossings;
// }}}

// Using namespaces {{{
using namespace celaeno::concepts;
using namespace celaeno::graph::concepts;
// }}}

// run {{{
template<SignedIntegral T, Neighbors N1, Neighbors N2, Edge E1, Edge E2>
decltype(auto) run(T t, N1&& p, N2&& s, E1&& u, E2&& l)
{
  auto [dv,_] {proximity::run(t, std::forward<N1>(p), std::forward<N2>(s))};

  balance::run(t,
      std::forward<N1>(p), std::forward<N2>(s),
      std::forward<E1>(u), std::forward<E2>(l)
  );

  auto adj = [&](auto&& a, auto&& b) { return ! fw::apply(p(a),fw::append(s(b))).empty(); };

  auto layer = [&dv](i64 idx)
  {
    return fw::apply(dv
      , fw::drop_if([&idx](auto&& e){ return e.first != idx; })
      , fw::get_map_values()
    );
  };

  // Number of layers of the graph
  auto layers {fw::apply(dv,fw::get_map_keys(),fw::unique(),fw::size_of_cont())};

  // Incidence matrices of the graph
  auto&& ms {incidence::run(layer, adj, layers)};

  // Minimize crossings
  return minimize::run(std::move(ms),layer,layers);

} // function: run

// }}}

} // celaeno::graph::operations::minimize::crossings::pipeline }}}
