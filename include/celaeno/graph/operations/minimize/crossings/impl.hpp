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
#include <execution>
#include <fmt/core.h>
#include <fmt/ranges.h>
#include <fplus/fplus.hpp>
#include <range/v3/all.hpp>
#include <celaeno/aliases.hpp>
#include <celaeno/concepts.hpp>
#include <celaeno/heuristics/barycenter.hpp>
#include <celaeno/graph/operations/count/crossings.hpp>
#include <celaeno/graph/representations/incidence.hpp>

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
namespace rg = ranges;
namespace rv = ranges::views;
namespace fp = fplus;
namespace depth = celaeno::graph::views::depth;
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
template<typename C, typename L>
decltype(auto) bor(C const& c, L const& l)
{
  assertm(c.size() > 0, "Empty matrix");

  auto sort = []<typename _C>(_C _c, L _l) -> auto
  {
    auto zipped_values{fp::zip(_c,_l)};

    std::sort(std::execution::par_unseq, zipped_values.begin(),zipped_values.end(),
    [&](auto&& lhs, auto&& rhs)
    {
      return barycenter::run(lhs.first) < barycenter::run(rhs.first);
    });

    return fp::unzip(zipped_values);
  };

  return sort(c,l);
} // function: bor }}}

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
template<SignedIntegral S, typename F1, typename F2, typename F3>
decltype(auto) phase_1(S root, F1&& f_pred, F2&& f_succ, F3&& f_adj)
{

  //
  // @ Create a depth view
  //
  auto depth_view{depth::run(root,f_pred,f_succ).first};

  //
  // @ Generate layer pairs
  //
  std::size_t layer_count{depth_view.size()};
  assertm(layer_count != 0, "Layer count equals zero");
  auto layers = fp::overlapping_pairs(fp::numbers({},layer_count));

  auto sort_by_barycenter = [&]
  {
    // Compute overlapping and non overlapping pairs separately
    auto non_overlapping{fp::keep_if([i=0](auto e) mutable { return (i++%2 == 0); },layers)};
    auto overlapping{fp::keep_if([i=0](auto e) mutable { return (i++%2 != 0); },layers)};

    auto process = [&](auto il1, auto il2) -> void
    {

      // Create matrix for layers l1 and l2 {{{
      auto& l1{depth_view.at(il1)};
      auto& l2{depth_view.at(il2)};
      auto m0 {incidence::run(l1,l2,f_adj)};
      // }}}

      // Step 2: Br {{{
      auto r1{bor(m0,l1)};
      auto& m1{r1.first};
      if ( ccrossings::run(r1.second,l2,f_succ) < ccrossings::run(l1,l2,f_succ) )
      {
        l1 = std::move(r1.second);
      } // if
      // }}}

      // Step 4: Bc {{{
      auto r2{bor(reverse(m1),l2)};

      if (ccrossings::run(l1,r2.second,f_succ) < ccrossings::run(l1,l2,f_succ))
      {
        l2 = std::move(r2.second);
      } // if
      // }}}

    };

    //
    // @ Enqueue and execute threads with non-overlapping pairs
    //
    std::vector<std::thread> thread_no;
    for (auto il : non_overlapping)
    {
      thread_no.emplace_back([=]{ process(il.first,il.second); });
    } // for
    for (auto& t : thread_no) { t.join(); }

    //
    // @ Enqueue and execute threads with overlapping pairs
    //
    std::vector<std::thread> thread_ov;
    for (auto il : overlapping)
    {
      thread_ov.emplace_back([=]{ process(il.first,il.second); });
    } // for
    for (auto& t : thread_ov) { t.join(); }
  };

  rg::for_each(rv::iota(0,11),[&](auto){ sort_by_barycenter(); });

  return depth_view
    | rv::transform([](auto&& e){ return e.second; })
    | rg::to<std::vector<std::vector<i64>>>;

} // }}}

} // namespace celaeno::graph::operations::minimize::crossing::impl }}}
