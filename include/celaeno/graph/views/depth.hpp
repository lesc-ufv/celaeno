//
// @author      : Ruan E. Formigoni (ruanformigoni@gmail.com)
// @file        : depth
// @created     : Sunday Feb 23, 2020 18:35:30 -03
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

#include <set>
#include <map>
#include <range/v3/all.hpp>
#include <fplus/fplus.hpp>
#include <celaeno/graph/kahn.hpp>
#include <type_traits>

namespace celaeno::graph::views::depth
{
//
// Aliases
//
namespace fp = fplus;
namespace fw = fplus::fwd;
namespace rg = ranges;
namespace rv = ranges::views;
namespace kahn = celaeno::graph::kahn;

//
// Concepts
//

template<typename T>
concept Iterable = requires{ std::input_iterator<T> && std::incrementable<T>; };
template<typename T>
concept Function = requires(T t) { {t(int64_t{})} -> Iterable; };

//
// Algorithm
//

template<std::signed_integral T, Function F1, Function F2>
std::pair<std::multimap<T,T>,std::map<T,T>>
  depth(T root, F1&& pred, F2&& succ)
{
  // level -> nodes
  std::multimap<T,T> m;
  // node -> level
  std::map<T,T> m_rev;

  auto topo_sort
  {
    kahn::kahn(
      std::forward<T>(root),
      std::forward<F1>(pred),
      std::forward<F2>(succ),
      [&pred,&m,&m_rev](auto&& v) -> bool
      {
        if( pred(v).size() == 0 )
        {
          m.emplace(0,v);
          m_rev.emplace(v,0);
        } // if
        else
        {
          // Get predecessors
          // Get their levels
          // Get the max value
          auto level
          {
            fw::apply(
              pred(v)
              , fw::transform([&m_rev](auto&& v){ return m_rev.at(v); })
              , fw::maximum()
            )
          };
          m.emplace(level+1,v);
          m_rev.emplace(v,level+1);
        } // else
        return false;
      } // lambda
    ) // kahn's algorithm
  };

  return std::make_pair(m,m_rev);
} // depth_view

} // namespace celaeno::graph::view::depth
