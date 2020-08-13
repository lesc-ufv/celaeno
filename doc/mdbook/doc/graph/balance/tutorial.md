# Tutorial

<!-- toc -->

## Example with std::multimap

```cpp
#include <iostream>
#include <map>
#include <cstdlib>
#include <range/v3/all.hpp>
#include <celaeno/graph/balance.hpp>

int main()
{
  namespace rg = ranges;
  namespace rv = ranges::views;
  namespace balance = celaeno::graph::balance;

  // Create a simple graph
  std::multimap<int32_t,int32_t> graph
  {
    {1,5},{2,4},{3,4},{6,9},
    {4,7},{5,7},{5,8},{5,9},
    {7,12},{8,10},{8,11},
  };

  // Create a lambda to obtain predecessor vertices
  auto pred = [&graph](auto&& src) -> std::vector<int32_t>
  {
    return graph
    | rv::filter([&src](auto&& e){ return src == e.second; })
    | rv::transform([](auto&& e){ return e.first; })
    | rg::to<std::vector>;
  };

  // Create a lambda to obtain successor vertices
  auto succ = [&graph](auto&& src) -> std::vector<int32_t>
  {
    return graph
    | rv::filter([&src](auto&& e){ return src == e.first; })
    | rv::transform([](auto&& e){ return e.second; })
    | rg::to<std::vector>;
  };

  // Create a lambda to link vertices
  auto link = [&graph](auto&& e) -> void { graph.emplace(e); };

  // Create a lambda to unlink vertices
  auto unlink = [&graph](auto&& e) -> void
  {
    auto rng {graph.equal_range(e.first)};
    for (auto it{rng.first}; it != rng.second; ++it)
    {
      if( it->second == e.second )
      {
        graph.erase(it);
        break;
      }
    } // for: it != it.second
  };

  balance::run(1, pred, succ, link, unlink);

  return EXIT_SUCCESS;
} // main

```
<!-- ## Result -->

<!-- TODO Include result ->

<!-- vim: set expandtab fdm=marker ts=2 sw=2 tw=80 et : -->
