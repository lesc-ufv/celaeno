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
#include <optional>

#include <fplus/fplus.hpp>
#include <range/v3/all.hpp>

#include <celaeno/aliases.hpp>
#include <celaeno/concepts.hpp>
#include <celaeno/heuristics/manhattan.hpp>

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

// Concepts {{{
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
// }}}

// Helpers {{{

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

// }}}

// fn: rebuild_path {{{
template<typename T, typename Map>
std::deque<T> rebuild_path(T start, T end, Map const& m)
{
  std::deque<T> out;

  while( m.contains( end ) )
  {
    out.emplace_front(end);
    end = m.at(end);
  } // while

  out.emplace_front(start);

  return out;
} // fn: rebuild_path }}}

// fn: run {{{
template<typename T, typename F1, typename F2>
std::optional<std::deque<T>>
  run(T start, T end, F1&& f_neighbors, F2&& f_constraints)
{
  auto f_heuristic = [&](std::pair<i64,i64> n){ return ns_heuristics::manhattan::run(n,end); };

  // Open set in ascending order
  std::multimap<f64,T> open;

  // Closed set
  std::set<T> closed;

  // Heuristic cost
  std::map<T,f64> h_cost;

  // Distance cost
  std::map<T,f64> d_cost;

  // Paths memory
  std::map<T,T> mem;

  // Initialize open set and costs
  open.emplace(f_heuristic(start), start);
  d_cost.emplace(start, 0);

  // Save manhattan dist begin/end
  auto dist_be{ns_heuristics::manhattan::run(start,end)};

  // Main loop
  while( ! open.empty() )
  {

    // cost == combined heuristic and distance costs
    // c = current
    auto [cost, c]  { *open.cbegin()  }; open.erase(open.cbegin());

    // Stop algorithm if distance is 2 times greater than dist_be
    // TODO detect when algorithm is circlying around instead
    if( ns_heuristics::manhattan::run(start,c) > dist_be*4 ){ break; }

    // fmt::print("Combined cost of {}: {}\n", c, cost);

    // If it is the goal, rebuild the path and return
    if ( c == end ){ return rebuild_path(start,end,mem); }

    // Insert the vertex into the closed set
    closed.insert(c);

    // For each neighbor of current vertex
    for (auto&& n : f_neighbors(c))
    {
      // Ignore constrained elements
      if( f_constraints(n) && n != end ){ continue; }

      // Do not explore vertices in the closed set
      if( closed.contains(n) ){ continue; }

      // Analyse the cost to goal
      auto n_cost {d_cost.at(c)+0};

      // If the cost is infinite (there is not other path) or
      // If the cost is better than an existing one
      // Update the score
      if ( ! d_cost.contains(n) || n_cost < d_cost.at(n) )
      {
        // Save path c → n in path
        mem[n] = c;

        // Update cost
        d_cost.emplace(n,n_cost);

        // Update combined cost
        // fmt::print("-- Cost of {} to {}: {} + {}\n", n, end, n_cost, f_heuristic(n));
        open.emplace(n_cost+f_heuristic(n),n);
      } // if
    } // for
  } // while

  return std::nullopt;
} // fn: run }}}

} // namespace celaeno::graph::search::a_star
