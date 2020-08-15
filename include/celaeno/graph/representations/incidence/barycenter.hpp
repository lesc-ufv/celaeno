// vim: set expandtab fdm=marker ts=2 sw=2 tw=100 et :
//
// @author      : Ruan E. Formigoni (ruanformigoni@gmail.com)
// @file        : barycenter
// @created     : quinta jun 11, 2020 10:32:29 -03
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

# pragma once
#include <fplus/fplus.hpp>
#include <range/v3/all.hpp>
#include <concepts>
#include <type_traits>
#include <celaeno/concepts.hpp>
#include <celaeno/graph/concepts.hpp>

// namespace celaeno::graph::operations::incidence::barycenter {{{
namespace celaeno::graph::operations::incidence::barycenter
{

// Aliases {{{
namespace fp = fplus;
namespace fw = fplus::fwd;
// }}}

// Using namespaces {{{
using namespace celaeno::concepts;
using namespace celaeno::graph::concepts;
// }}}

// Algorithm {{{
template<Vector V>
auto run(V&& v)
{
  auto index_mult = [i=1](auto&& e) mutable { return e*i++; };

  auto a {fw::apply(v, fw::transform(index_mult), fw::sum())};

  auto b { fp::sum(v) };

  return (b != 0)? static_cast<double>(a)/b : 0;
} // function: run }}}

} // namespace celaeno::graph::operations::incidence::barycenter }}}
