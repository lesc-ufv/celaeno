// vim: set expandtab fdm=marker ts=2 sw=2 tw=80 et :
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

#include <ranges>
#include <utility>
#include <range/v3/all.hpp>
#include <celaeno/concepts.hpp>
#include <celaeno/graph/concepts.hpp>
#include <celaeno/graph/search/kahn.hpp>
#include <celaeno/graph/views/depth.hpp>

// namespace celaeno::graph::views::proximity {{{

namespace celaeno::graph::views::proximity
{

// Namespaces {{{
namespace rg = ranges;
namespace rv = ranges::views;
namespace depth = celaeno::graph::views::depth;
namespace kahn = celaeno::graph::search::kahn;
// }}}

// Using namespaces {{{
using namespace celaeno::concepts;
using namespace celaeno::graph::concepts;
// }}}

// using declarations {{{
using rg::for_each;
using rv::transform;
using rg::sort;
// }}}

// fn: run {{{

template<SignedIntegral T, typename P, typename S>
auto run(T root, P&& f_pred, S&& f_succ) -> std::pair<std::map<T,std::vector<T>>,std::map<T,T>>
{
  // Create a depth-view
  auto [lvs,vl] {depth::run(root, std::forward<P>(f_pred), std::forward<S>(f_succ))};

  // Perform Topological sorting
  auto topo {kahn::run(std::forward<T>(root),std::forward<P>(f_pred),std::forward<S>(f_succ))};

  // Reverse topo view
  std::reverse(topo.begin(),topo.end());

  for (auto&& t : topo)
  {
    auto preds{f_pred(t)};
    // is not lev(0)
    if( ! preds.empty() )
    {
      for (auto&& p : preds)
      {
        // Predecessor has an inter edge
        if( (vl.at(t) - vl.at(p)) > 1 )
        {
          // Get successors
          auto succs{f_succ(p)};
          // If successors are empty, continue
          if( succs.empty() ) continue;
          // Levels ordered by successors
          std::vector<T> levels;
          // Order successors in ascending order of lev(s)
          sort(succs,
            [vl=std::ref(vl)](auto&& lhs, auto&& rhs)
            {
              return vl.get().at(lhs) < vl.get().at(rhs);
            }
          );
          // Populate levels by sorted successors
          for_each(succs,
            [&,vl=std::ref(vl)](auto&& s)
            {
              levels.emplace_back(vl.get().at(s));
            }
          );
          // Get the difference of lev(min(s)) and lev(p)
          auto diff { levels.at(0) - vl.at(p) };
          // If the diff is gt than one, update lev(p)
          if( diff > 1) vl.at(p) = levels.at(0)-1;
        }
      } // for
    } // if
  } // for

  // Update level -> vertices
  for_each(vl, [lvs=std::ref(lvs)](auto&& e){ lvs.get().at(e.second).emplace_back(e.first); });

  return { lvs, vl };

} // function: run }}}

} // namespace celaeno::graph::views::proximity }}}
