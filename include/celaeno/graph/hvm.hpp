// vim: set ts=2 sw=2 tw=0 et :
//
// @company     : Universidade Federal de Viçosa - Florestal
// @author      : Ruan E. Formigoni (ruanformigoni@gmail.com)
// @file        : hwm
// @created     : Saturday Aug 17, 2019 17:17:23 -03
// @license     : MIT
// @description : C++ Algorithms Collection
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

#include <unordered_map>
#include <set>
#include <functional>
#include <range/v3/all.hpp>
#include <algorithm>

namespace celaeno::graph::hvm
{

//
// @params
// root -> The root vertex of the graph
// adjancent -> A lambda to obtain the neighbors of an
//              arbitrary node
//

template<typename T, typename F1>
std::unordered_map<T,T> hvm(T&& root, F1&& adjacent)
{

  //
  // Algorithm's steps:
  // sat -> A set of saturated vertices.
  // usat -> A set of unsaturated vertices.
  // edg -> A map that represents matched vertices.
  // 1. From a given vertex v1, obtain all the adjacent ones in a
  //    reference container usat that are not in sat.
  // 2. Get the vertices adjacent to v1
  //    2.1 If there is none, saturate v1, go to step 5.
  //    2.2 Get the one with maximum cardinality
  //    2.3 Remove the vertex from usat
  //    2.4 Pick and remove the vertex v2 with max cardinality from usat
  //    that is adjacent to v1.
  // 3. Insert v2 in sat
  // 4. Insert v1 and v2 in edg
  // 4. Insert in usat, all the neighbors of v2 that are not in sat.
  // 5. Pick a new vertex with the highest cardinality v1 from usat
  //    5.1 If usat is empty, end of algorithm.
  //    5.2 Remove the vertex from usat
  //    5.3 Include the vertex in sat
  //    5.4 Go back to step 1
  //
  using namespace ranges;

  std::set<T> sat;
  std::set<T> usat;
  std::unordered_map<T,T> edg;

  // Helper lambdas
  auto usat_ins = [&usat](auto v){ usat.insert(v); };
  auto not_in_sat = [&sat](auto v){ return sat.find(v) == sat.cend(); };
  auto card = [&adjacent](auto v){ return adjacent(v).size(); };
  auto max_card = [&card](auto v1, auto v2){ return (card(v1) < card(v2)); };


  // Algorithm
  auto v1 {root};

  do
  {
    // Step 1
    auto adj {adjacent(v1)};
    auto nis {adj | views::filter(not_in_sat)};
    ranges::for_each(nis, usat_ins);

    // Step 2
    // Get the vertices adjacent to v1
    auto adj_v1 { adj |
      views::filter([&usat](auto v){ return usat.contains(v); }) };

    if( ! adj_v1.empty() )
    {
      // 2.2 Get the one with maximum cardinality
      auto v2 {ranges::max_element(adj_v1, max_card)};
      // 2.3 Remove the vertex from usat
      usat.erase(*v2);
      // Step 3
      sat.insert(v1); sat.insert(*v2);
      edg.insert({v1,*v2});
      // Step 4
      adj = adjacent(*v2);
      nis = adj | views::filter(not_in_sat);
      ranges::for_each(nis, usat_ins);
    }
    else
    {
      // Step 2.1
      sat.insert(v1);
    }

    // Step 5
    auto v {ranges::max_element(usat, max_card)};
    // 5.1 If usat is empty, end of algorithm.
    if( v == ranges::end(usat) )
    {
      break;
    }
    else
    {
      // 5.2 Remove the vertex from usat
      usat.erase(*v);
      // 5.3 Include the vertex in sat
      sat.insert(*v);
      v1 = *v;
    }
  }
  while( true );

  return edg;
}

} // namespace celaeno::graph::hvm

//
// Notes
// https://www.youtube.com/watch?v=jtgBCGVux-8
//
