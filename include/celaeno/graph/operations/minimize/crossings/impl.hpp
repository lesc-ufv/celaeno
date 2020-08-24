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
  size_t sz{};
  try{ sz = m.at(0).size(); }
  catch(std::exception const& e) { std::cerr << "Empty matrix" << std::endl; }

  // Return new container with columns as rows
  return fw::apply(fp::numbers(size_t{},sz),fw::transform([&](auto i){return col(i);}));

} // function: swap_rows_and_cols

// }}}

// Algorithm {{{

// template<Matrix Model>
// void phase_1(M& best, Model const& model)
// {
//   for (auto&& [l,m] : model)
//   {
//
//   } // for [l,m] : model
// } // function: phase_1

// @mr  → Matrix realization
// @layer → lambda to obtain the nodes a graph layer
// @depth → depth of the topologically sorted graph
template<Matrices MS, Layer L>
auto run(MS&& mr, L&& f_layer, size_t depth)
{

  // Aliases {{{

  // Graph layer
  using Layer = typename std::decay_t<decltype(f_layer(0))>;

  // Matrix
  using Matrix = typename std::decay_t<MS>::value_type;

  // Model
  using Model = std::multimap<Layer,Matrix>;

  // }}}

  // Get all the graph layers
  std::vector<Layer> layers
  {
    fw::apply(fp::numbers(size_t{},depth),fw::transform([&](auto i){ return f_layer(i); }))
  };

  // Create the model
  Model model;

  // Populate the model with layers and matrices
  for (auto&& e : fp::zip(layers,mr)) { model.emplace(e); }

  // mrc = all matrices reversed, for column barycenter
  MS mrc {fp::transform([&](auto&& m){ return swap_rc(m); },mr)};

  // Layers for columns start at the second position
  layers.erase(layers.begin());

  // Populate the model with layers and matrices
  for (auto&& e : fp::zip(layers,mrc)) { model.emplace(e); }

  // No need to use layers anymore, clear to save memory
  layers.clear();

  // Keep the best solutions
  Model best{model};

  // @ itm: Iterator for the current model
  // @ itb: Iterator for the best model
  // @ best model is the one which holds the best incidence matrices
  // @ itm,itb: Starts in a layer of a row, therefore, each iteration
  // intersperse between layers of rows and columns, with this observation, it
  // is possible to know if an iterator is of a row or col layer, with the usage
  // of the modulus operator, e.g, ( (std::distance(model.begin(), itm)%2)!=0 ),
  // when the condition is true, it is always a column.
  for (auto&& itm{model.begin()}, itb{best.begin()};
      itm != model.end();
      ++itm, ++itb
  )
  {
    // Current and best models pairs of layers and matrices
    auto [lm,mm] = std::make_pair( itm->first, itm->second);
    auto [lb,mb] = std::make_pair( itb->first, itb->second);

    // Compute the current and best matrices
    auto bsm {fw::apply(mm
      , fw::transform([&](auto&& v){ return barycenter::run(v); })
    )};

    auto bsb {fw::apply(mb
      , fw::transform([&](auto&& v){ return barycenter::run(v); })
    )};

    // Order the layers with respect to the barycenter
    auto slm{fw::apply(lm,fw::zip(bsm),fw::sort(),fw::unzip(),fw::snd())};
    auto slb{fw::apply(lb,fw::zip(bsb),fw::sort(),fw::unzip(),fw::snd())};

    // Order the current and matrices with respect to barycenter
    auto smm{fw::apply(mm,fw::zip(bsm),fw::sort(),fw::unzip(),fw::snd())};
    auto smb{fw::apply(mb,fw::zip(bsb),fw::sort(),fw::unzip(),fw::snd())};

    // Compute the number of crossings for the current and best sorted matrices
    auto cost_smm{count_crossings::impl::run(smm)};
    auto cost_smb{count_crossings::impl::run(smb)};

    // Helper to update models
    auto update_model = [](auto& m, auto&& it, auto&& sl, auto&& sm) -> void
    {
      // Extract current multimap node
      auto handle{m.extract(it)};
      // Update layers ordering
      handle.key() = sl;
      // Update matrix
      handle.mapped() = sm;
      // Reinstate node handle
      m.insert(std::move(handle));
    };

    // IMPORTANT Update current and next matrix and layers
    // IMPORTANT Update global and next matrix and layers
    // If this is the first or last element
    if ( (itm == model.begin()) || (std::distance(itm,model.end()) == 1) )
    {
      update_model(model, itm, slm, smm);
      // Update current best
      if (cost_smb <= cost_smm)
      {
        update_model(best, itb, slb, smb);
      } // if cost_smb < cost_smm
    } //  (itm == model.begin()) || (std::distance(itm,model.end()) == 1)
    else // TODO Create glossary to explain this
    {
      {
        update_model(model, itm, slm, smm);
        // Update current best
        if (cost_smb <= cost_smm)
        {
          update_model(best, itb, slb, smb);
        } // if cost_smb < cost_smm
      }
      // If current layer and next are equal (have same vertices)
      if ((std::distance(model.begin(), itm)%2) != 0)
      {
        auto update_next_model =
          [](auto& _model, auto&& _it, auto&& _sl, auto&& _bs)
        {
          // Get the next model
          auto next_model{std::next(_it)};
          // Get the handle for the element
          auto handle{_model.extract(next_model)};
          // Update model layers
          handle.key() = _sl;
          // Sort model matrix by current layers
          auto& mpd{handle.mapped()};
          mpd = fw::apply(mpd,fw::zip(_bs),fw::sort(),fw::unzip(),fw::snd());
          // Reinstate node handle
          _model.insert(std::move(handle));
        };
        // Update current and best models
        update_next_model(model,itm,slm,bsm);
        update_next_model(best,itb,slb,bsb);
      } // if std::distance(model.begin(), itm)%2 != 0
    } // else

  } // for: itm != model.end()

  return fp::get_map_keys(best);

} // function: run

// }}}

} // namespace celaeno::graph::operations::minimize::crossing::impl }}}
