// vim: set expandtab fdm=marker ts=2 sw=2 tw=100 et :
//
// @author      : Ruan E. Formigoni (ruanformigoni@gmail.com)
// @file        : crossings
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

#include <range/v3/all.hpp>
#include <celaeno/aliases.hpp>
#include <celaeno/graph/concepts.hpp>
#include <celaeno/graph/representations/incidence.hpp>
#include <celaeno/graph/operations/count/crossings/impl.hpp>

// namespace celaeno::graph::operations::count::crossings {{{
namespace celaeno::graph::operations::count::crossings
{

// namespaces {{{
namespace rg = ranges;
namespace rv = ranges::views;
namespace incidence = celaeno::graph::representations::incidence;
namespace crossings = celaeno::graph::operations::count::crossings::impl;
// }}}

// Using namespaces {{{
using namespace celaeno::graph::concepts;
// }}}

// function: run {{{
template<SignedIntegral T, Neighbors N1, Neighbors N2>
auto run(T root, N1&& p, N2&& s)
{
  // Create the incidence matrix
  auto ms {incidence::run(root, p, s)};

  // Return cost
  return rg::accumulate(rv::all(ms),0,[&](auto&& c, auto&& n){ return c + crossings::run(n); });

} // function: run }}}

// Usage with fold expressions {{{
template<Matrix... MS>
decltype(auto) run(MS&&... ms)
{
  return (crossings::run(std::forward<MS>(ms)) + ...);
} // function: run }}}

} // celaeno::graph::operations::count::crossings }}}
