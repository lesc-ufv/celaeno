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

#include <queue>
#include <set>
#include <vector>
#include <fplus/fplus.hpp>
#include <range/v3/all.hpp>
#include <concepts>

namespace celaeno::graph::a_star
{

//
// Aliases
//

namespace rg = ranges;
namespace rv = ranges::views;
namespace ra = ranges::action;
namespace fp = fplus;
namespace fw = fplus::fwd;
using float64_t = double;

//
// Concepts
//
template<typename T>
concept SignedInt = std::signed_integral<T>;

template<typename T>
concept BaseType =
requires(T t)
{
  { t.first  } -> std::convertible_to<int64_t>;
  { t.second } -> std::convertible_to<int64_t>;
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
  using Base = std::conditional_t<
    std::is_integral_v<T>, int64_t, std::pair<int64_t,int64_t>
  >;

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

template<BaseType T, typename F1, typename F2, typename F3>
decltype(auto) a_star(T&& start, T&& end, F1&& f_neighbors, F2&& f_distance, F3&& f_heuristic)
{
  using Base = std::conditional_t<std::is_integral_v<T>,
    int64_t, std::pair<int64_t,int64_t>
  >;

  // Open set in ascending order
  std::multimap<float64_t,Base> open;

  // Closed set
  std::set<Base> closed;

  // G-Score
  std::map<Base,float64_t> g_score;

  // Paths memory
  std::map<Base,Base> mem;

  // Insert initial vertex
  open.emplace(f_heuristic(start), start);
  g_score.emplace(start, 0.);

  // keep the previous vertex for final path
  Base prev{start};

  // Main loop
  while( ! open.empty() )
  {

    // h == heuristic cost value
    // id = vertex id
    auto [h, id]  { *open.cbegin()  }; open.erase(open.cbegin());

    // Update memory
    if( id != start ) mem.emplace(prev,id);

    // If it is the goal, rebuild the path and return
    if ( id == end ) return rebuild_path(mem,end);

    // Insert the vertex into the closed set
    closed.insert(id);

    // Update previous vertex
    prev = id;

    // For each neighbor of current vertex
    for (auto&& n : f_neighbors(id))
    {
      // Do not explore vertices in the closed set
      if( closed.contains(n) ) continue;

      // Analyse the cost to goal
      auto ng {g_score.at(id)+f_distance(n)};

      // If the cost is infinite (there is not other path) or
      // If the cost is better than an existing one
      // Update the score
      if ( ! g_score.contains(n) || ng < g_score.at(n) )
      {
        // Update g_score
        g_score.emplace(n,ng);
        // Update f_score
        open.emplace(ng+f_heuristic(n),n);
      } // if
    } // for f_neighbors(id)
  } // while ! open.empty()

  return std::deque<Base>{};
}

} // namespace celaeno::graph::a_star
