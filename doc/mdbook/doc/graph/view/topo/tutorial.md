# Tutorial

<!-- toc -->

## Example with std::multimap

```cpp
#include <iostream>
#include <map>
#include <cstdlib>
#include <range/v3/all.hpp>
#include <celaeno/graph/views/depth.hpp>

int main()
{
  namespace rg = ranges;
  namespace rv = ranges::views;
  namespace depth = celaeno::graph::views::depth;

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

  // Returns a pair
  // 1 → level -> vertices
  // 2 → vertex -> level
  auto [lv,vl] = depth::run(1, pred, succ);

  return EXIT_SUCCESS;
} // main
```

<!-- ## Result -->

<!-- TODO Include result ->

<!-- vim: set expandtab fdm=marker ts=2 sw=2 tw=80 et : -->
