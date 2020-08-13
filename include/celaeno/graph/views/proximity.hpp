// vim: set expandtab fdm=marker ts=2 sw=2 tw=100 et :
//
// @author      : Ruan E. Formigoni (ruanformigoni@gmail.com)
// @file        : proximity
// @created     : sexta jul 31, 2020 02:23:41 -03
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

#include <concepts>
#include <utility>
#include <type_traits>
#include <range/v3/all.hpp>
#include <fplus/fplus.hpp>
#include <celaeno/graph/kahn.hpp>
#include <celaeno/graph/views/depth.hpp>
#include <cmath>

// namespace celaeno::graph::views::proximity {{{

namespace celaeno::graph::views::proximity
{

// Namespaces {{{
namespace depth = celaeno::graph::views::depth;
namespace rg = ranges;
namespace rv = ranges::views;
namespace ra = ranges::actions;
namespace fp = fplus;
namespace fw = fplus::fwd;
namespace kahn = celaeno::graph::kahn;
// }}}

// Concepts {{{
template<typename T>
concept SignedIntegral = std::signed_integral<T>;

template<typename T>
concept Iterable = requires{ std::input_iterator<T> && std::incrementable<T>; };

template<typename T>
concept Neighbors = requires(T t) { {t(int64_t{})} -> Iterable; };
// }}}

// fn: run {{{

template<SignedIntegral T, Neighbors P, Neighbors S>
auto run(T root, P&& pred, S&& succ) -> std::pair<std::multimap<T,T>,std::map<T,T>>
{
  // Create a depth-view
  auto [dv,vd] {depth::run(root, std::forward<P>(pred), std::forward<S>(succ))};

  // Perform Topological sorting
  auto topo {kahn::run(std::forward<T>(root),std::forward<P>(pred),std::forward<S>(succ))};

  // Reverse topo view
  topo |= ra::reverse;

  for (auto&& t : topo)
  {
    auto preds{pred(t)};
    // is not lev(0)
    if( ! preds.empty() )
    {
      for (auto&& p : preds)
      {
        // Predecessor has an inter edge
        if( (vd.at(t) - vd.at(p)) > 1 )
        {
          // Get successors
          auto succs{succ(p)};
          // If successors are empty, continue
          if( succs.empty() ) continue;
          // Order successors in ascending order of lev(s)
          auto levs {fw::apply(succs, fw::transform([&](auto&& s){ return vd.at(s); }),fw::sort())};
          // Get the difference of lev(min(s)) and lev(p)
          auto diff { levs.at(0) - vd.at(p) };
          // If the diff is gt than one, update lev(p)
          if( diff > 1) vd.at(p) = levs.at(0)-1;
        }
      } // for p : preds
    }
  } // for t : topo

  dv.clear();
  for (auto&& [v,d] : vd) { dv.emplace(d,v); } // for [v,d] : vd

  return { dv, vd };

} // function: run }}}

} // namespace celaeno::graph::views::proximity }}}
