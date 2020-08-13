# Tutorial

<!-- toc -->

## Example with std::multimap

```cpp
#include <iostream>
#include <map>
#include <cstdlib>
#include <range/v3/all.hpp>
#include <celaeno/graph/bfs.hpp>

int main()
{
  namespace rg = ranges;
  namespace rv = ranges::views;
  namespace bfs = celaeno::graph::bfs;

  // Create a simple graph
  std::multimap<int32_t,int32_t> graph
  {
    {1,5},{2,4},{3,4},{6,9},
    {4,7},{5,7},{5,8},{5,9},
    {7,12},{8,10},{8,11},
  };

  // Create a lambda to obtain neighboring vertices
  auto neighbors = [&graph](auto&& v) -> std::vector<int32_t>
  {
    auto pred = graph
      | rv::filter([&v](auto&& e){ return e.second == v; })
      | rv::transform([](auto&& e){ return e.first; });
    auto succ = graph
      | rv::filter([&v](auto&& e){ return e.first == v; })
      | rv::transform([](auto&& e){ return e.second; });
    return rv::concat(succ,pred) | rg::to<std::vector>;
  };

  // OPTIONAL callback applied to each node in a breadth-first order
  // Must return boolean to continue, useful searching for a
  // specific node in a bfs order
  auto callback = [](auto&& e){ std::cout << e << ",";  return false;};

  // Run the bfs algorithm
  auto result {bfs::run(1, neighbors, callback)};

  return EXIT_SUCCESS;
} // main
```

<!-- ## Result -->

<!-- TODO Include result ->

<!-- vim: set expandtab fdm=marker ts=2 sw=2 tw=80 et : -->
