// vim: set expandtab fdm=marker ts=2 sw=2 tw=100 et :
//
// @author      : Ruan E. Formigoni (ruanformigoni@gmail.com)
// @file        : pipeline
// @created     : sábado ago 15, 2020 16:30:36 -03
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

#include <fplus/fplus.hpp>
#include <celaeno/aliases.hpp>
#include <celaeno/concepts.hpp>
#include <celaeno/graph/concepts.hpp>
#include <celaeno/graph/views/proximity.hpp>
#include <celaeno/graph/operations/count/crossings.hpp>
#include <celaeno/graph/representations/incidence.hpp>

// namespace celaeno::graph::operations::count::crossings::pipeline {{{
namespace celaeno::graph::operations::count::crossings::pipeline
{

// namespaces {{{
namespace fw = fplus::fwd;
namespace proximity = celaeno::graph::views::proximity;
namespace incidence = celaeno::graph::representations::incidence;
// }}}

// Using namespaces {{{
using namespace celaeno::concepts;
using namespace celaeno::graph::concepts;
// }}}

// function: run {{{
template<SignedIntegral T, Neighbors N1, Neighbors N2>
auto run(T t, N1&& p, N2&& s)
{
  // Create the proximity view
  auto [lv,_] = proximity::run(1,p,s);
  // Check if vertices ab are adjacent
  auto adj = [&](auto&& a, auto&& b) { return ! fw::apply(p(a),fw::append(s(b))).empty(); };
  // Calculate the number of layers of the graph
  auto layers {fw::apply(lv,fw::get_map_keys(),fw::unique(),fw::size_of_cont())};
  // Get a layer from the proximity view
  auto layer = [&lv](i64 idx)
  {
    return fw::apply(lv
      , fw::drop_if([&idx](auto&& e){ return e.first != idx; })
      , fw::get_map_values()
      , fw::sort()
    );
  };
  // Create the incidence matrix
  auto ms {incidence::run(layer, adj, layers)};

  // Calculate cost
  i64 cost{};
  for (decltype(ms.size()) i{}; i < ms.size(); ++i)
  {
    cost += crossings::run(ms.at(i));
  } // for: i < l.size()

  return cost;
} // function: run }}}

} // celaeno::graph::operations::count::crossings::pipeline }}}
