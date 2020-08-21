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

// template<Matrix M, typename Model>
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

  // Variables {{{

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

  // Keep the best solutions
  Model best{model};

  fmt::print("Before ------------\n");
  for (auto&& [l,m] : model)
  {
    std::vector tmp(l);
    fmt::print("------------\n");
    for (auto&& v : m)
    {
      std::cout << tmp.front() << " : " << rv::all(v) << std::endl;
      tmp |= ra::reverse; tmp.pop_back(); tmp |= ra::reverse;
    } // for v : m
    fmt::print("------------\n");
  } // for [l,m] : model


  // Conditionally update global best
  // Sequence row,col,row,col ... col
  for (auto&& it{model.begin()}; it != model.end(); ++it)
  {
    // Get the current matrix and layer
    auto [l,m] = std::make_pair( it->first, it->second);

    // Compute barycenters
    auto bs {fw::apply(m
      , fw::transform([&](auto&& v){ return barycenter::run(v); })
    )};

    // Order the layer by barycenter TODO This can be done after the
    // cost check of sm
    auto sl{fw::apply(l,fw::zip(bs),fw::sort(),fw::unzip(),fw::snd())};

    // Order the matrix by barycenter
    auto sm{fw::apply(m,fw::zip(bs),fw::sort(),fw::unzip(),fw::snd())};

    // Number of crossings before and after the ordering
    auto costb{count_crossings::impl::run(m)};
    auto costa{count_crossings::impl::run(sm)};

    // Continue to compute until cost stops decreasing
    while (true)
    {
      // Break if it stopped decreasing
      if( costa == costb ) { break; }
      else
      {
        sl = fw::apply(l,fw::zip(bs),fw::sort(),fw::unzip(),fw::snd());
        sm = fw::apply(m,fw::zip(bs),fw::sort(),fw::unzip(),fw::snd());
        costa = count_crossings::impl::run(sm);
        costb = count_crossings::impl::run(sm);
      } // else
    } // while: true

    // IMPORTANT Update current and next matrix and layers
    // IMPORTANT Update global and next matrix and layers
    // If this is the first or last element
    if ( (it == model.begin()) || (std::distance(it,model.end()) == 1) )
    {
      auto handle{model.extract(it)};
      handle.key() = sl;
      handle.mapped() = sm;
      model.insert(std::move(handle));
    } //  (it == model.begin()) || (std::distance(it,model.end()) == 1)
    else // TODO Create glossary to explain this
    {
      // Update current layer and matrix
      {
        // Extract current multimap node
        auto handle{model.extract(it)};
        // Update matrix layers
        handle.key() = sl;
        // Update matrix
        handle.mapped() = sm;
        // Reinstate node handle
        model.insert(std::move(handle));
      }
      // If current layer and next are equal (have same vertices), update
      if (std::distance(model.begin(), it)%2 != 0)
      {
        // Get next element
        auto handle{model.extract(std::next(it))};
        // Update next matrix layers
        handle.key() = sl;
        // Sort next matrix by current layers
        handle.mapped() = fw::apply(handle.mapped()
          , fw::zip(bs)
          , fw::sort()
          , fw::unzip()
          , fw::snd()
        );
        // Reinstate node handle
        model.insert(std::move(handle));
      } // if std::distance(model.begin(), it)%2 != 0
    } // else

  } // for: it != model.end()


  i64 crossings{};
  fmt::print("After ------------\n");
  for (auto&& [l,m] : model)
  {
    crossings += count_crossings::run(m);
    std::vector tmp(l);
    fmt::print("------------\n");
    for (auto&& v : m)
    {
      std::cout << tmp.front() << " : " << rv::all(v) << std::endl;
      tmp |= ra::reverse; tmp.pop_back(); tmp |= ra::reverse;
    } // for v : m
    fmt::print("------------\n");
  } // for [l,m] : model

  fmt::print("Crossings: {}\n", crossings);

  //
  // // }}}
  // fmt::print("Before ------------\n");
  // for (auto&& [l,m] : model)
  // {
  //   std::vector tmp(l);
  //   fmt::print("------------\n");
  //   for (auto&& v : m)
  //   {
  //     std::cout << tmp.front() << " : " << rv::all(v) << std::endl;
  //     tmp |= ra::reverse; tmp.pop_back(); tmp |= ra::reverse;
  //   } // for v : m
  //   fmt::print("------------\n");
  // } // for [l,m] : model
  //
  // auto prev_layer{layers.at(0)};
  // for (auto& [l,m] : model)
  // {
  //   fmt::print("Level: {}\n",l);
  //
  //   // Calculate the barycenters
  //   auto bs {fw::apply(m
  //     , fw::transform([&](auto&& u){ return  barycenter::run(u); })
  //   )};
  //
  //   l = fw::apply(l,fw::zip(bs),fw::sort(),fw::unzip(),fw::snd());
  //   m = fw::apply(m,fw::zip(bs),fw::sort(),fw::unzip(),fw::snd());
  //
  //   // if (prev_layer != l)
  //   // {
  //   //
  //   // } // if prev_layer != l
  //   // fmt::print("Barycenter: {}\n",bs);
  //   // fmt::print("Sorted vts: {}\n",l_sorted);
  //   // for (auto&& v : m)
  //   // {
  //   // } // for v : m
  // } // for [l,m] : model
  //
  // fmt::print("After ------------\n");
  // for (auto&& [l,m] : model)
  // {
  //   std::vector tmp(l);
  //   fmt::print("------------\n");
  //   for (auto&& v : m)
  //   {
  //     std::cout << tmp.front() << " : " << rv::all(v) << std::endl;
  //     tmp |= ra::reverse; tmp.pop_back(); tmp |= ra::reverse;
  //   } // for v : m
  //   fmt::print("------------\n");
  // } // for [l,m] : model

  return layers;

} // function: run

// }}}

} // namespace celaeno::graph::operations::minimize::crossing::impl }}}
