// vim: set expandtab fdm=marker ts=2 sw=2 tw=100 et :
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
#include <utility>

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
using namespace celaeno::aliases;
// }}}

// namespaces {{{
namespace rg = ranges;
namespace ra = ranges::actions;
namespace rv = ranges::views;
namespace fp = fplus;
namespace ns_views = celaeno::graph::views;
namespace barycenter = celaeno::heuristics::barycenter;
namespace incidence = celaeno::graph::representations::incidence;
namespace ccrossings = celaeno::graph::operations::count::crossings;
// }}}

// fn: reverse {{{
template<Matrix M>
[[nodiscard]] decltype(auto) reverse(M&& m)
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
[[nodiscard]] decltype(auto) bor(C c, L l)
{
  assertm(c.size() > 0, "Empty matrix");

  auto sort = []<typename _C>(_C _c, L _l) -> auto
  {
    auto zipped_values{fp::zip(_c,_l)};

    std::sort(zipped_values.begin(),zipped_values.end(),
    [&](auto&& lhs, auto&& rhs)
    {
      return barycenter::run(lhs.first) < barycenter::run(rhs.first);
    });

    return fp::unzip(zipped_values);
  };

  return sort(c,l);
} // function: bor }}}

// fn: ror {{{
template<Matrix M, Range L>
std::optional<std::pair<std::decay_t<M>,std::decay_t<L>>> ror(M m, L col_layer)
{
  // Get column matrix
  auto _m {reverse(m)};

  // Check if any swap occurred
  bool b_swap{false};

  // Swap adjacent columns/nodes if they have the same barycenter
  for (i64 i{}; i < _m.size()-1; ++i)
  {
    if(  barycenter::run(_m.at(i)) == barycenter::run(_m.at(i+1)) )
    {
      b_swap = true;
      _m.at(i+1) = std::exchange(_m.at(i),_m.at(i+1));
      col_layer.at(i+1) = std::exchange(col_layer.at(i),col_layer.at(i+1));
    }
  } // for

  // Return values if at least one swap took place
  if( b_swap )
  {
    return std::make_pair(reverse(_m),col_layer);
  } // if

  return std::nullopt;
} // function: ror }}}

// fn: roc {{{
template<Matrix M, Range L>
std::optional<std::pair<std::decay_t<M>,std::decay_t<L>>> roc(M m, L row_layer)
{
  // Check if any swap occurred
  bool b_swap{false};

  // Swap adjacent columns/nodes if they have the same barycenter
  for (i64 i{}; i < m.size()-1; ++i)
  {
    if(  barycenter::run(m.at(i)) == barycenter::run(m.at(i+1)) )
    {
      b_swap = true;
      m.at(i+1) = std::exchange(m.at(i),m.at(i+1));
      row_layer.at(i+1) = std::exchange(row_layer.at(i),row_layer.at(i+1));
    }
  } // for

  // Return values if at least one swap took place
  if( b_swap )
  {
    return std::make_pair(m,row_layer);
  } // if

  return std::nullopt;
} // function: roc }}}

// fn: run {{{
template<SignedIntegral S, typename N1, typename N2, typename N3>
[[nodiscard]] decltype(auto) run(S root, N1&& f_pred, N2&& f_succ, N3&& f_adj)
  requires CallableWith<N1,i64>
  && CallableWith<N2,i64>
  && CallableWith<N3,i64,i64>
{

  //
  // @ Create a depth view
  //
  auto depth_view{ns_views::depth::run(root,f_pred,f_succ).ln};

  //
  // @ Generate layer pairs
  //
  std::size_t layer_count{depth_view.size()};
  assertm(layer_count != 0, "Layer count equals zero");
  auto layers = fp::overlapping_pairs(fp::numbers({},layer_count));

  auto impl_phase_1 =
  [&]<Matrix M>(M m0, auto& l1, auto &l2)
  {
    while(true)
    {
      //
      // Step 2: Calculate row barycenter
      //
      auto [m1,new_l1] {bor(m0,l1)};

      //
      // Step 3: Evaluate solution
      //
      if ( ccrossings::run(new_l1,l2,f_succ) < ccrossings::run(l1,l2,f_succ) )
      {
        l1 = std::move(new_l1);
      } // if

      //
      // Step 4: Calculate col barycenter
      //
      auto [m2,new_l2] {bor(reverse(m1),l2)};

      //
      // Step 5: Evaluate Solution
      //
      if (ccrossings::run(l1,new_l2,f_succ) < ccrossings::run(l1,l2,f_succ))
      {
        l2 = std::move(new_l2);
      } // if

      //
      // Step 6: Stop if M0 and M2 are equal
      //
      if( auto rev_m2{reverse(m2)}; rev_m2 == m0 ){ return rev_m2; } // if
      else { m0 = rev_m2; } // else
    } // while
  }; // lamb: impl_phase_1

  auto phase_1 =
  [&]
  {
    // Compute best number of crossings
    i64 best_crossings{};

    rg::for_each(layers,[&](auto e)
    {
      best_crossings += ccrossings::run(depth_view.at(e.first),depth_view.at(e.second),f_succ);
    });

    // Save best node arrangements
    auto best_arrangement{depth_view};

    // Iterate while solutions keeps improving
    while( true )
    {
      i64 curr_crossings{};

      for( auto [il1,il2] : layers )
      {
        auto& l1{depth_view.at(il1)};
        auto& l2{depth_view.at(il2)};
        auto m2{impl_phase_1(incidence::run(l1,l2,f_adj),l1,l2)};
      } // for

      rg::for_each(layers,[&](auto e)
      {
        curr_crossings += ccrossings::run(depth_view.at(e.first),depth_view.at(e.second),f_succ);
      });

      if( curr_crossings >= best_crossings )
      {
        break;
      }
      else if (curr_crossings < best_crossings)
      {
        best_arrangement = depth_view;
        best_crossings = curr_crossings;
      } // else

      layers = ra::reverse(layers);
    } // while

    depth_view = best_arrangement;
  }; // lamb: phase_1

  //
  // Execute ror and roc until both fail
  //
  for ( bool stop{false}; stop != true; )
  {
    for( auto [il1,il2] : layers )
    {
      auto& l1{depth_view.at(il1)};
      auto& l2{depth_view.at(il2)};

      //
      // Step 7
      //
      auto opt_ror{ror(incidence::run(l1,l2,f_adj),l2)};

      if( opt_ror )
      {
        // spdlog::info("opt_ror");
        l2 = opt_ror->second;
        phase_1();
      }

      //
      // Step 8
      //
      auto opt_roc{roc(incidence::run(l1,l2,f_adj),l1)};
      if( opt_roc )
      {
        // spdlog::info("opt_roc");
        l1 = opt_roc->second;
        phase_1();
      }

      if( ! opt_ror && ! opt_roc ){ stop = true; }

    } // for

    layers = ra::reverse(layers);
  } // while

  return fp::get_map_values(depth_view);
} // }}}

} // namespace celaeno::graph::operations::minimize::crossing::impl }}}
