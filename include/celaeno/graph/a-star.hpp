//
// @author      : Ruan E. Formigoni (ruanformigoni@gmail.com)
// @file        : a-star
// @created     : Thursday Feb 20, 2020 12:05:37 -03
//

#pragma once

#include <queue>
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
  T&& start,
  T&& end,
  F1&& f_neighbors,
  F2&& f_distance,
  F3&& f_heuristic
  )
{
  // id, g_score, f_score
  using Entry = std::tuple<T,T,T>;

  // Comparison by f_score
  auto comp = [](auto& a, auto& b)
    { return std::get<2>(a) < std::get<2>(b); };

  // Paths memory
  std::unordered_map<T,T> mem;

  // Ascending priority queue
  PriorityQueue<Entry,std::vector<Entry>,decltype(comp)> p_queue;

  // Insert start node
  p_queue.push({start,0,f_heuristic(start)});

  // Main loop
  while( ! p_queue.empty() )
  {
    auto current { p_queue.top() };
    auto& curr_id { std::get<0>(current) };
    auto& curr_g  { std::get<1>(current) };
    auto& curr_f  { std::get<2>(current) };

    if ( curr_id == end )
      return rebuild_path(mem,curr_id);

    p_queue.pop();
    for( auto n : f_neighbors(curr_id) )
    {
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
          p_queue.push({n,test_g,f_heuristic(n)});
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
