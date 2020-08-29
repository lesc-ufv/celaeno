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

#include <map>
#include <utility>
#include <iterator>
#include <fplus/fplus.hpp>
#include <range/v3/all.hpp>
#include <celaeno/aliases.hpp>
#include <celaeno/graph/concepts.hpp>
#include <celaeno/graph/representations/incidence/barycenter.hpp>
#include <celaeno/graph/operations/count/crossings.hpp>

// TODO REMOVE
#include <fmt/core.h>
#include <fmt/ranges.h>

// namespace celaeno::graph::operations::minimize::crossings::impl {{{
namespace celaeno::graph::operations::minimize::crossings::impl
{

// Macros {{{
#define assertm(exp, msg) assert(((void)msg, exp))
// }}}

// namespaces {{{

namespace fp = fplus;
namespace fw = fplus::fwd;
namespace rg = ranges;
namespace rv = ranges::views;
namespace ra = ranges::actions;
namespace barycenter = celaeno::graph::operations::incidence::barycenter;
namespace count_crossings = celaeno::graph::operations::count::crossings;
template<typename T> using ref = std::reference_wrapper<T>;

// }}}

// Using namespaces {{{
using namespace celaeno::graph::concepts;
// }}}

// Helpers {{{

template<Matrix M>
auto swap_rc(M&& m)
{
  // Get a column by index
  auto col = [&](auto i){return fp::transform([&i](auto&& v){return v.at(i);},m);};

  // Get the number of columns
  assertm(m.at(0).size(), "Empty incidence matrix");

  // Return new container with columns as rows
  return fw::apply(fp::numbers(std::size_t{},m.at(0).size())
    , fw::transform([&](auto i){return col(i);})
  );

} // function: swap_rows_and_cols

// }}}

// Algorithm {{{

template<Matrix M>
void reorder(M& m)
{
  //
  // @ Calculate the barycenter for the incidence matrix
  // @ Swap rows of the matrix with equal barycenters
  //
  auto bs{fw::apply(m,fw::transform([](auto&& e){return barycenter::run(e);}))};

  assertm(bs.size() == m.size(), "Barycenter vec does not match matrix size");

  for (std::size_t i{}; i < m.size()-1; ++i)
  {
    if (bs.at(i) == bs.at(i+1))
    {
      m.at(i+1) = std::exchange(m.at(i),m.at(i+1));
    } // if bs.at(i) == bs.at(i+1)
  } // for: i < bs.size()
} // function: ror

// @mr  → Matrix realization
// @layer → lambda to obtain the nodes a graph layer
// @depth → depth of the topologically sorted graph
template<Matrices MS, Layer L>
auto run(MS&& mr, L&& f_layer, size_t depth)
  -> typename std::vector<std::decay_t<decltype(f_layer(0))>>
{

  // Aliases {{{

  // Graph layer
  using Layer = typename std::decay_t<decltype(f_layer(0))>;

  // Matrix
  using Matrix = typename std::decay_t<MS>::value_type;

  // Model
  using Model = std::multimap<i64,std::pair<Layer,Matrix>>;

  // }}}

  // Get all the graph layers
  std::vector<Layer> layers
  {
    fw::apply(fp::numbers(size_t{},depth)
      , fw::transform([&](auto i){ return f_layer(i); })
    )
  };

  assertm(layers.size() == depth, "Number of layers does not match depth");

  // Create the model
  Model model;

  //
  // @ Key idea: Initially, all rows are paired with their layers
  // except for the last layer, which will be used for the last
  // column, 'zip' takes care of this
  //

  assertm(layers.size() == mr.size()+1, "Degenerate layers or matrices");

  // Populate the model with layers and matrices
  for (auto&& e : fp::enumerate(fp::zip(layers,mr))) { model.emplace(e); }

  assertm( layers.size() == model.size()+1, "Degenerate model");

  // mrc = all matrices reversed, for column barycenter
  MS mrc {fp::transform([&](auto&& m){ return swap_rc(m); },mr)};

  assertm( mr.size() == mrc.size(), "Loss in reverse matrix fn");

  // Layers for columns start at the second position
  layers.erase(layers.begin());

  // Populate the model with layers and matrices
  for (auto&& e : fp::enumerate(fp::zip(layers,mrc))) { model.emplace(e); }

  assertm( model.size() == (mr.size()*2), "Degenerate reverse model");

  // No need to use layers anymore, clear to save memory
  layers.clear();

  // Keep the best solutions
  Model best{model};

  assertm( model.size() == best.size(), "Failed to copy model");

  // Phase 1 {{{
  auto phase_1 = [](auto& _model, auto& _best, auto& _itm, auto& _itb)
  {
    // Current layers and matrix
    auto [l,m] = std::make_pair( _itm->second.first, _itm->second.second);

    // Compute the barycenter of current matrix
    auto bs {fw::apply(m
      , fw::transform([&](auto&& v){ return barycenter::run(v); })
    )};

    // Order the layers and matrices with respect to the barycenter
    auto ls{fw::apply(l,fw::zip(bs),fw::sort(),fw::unzip(),fw::snd())};
    auto ms{fw::apply(m,fw::zip(bs),fw::sort(),fw::unzip(),fw::snd())};

    // Compute the number of crossings for the current and best sorted matrices
    auto cost_m_prev{count_crossings::run(m)};
    auto cost_m_sort{count_crossings::run(ms)};

    // Early return on same
    // Sorting by barycenter always improves cost,
    if (cost_m_prev == cost_m_sort)
    {
      return;
    } // if cost_m_sort == cost_m_prev

    fmt::print("cost_prev: {}\n", cost_m_prev);
    fmt::print("cost_sort: {}\n\n", cost_m_sort);

    // Helpers to update models
    auto update_model =
      [](auto&& __it, auto&& __sl, auto&& __bs) -> void
    {
      // Update layer
      __it->second.first = __sl;
      // Update matrix
      __it->second.second = fw::apply(__it->second.second // std::pair<Layer,Matrix>
        ,fw::zip(__bs) // std::pair<bs_t,std::pair<Layer,Matrix>>
        ,fw::sort()    // Sorted by bs_t
        ,fw::unzip()
        ,fw::snd()     // std::pair<Layer,Matrix>
      );
    };

    //
    // @ if: The first and last columns have no associated rows/columns
    // that need to be updated, therefore, they can be processed separately.
    //
    // @ else: If the matrix is a column matrix, then the next matrix is a row
    // matrix, and it needs to be updated for a consistent state
    //
    // If this is the first or last element
    if ( (_itm == _model.begin()) || (std::next(_itm) == _model.end()) )
    {
      update_model(_itm, ls, bs);
      // Update current best if cost is less than
      if (cost_m_sort < count_crossings::run(_itb->second.second))
      {
        update_model(_itb, ls, bs);
      } // if cost_m_sort < count_crossings::run(_itb->second)

      // Check if model was not currupted
      assertm(_model.size() == _best.size(),"Degenerated models after update");
      assertm(std::distance(_itm,_model.end()) == std::distance(_itb,_best.end()),
          "Unaligned iterators"
      );

    } //  (_itm == _model.begin()) || (std::distance(_itm,_model.end()) == 1)
    else
    {
      update_model(_itm, ls, bs);
      // Update current best if cost is less than
      if (cost_m_sort < count_crossings::run(_itb->second.second))
      {
        update_model(_itb, ls, bs);
      } // if cost_m_sort < count_crossings::run(_itb->second.second)

      // Check if model was not currupted
      assertm(_model.size() == _best.size(),"Degenerated models after update");
      assertm(std::distance(_itm,_model.end()) == std::distance(_itb,_best.end()),
          "Unaligned iterators"
      );

      // Perform
      if ( (std::distance(_model.begin(), _itm) % 2 ) != 0 )
      {
        update_model(std::next(_itm),ls,bs);
      } // if std::distance(_model.begin(), _itm)%2 != 0
    } // else
  }; // lamb: phase_1

  // Phase 2 {{{
  auto phase_2 = [&phase_1](auto& _model, auto& _best)
  {
    assertm(_model.size() == _best.size(),"Phase 1 degenerated models");

    // Check if any rows or cols are not in a barycentric ordered manner
    auto is_ordered = [](auto& __model) -> bool
    {
      return fw::apply(__model
        , fw::get_map_values() // std::vector<std::pair<Layer,Matrix>>
        , fw::transform([](auto&& e){ return e.second; })
        , fw::transform([](auto&& m){
            return fp::transform([](auto&& v){ return barycenter::run(v); },m);
          })
        , fw::all_by([](auto&& bs){ return fp::is_sorted(bs); })
      );
    };

    while (! is_ordered(_model))
    {
      // @ itm: Iterator for the current model
      // @ itb: Iterator for the best model
      // @ best model is the one which holds the best incidence matrices
      // @ itm,itb: Starts in a layer of a row, therefore, each iteration
      // intersperse between layers of rows and columns, with this observation,
      // it is possible to know if an iterator is of a row or col layer, with
      // the usage of the modulus operator, e.g:
      // ( (std::distance( model.begin(),itm) % 2) != 0 ),
      // when the condition is true, it is always a column.
      for (auto&& itm{_model.begin()}, itb{_best.begin()};
          itm != _model.end();
          ++itm, ++itb
      )
      {
        // Carry out phase 1
        phase_1(_model,_best,itm,itb);
        // Reorder only current matrix, best should be preserved
        reorder(itm->second.second);
        // Currupted models
        assertm(_model.size() == _best.size(),"Phase 1 degenerated models");
        // Unaligned iterators
        assertm(std::distance(itm,_model.end()) == std::distance(itb,_best.end()),
            "Unaligned iterators"
        );

      } // for

    } // while: ! is_ordered(_model)

  };
  // }}}

  phase_2(model,best);

  return fw::apply(best
    , fw::get_map_values() // std::vector<std::pair<Layer,Matrix>>
    , fw::transform([](auto&& e){ return e.first; }) // std::vector<Layer>
  );

} // function: run

// }}}

} // namespace celaeno::graph::operations::minimize::crossing::impl }}}
