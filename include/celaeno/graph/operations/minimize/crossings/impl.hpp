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
#include <celaeno/graph/operations/count/crossings.hpp>
#include <celaeno/graph/representations/incidence.hpp>

// TODO remove
#include <fmt/core.h>
#include <fmt/ranges.h>

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
namespace incidence = celaeno::graph::representations::incidence;
namespace ccrossings = celaeno::graph::operations::count::crossings;
// }}}

// fn: reverse {{{
template<Matrix M>
decltype(auto) reverse(M&& m)
{
  using matrix_t = typename std::decay_t<M>;

  matrix_t rev{std::size_t{m.at(0).size()},std::vector<bool>(std::size_t{m.size()})};

  for (std::size_t i{}; i < m.size(); ++i)
  {
    for (std::size_t j{}; j < m.at(i).size(); ++j)
    {
      rev.at(j).at(i) = m.at(i).at(j);
    } // for: j
  } // for: i

  return rev;
} // }}}

// fn: bor {{{
template<Matrix M>
decltype(auto) bor(M&& m)
{
  assertm(m.size() > 0, "Empty matrix");

  auto sort = [](std::decay_t<M>&& _m) -> decltype(auto)
  {
    std::ranges::sort(_m,[&](auto&& lhs, auto&& rhs)
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
  return reverse(std::move(bor(std::move(reverse(std::forward<M>(m))))));
} // function: boc }}}

// fn: ror TODO {{{
template<Matrix M>
decltype(auto) ror(M&& m)
{
  return m;
} // function: ror }}}

// fn: roc TODO {{{
template<typename M>
decltype(auto) roc(M&& m)
{
  return m;
} // function: roc }}}

// fn: phase_1 {{{
template<typename L, typename FNA>
decltype(auto) phase_1(L&& layers, FNA&& f_adjacent)
{
  using E = typename std::decay_t<L>::value_type::value_type;

  std::vector<std::vector<E>> result;

  // Step 1 - Keep the best ordering of layers
  std::decay_t<L> best_orderings{layers};

  assertm(layers.size() >= 2, "Layer count is less than 2");

  for (auto it{layers.cbegin()}; it != layers.cend(); ++it)
  {
    // Step 2 - Sort layer by row barycenter
    auto adjacent = []<typename T, typename U>(T&& t, U&& u)
    {
      return rg::contains(succ(t),u);
    };

    auto matrix{ incidence::run( pred, succ,  ) };
  }

} // namespace celaeno::graph::operations::minimize::crossing::impl }}}
