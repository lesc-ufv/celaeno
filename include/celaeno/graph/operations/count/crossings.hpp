// vim: set expandtab fdm=marker ts=2 sw=2 tw=100 et :
//
// @author      : Ruan E. Formigoni (ruanformigoni@gmail.com)
// @file        : crossings
// @created     : sábado ago 15, 2020 16:30:36 -03
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

#include <range/v3/all.hpp>
#include <fplus/fplus.hpp>
#ifndef NDEBUG
  #include <spdlog/spdlog.h>
#endif

#include <celaeno/concepts.hpp>
#include <celaeno/aliases.hpp>

// namespace celaeno::graph::operations::count::crossings {{{
namespace celaeno::graph::operations::count::crossings
{

// Macros {{{
#define assertm(exp, msg) assert(((void)msg, exp))
// }}}

// namespaces {{{
namespace rg = ranges;
namespace fp = fplus;
namespace fw = fplus::fwd;
// }}}

// Using namespaces {{{
using namespace celaeno::concepts;
// }}}

// function: run {{{
template<SignedIntegral T = i64, Range R, typename S>
T run(R&& l1, R&& l2, S&& f_succ)
  requires CallableWith<S,i64>
{

#ifndef NDEBUG
  spdlog::set_level(spdlog::level::debug);
  spdlog::debug("Algorithm: celaeno::graph::operations::count::crossings");
#endif

  //
  // @ Index by vertices positions in layer l2
  //
  std::map<T,T> ids;
  rg::for_each(fp::numbers({},l2.size()),[&](T n) mutable { ids.emplace(l2.at(n),n); });

  //
  // @ Create vector of id ordered successors, for all vertices of l2
  //
  std::vector<std::vector<T>> succs;

  std::ranges::for_each(l1,
  [&](auto u)
  {
    // Get successors of u, sort and transform in ids
    succs.emplace_back(fw::apply(f_succ(u)
      , fw::sort_by([&](auto a, auto b){ return ids.at(a) < ids.at(b); })
      , fw::transform([&](auto v){ return static_cast<T>(ids.at(v)); })
    ));
  });

  //
  // @ For each level 1..n, merge with level 0, to compute crossings
  //

  // Test for empty successors vec
  assertm( !succs.empty() , "Empty successors map!");

  // Keep number of crossings
  i64 crossings{};

  // First vector accumulates all values
  auto& acc{*succs.begin()};

  for (auto it1{std::next(succs.begin())}; it1 != succs.end(); ++it1)
  {
    // For each value of current vector
    for (auto it2{it1->begin()}; it2 != it1->end(); ++it2)
    {
      // Find a position in the accumulator
      auto search{ std::ranges::find_if(acc,[&](auto e){ return e > *it2; }) };
      // If pos != end, then the number of crossings equals the distance of
      // end - current position
      if ( search != std::ranges::end(acc) )
      {
        // Insert element
        // Include the distance from inserted position in accumulator last elem
        crossings += std::distance(acc.insert(search,*it2),std::prev(acc.end()));
      } // if
      else
      {
        // Insert at the end
        acc.push_back(*it2);
      } // else
    } // for
  } // for

  return crossings;

} // function: run }}}

} // celaeno::graph::operations::count::crossings }}}
