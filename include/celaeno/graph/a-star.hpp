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
#include <range/v3/all.hpp>

namespace celaeno::graph::a_star
{

template<
    class T1,
    class Container = std::vector<T1>,
    class Compare = std::less<typename Container::value_type>
>
class PriorityQueue : public std::priority_queue<T1, Container, Compare>
{
  public:
    template<typename Comp>
    std::optional<T1> find(Comp&& comp) const noexcept
    {
      auto search = std::find_if(
        this->c.cbegin(),
        this->c.cend(),
        std::forward<Comp>(comp)
      );

      if( search != this->c.cend() )
      {
        return *search;
      }
      else
      {
        return std::nullopt;
      }
    }

#ifdef DEBUG
    auto get_container() const noexcept
    {
      return this->c;
    }
#endif

    template<typename Comp>
    void erase(Comp&& comp) noexcept
    {
      auto search = std::find_if(
        this->c.cbegin(),
        this->c.cend(),
        std::forward<Comp>(comp)
      );

      if( search != this->c.cend() )
      {
        this->c.erase(search);
      }
    }
};

template<typename Map, typename T>
std::vector<T> rebuild_path(Map&& m, T current)
{
  std::vector<T> final_path;
  auto comp{current};

  final_path.emplace_back(comp);

  while( m.contains( comp ) )
  {
    comp = m[comp];
    final_path.emplace_back(comp);
  }

  std::reverse(final_path.begin(), final_path.end());

  return final_path;
}

// T must be default constructible
template<typename T, typename F1, typename F2, typename F3>
std::optional<std::vector<T>> a_star(
  T start,
  T end,
  F1 f_neighbors,
  F2 f_distance,
  F3 f_heuristic
  )
{
  using float64_t = long double;
  // id, g_score, f_score
  using Entry = std::tuple<T,float64_t,float64_t>;

  // Comparison by f_score
  auto comp = [](auto& a, auto& b)
    {
      return std::get<2>(a) > std::get<2>(b);
    };

  // Open set
  std::set<T> open;

  // Paths memory
  std::map<T,T> mem;

  // Ascending priority queue
  PriorityQueue<Entry,std::vector<Entry>,decltype(comp)> p_queue;

  // Insert start node
  p_queue.push({start,0,f_heuristic(start)});

  // Main loop
  while( ! p_queue.empty() )
  {

#ifdef DEBUG // Queue ordering test
    auto const& c {p_queue.get_container()};
    for(auto it{c.cbegin()}; it!=c.cend(); ++it)
    {
      if(it+1 == c.cend()) break;
      assert(
          std::get<0>(*it) < std::get<0>(*(it+1))
          && "Unordered priority queue for A*"
      );
    }
#endif

    auto current { p_queue.top() };
    auto& curr_id { std::get<0>(current) };
    auto& curr_g  { std::get<1>(current) };
    auto& curr_f  { std::get<2>(current) };

    if ( curr_id == end )
      return rebuild_path(mem,curr_id);

    p_queue.pop();
    for( auto n : f_neighbors(curr_id) )
    {
      // If the first time exploring node, add to the open set
      if( ! open.contains(n) ) open.insert(n);
      // else, continue
      else continue;

      auto n_comp = [&n](auto&& v) { return std::get<0>(v) == n; };

      if( auto search = p_queue.find(n_comp) )
      {
        // The node is already in the queue
        // Must retrieve the original value
        auto& n_g  {std::get<1>(*search)};
        auto& n_f  {std::get<2>(*search)};
        auto test_g {curr_g + f_distance(curr_id, n)};
        if( test_g < n_g )
        {
          mem[n] = curr_id;
          p_queue.erase(n_comp);
          p_queue.push({n,test_g,test_g+f_heuristic(n)});
        }
      }
      else
      {
        // There is no known path to new node,
        // create a new path from the current
        // node.
        mem[n] = curr_id;
        auto g {curr_g + f_distance(curr_id,n)};
        p_queue.push({n,g,g+f_heuristic(n)});
      }
    }
  }

  return std::nullopt;
}

} // namespace celaeno::graph::a_star
