// vim: set expandtab fdm=marker ts=2 sw=2 tw=100 et :
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

#include <map>
#include <fplus/fplus.hpp>
#include <celaeno/graph/kahn.hpp>
#include <type_traits>

// namespace celaeno::graph::views::depth {{{
namespace celaeno::graph::views::depth
{

// Namespaces {{{
namespace fp = fplus;
namespace fw = fplus::fwd;
namespace kahn = celaeno::graph::kahn;
// }}}

// Concepts {{{
template<typename T>
concept Iterable = requires{ std::input_iterator<T> && std::incrementable<T>; };
template<typename T>
concept Function = requires(T t) { {t(int64_t{})} -> Iterable; };
// }}}

// Algorithm {{{
template<std::signed_integral T, Function F1, Function F2>
std::pair<std::multimap<T,T>,std::map<T,T>>
  run(T root, F1&& pred, F2&& succ)
{
  // level -> nodes
  std::multimap<T,T> ln;

  // node -> level
  std::map<T,T> nl;

  // Emplace in ln and nl
  auto emplace = [&](auto&& l, auto&& n) { ln.emplace(l,n); nl.emplace(n,l); };

  // Perform Topological sorting
  auto topo {kahn::run(std::forward<T>(root),std::forward<F1>(pred),std::forward<F2>(succ))};

  // lambda to get all levels of a set vertices
  auto levels = [&](auto&& vs) { return fp::transform([&](auto&& n){ return nl.at(n); }, vs);};

  // lambda to get the predecessor ps with max level
  auto max = [&](auto&& ps) { return fw::apply(levels(ps),fw::maximum()); };

  // Split by level
  fw::apply(topo,fw::transform(([&](auto&& n)
  {
    auto preds {pred(n)};
    // If is in the first level (has no predecessors), emplace 0
    if( preds.size() == 0 ) [[unlikely]] { emplace(0,n); }
    // else, emplace max level of the predecessors + 1
    else [[likely]] { emplace(max(preds)+1,n); }
    return true;
  })));

  return { ln, nl };
} // function: run
// }}}

} // namespace celaeno::graph::view::depth

// }}}
