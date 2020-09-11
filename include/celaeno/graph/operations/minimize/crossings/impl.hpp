// vim: set expandtab fdm=marker ts=2 sw=2 tw=80 et :
//
// @author      : Ruan E. Formigoni (ruanformigoni@gmail.com)
// @file        : impl
// @created     : quarta jun 24, 2020 11:56:13 -03
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
#include <ranges>
#include <celaeno/aliases.hpp>
#include <celaeno/concepts.hpp>
#include <celaeno/heuristics/barycenter.hpp>

// namespace celaeno::graph::operations::minimize::crossings::impl {{{
namespace celaeno::graph::operations::minimize::crossings::impl
{

// Macros {{{
#define assertm(exp, msg) assert(((void)msg, exp))
// }}}

// Using namespaces {{{
using namespace celaeno::concepts;
// }}}

// namespaces {{{
namespace barycenter = celaeno::heuristics::barycenter;
// }}}

// fn: bor {{{
template<Matrix M>
decltype(auto) bor(M&& m)
{
  // R will be used as an r-value of M
  using R = typename std::decay_t<M>;

  // Namespaces
  namespace rg = std::ranges;

  assertm(m.size() > 0, "Empty matrix");

  auto sort = [](R&& _m) -> decltype(auto)
  {
    rg::sort(_m,[&](auto&& lhs, auto&& rhs)
    {
      return barycenter::run(lhs) < barycenter::run(rhs);
    });
    return _m;
  };

  return sort(std::move(m));
} // function: bor }}}

// fn: boc {{{
template<Matrix M>
decltype(auto) boc(M&& m)
{
  using std::vector;

  // R will be used as an r-value of M
  using R = typename std::decay_t<M>;

  auto reverse = [](R&& _m) -> decltype(auto)
  {
    R rev{std::size_t{_m.at(0).size()},vector<bool>(std::size_t{_m.size()})};

    for (std::size_t i{}; i < _m.size(); ++i)
    {
      for (std::size_t j{}; j < _m.at(i).size(); ++j)
      {
        rev.at(j).at(i) = _m.at(i).at(j);
      } // for: j
    } // for: i
    return rev;
  };

  return reverse(std::move(bor(std::move(reverse(std::move(m))))));
} // function: boc }}}

// fn: ro TODO {{{
template<Matrix M>
decltype(auto) ro(M&& m)
{
  return m;
} // function: ro }}}

} // namespace celaeno::graph::operations::minimize::crossing::impl }}}
