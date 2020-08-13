// vim: set expandtab fdm=marker ts=2 sw=2 tw=100 et :

//
// @author      : Ruan E. Formigoni (ruanformigoni@gmail.com)
// @file        : minimize-crossings
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
#include <fplus/fplus.hpp>
#include <range/v3/all.hpp>
#include <celaeno/aliases.hpp>
#include <celaeno/concepts.hpp>
#include <celaeno/graph/barycenter.hpp>
#include <celaeno/graph/crossings.hpp>

// namespace celaeno::graph::minimize_crossings {{{
namespace celaeno::graph::minimize_crossings
{

// namespaces {{{

namespace cp = celaeno::concepts;
namespace fp = fplus;
namespace fw = fplus::fwd;
namespace rg = ranges;
namespace barycenter = celaeno::graph::barycenter;
namespace crossings = celaeno::graph::crossings;
template<typename T> using ref = std::reference_wrapper<T>;

// }}}

// Concepts {{{

template<typename T>
concept Layer =
requires(T t)
{
  { t(i64{}) } -> cp::Iterable;
};

// }}}

// Helpers {{{

template<cp::Matrix M>
auto swap_rows_and_cols(M&& m)
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

// @mr  → Matrix realization
// @layer → lambda to obtain the nodes a graph layer
// @depth → depth of the topologically sorted graph
template<cp::Matrices MS, Layer L>
auto run(MS&& mr, L&& layer, size_t depth)
{

  // Aliases {{{

  // Graph layer
  using Layer = typename std::decay_t<decltype(layer(0))>;

  // Graph ref
  using LayerRef = ref<Layer>;

  // Matrix
  using Matrix = typename std::decay_t<MS>::value_type;

  // Return value
  using Ret = std::pair<std::pair<LayerRef,LayerRef>,Matrix>;

  // }}}

  // Variables {{{

  // Get all the graph layers
  std::vector<Layer> layers
  {
    fw::apply(fp::numbers(size_t{},depth),fw::transform([&](auto i){ return layer(i); }))
  };

  // Make a vector of references for the layers
  std::vector<LayerRef> layers_ref;
  rg::for_each(layers,[&](auto& l){ layers_ref.emplace_back(ref(l)); });

  // Zip layers and matrices, with two overlapping layers for each matrix
  std::vector<Ret> ret{fp::zip(fp::overlapping_pairs(layers_ref),mr)};

  // }}}

// Helpers {{{

  // Sort by barycenter {{{
  auto sort_by_barycenters =
  [](auto const& l, auto const& m, auto const& bs) -> auto
  {
    return std::make_pair(
      fw::apply(l,fw::zip(bs),fw::sort(),fw::unzip(),fw::snd()),
      fw::apply(m,fw::zip(bs),fw::sort(),fw::unzip(),fw::snd())
    );
  }; // lamb: sort_by_barycenters
  // }}}

  // Reorder adjacent equal barycenters {{{
  std::set<std::pair<i64,i64>> reordered;
  auto reorder_equal_barycenters = [&](auto const& l, auto const& m, auto const& bs) -> auto
  {
    typename std::decay_t<decltype(l)> new_l{l};
    typename std::decay_t<decltype(m)> new_m{m};
    // Check if any adjacent barycenters are equal
    for (decltype(bs.size()) i{}; i < bs.size()-1; ++i)
    {
      if (bs.at(i) == bs.at(i+1))
      {
        // Avoid repeatedly switching the same vertices
        if (reordered.contains({new_l.at(i),new_l.at(i+1)}))
        {
          continue;
        } // if reordered.contains({new_l.at(i),new_l.at(i+1)})
        else
        {
          reordered.insert({new_l.at(i+1),new_l.at(i)});
        } // else

        // Switch the nodes
        new_l.at(i+1) = std::exchange(new_l.at(i),new_l.at(i+1));
        // Switch the rows
        new_m.at(i+1) = std::exchange(new_m.at(i),new_m.at(i+1));
      } // if bs.at(i) == bs.at(i+1)
    } // for: i < bs.size()
    return std::make_pair(l,m);
  }; // lamb reorder_equal_barycenters }}}

  // Accumulated cost of all matrices {{{

  auto f_cost = [&]() -> i64
  {
    i64 cost{};
    for (auto& [l,m] : ret)
    {
      cost += crossings::run(m);
    } // for [l,m] : ret
    return cost;
  };

  // }}}

// }}}


  i64 min_cost{f_cost()};
  i64 tries{0};

  while (true)
  {
    // Step 1: Sort by barycenter {{{
    auto step_1 = [&]() -> void
    {
      for (auto it{ret.begin()}; it != ret.end(); ++it)
      {
        // -- CASE1: The first incidence matrix
        if (it == ret.begin())
        {
          // Current row cr; current matrix cm
          auto& cr {it->first.first.get()};
          auto& cm {it->second};
          auto bs {fw::apply(cm,fw::transform([&](auto&& v){return barycenter::run(v);}))};
          // Check if barycenter sorting reduces the number of crossings
          auto result {sort_by_barycenters(cr,cm,bs)};
          cr = result.first;
          cm = result.second;
          continue;
        } // if it == ret.begin()

        // -- CASE2: Previous matrix exists
        if (it != ret.begin())
        {
          // -- SUBCASE1: Get the prev. matrix and its column nodes
          auto& pc {(it-1)->first.second.get()};
          auto& pm {(it-1)->second};
          pm = swap_rows_and_cols(pm);

          // -- SUBCASE2: Get the curr. matrix and its row nodes
          auto& cr {it->first.first.get()};
          auto& cm {it->second};

          // -- SUBCASE3: Get the barycenter ordering for either matrices
          auto bs {fw::apply(cm,fw::transform([&](auto&& v){return barycenter::run(v);}))};

          // -- SUBCASE4: Sort the columns/rows  and matrices entries by barycenter
          auto prev {sort_by_barycenters(pc,pm,bs)};
          auto curr {sort_by_barycenters(cr,cm,bs)};

          // -- SUBCASE5: Assign new values sorted by barycenter
          if (
            crossings::run(prev.second) <= crossings::run(pm) &&
            crossings::run(curr.second) <= crossings::run(cm)
          )
          {
            pc = prev.first;
            cr = curr.first;
            pm = prev.second;
            cm = curr.second;
          } // if crossings::run(prev.second) <= crossings::run(pm)

          pm = swap_rows_and_cols(pm);

        } // if it != ret.begin()

        // -- CASE3: It is the last matrix
        if (it == (ret.end()-1))
        {
          // -- SUBCASE1: Reorder by column barycenters
          auto& cc {it->first.second.get()};
          auto& cm {it->second};
          // Swap rows and cols to reorder
          cm = swap_rows_and_cols(cm);
          // Create the barycenter vector
          auto bs {fw::apply(cm,fw::transform([&](auto&& v){return barycenter::run(v);}))};
          // Check if barycenter sorting reduces the number of crossings
          auto result {sort_by_barycenters(cc,cm,bs)};
          if (crossings::run(result.second) <= crossings::run(cm))
          {
            cc = result.first;
            cm = result.second;
          } // if crossings::run(result.second) <= crossings::run(cm)
          // Swap back
          cm = swap_rows_and_cols(cm);
        } // if it == (ret.end()-1)
      } // for: it != ret.end()
    }; // lamb: step_1 }}}

    // Step 2: Reorder rows {{{
    auto step_2 = [&]() -> void
    {
      for (auto it{ret.begin()}; it != ret.end(); ++it)
      {
        // Current row cr; current matrix cm
        auto cr { it->first.first.get() };
        auto cm { it->second };
        auto bs {fw::apply(cm,fw::transform([&](auto&& v){return barycenter::run(v);}))};
        // Reorder
        auto result {reorder_equal_barycenters(cr,cm,bs)};
        // Assign new result
        cr = result.first;
        cm = result.second;
      } // for: it != ret.end()
    }; // lamb: Step 2 }}}

    // Step 3: Reorder cols {{{
    auto step_3 = [&]() -> void
    {
      for (auto it{ret.begin()}; it != ret.end(); ++it)
      {
        auto cc { it->first.second.get() };
        auto cm { it->second };
        // Swap rows and cols to reorder
        cm = swap_rows_and_cols(cm);
        auto bs {fw::apply(cm,fw::transform([&](auto&& v){return barycenter::run(v);}))};
        // Reorder
        auto result {reorder_equal_barycenters(cc,cm,bs)};
        cc = result.first;
        cm = result.second;
        // Swap back
        cm = swap_rows_and_cols(cm);
      } // for: it != ret.end()
    }; // lamb: Step 3 }}}

    // Pipeline {{{
    // Sort rows and cols by barycenter
    step_1();
    // Rearrange cols with the same barycenter
    step_3();
    // // Sort rows and cols by barycenter
    step_1();
    // // Rearrange rows with the same barycenter
    step_2();
    // // Sort rows and cols by barycenter
    step_1();
    // }}}

    // Stop Conditions {{{

    // -- CASE1: Stop if the cost has not reduced n times
    i64 new_cost{f_cost()};

    if (new_cost < min_cost)
    {
      min_cost = new_cost;
      tries = 0;
    } // if new_cost < min_cost
    else
    {
      ++tries;
    } // else

    if( tries >= 100 ) { break; }

    // }}}

  } // while: true

  return std::make_pair(layers,fw::apply(ret,fw::unzip()).second);

} // function: run

// }}}

} // namespace celaeno::graph::minimize_crossings }}}
