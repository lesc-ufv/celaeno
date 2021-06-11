/* vim: set expandtab fdm=marker ts=2 sw=2 tw=80 et :*/
//
// @author      : Ruan E. Formigoni (ruanformigoni@gmail.com)
// @file        : a-star
// @created     : Thursday Feb 20, 2020 12:05:37 -03
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
#include <numeric>
#include <queue>
#include <set>
#include <vector>
#include <celaeno/aliases.hpp>
#include <celaeno/heuristics/manhattan.hpp>
#include <fplus/fplus.hpp>
#include <range/v3/all.hpp>

// TODO Remove
#include <fmt/core.h>
#include <fmt/ranges.h>

namespace celaeno::graph::search::a_star
{

// Namespaces {{{
namespace ns_heuristics = celaeno::heuristics;
// }}}

// Using Namespaces {{{
using namespace celaeno::aliases;
// }}}

//
// Aliases
//
namespace fp = fplus;

//
// Concepts
//
template<typename T>
concept SignedInt = std::signed_integral<T>;

template<typename T>
concept BaseType =
requires(T t)
{
  { t.first  } -> std::convertible_to<i64>;
  { t.second } -> std::convertible_to<i64>;
}
||
requires (T t)
{
  { t } -> std::signed_integral;
};

//
// Helpers
//

// Compare distinct pairs of integrals
template<SignedInt T1, SignedInt T2, SignedInt T3, SignedInt T4>
  requires
       (! std::same_as<T1,T3>)
    || (! std::same_as<T2,T4>)
    || (! std::same_as<T1,T4>)
    || (! std::same_as<T2,T3>)
bool operator==(std::pair<T1,T2> a, std::pair<T3,T4> b)
{
  return (a.first == b.first) && (a.second == b.second);
}

//
// Algorithm
//

template<typename Map, typename T>
auto rebuild_path(Map& m, T curr)
{
  using Base = std::conditional_t<std::is_integral_v<T>, i64, std::pair<i64,i64>>;

  m = fp::swap_keys_and_values(m);

  std::deque<Base> final_path;

  final_path.emplace_front(curr);

  while( m.contains( curr ) )
  {
    curr = m.at(curr);
    final_path.emplace_front(curr);
  }

  return final_path;
}

template<BaseType T, typename F1, typename F2>
decltype(auto) run(T start, T end, F1&& f_neighbors, F2&& f_constraints)
{
  using Base = std::conditional_t<std::is_integral_v<T>, i64, std::pair<i64,i64> >;

  auto f_heuristic = [&](std::pair<i64,i64> n){ return ns_heuristics::manhattan::run(n,end); };

  // Open set in ascending order
  std::multimap<f64,Base> open;

  // Closed set
  std::set<Base> closed;

  // Heuristic cost
  std::map<Base,f64> h_cost;

  // Distance cost
  std::map<Base,f64> d_cost;

  // Paths memory
  std::map<Base,Base> mem;

  // Initialize open set and costs
  open.emplace(f_heuristic(start), start);
  d_cost.emplace(start, 0);

  // keep the previous vertex for final path
  Base prev{start};

  // Main loop
  while( ! open.empty() )
  {

    // cost == combined heuristic and distance costs
    // c = current
    auto [cost, c]  { *open.cbegin()  }; open.erase(open.cbegin());

    // fmt::print("Combined cost of {}: {}\n", c, cost);

    // Update memory
    if( c != start ) mem.emplace(prev,c);

    // If it is the goal, rebuild the path and return
    if ( c == end ) return rebuild_path(mem,end);

    // Insert the vertex into the closed set
    closed.insert(c);

    // Update previous vertex
    prev = c;

    // For each neighbor of current vertex
    for (auto&& n : f_neighbors(c))
    {
      // Ignore constrained elements
      if( f_constraints(n) ){ continue; }

      // Do not explore vertices in the closed set
      if( closed.contains(n) ) continue;

      // Analyse the cost to goal
      auto n_cost {d_cost.at(c)+0};

      // If the cost is infinite (there is not other path) or
      // If the cost is better than an existing one
      // Update the score
      if ( ! d_cost.contains(n) || n_cost < d_cost.at(n) )
      {
        // Update cost
        d_cost.emplace(n,n_cost);
        // Update combined cost
        // fmt::print("Cost of {} to {}: {} + {}\n", n, end, n_cost, f_heuristic(n));
        open.emplace(n_cost+f_heuristic(n),n);
      } // if
    } // for f_neighbors(c)
  } // while ! open.empty()

  return std::deque<Base>{};
} // function: run

} // namespace celaeno::graph::search::a_star
