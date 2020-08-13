# Tutorial

<!-- toc -->

## Example with std::multimap

```cpp
#include <iostream>
#include <map>
#include <cstdlib>
#include <range/v3/all.hpp>
#include <celaeno/graph/matrix-realization.hpp>
#include <celaeno/graph/views/depth.hpp>

int main()
{
  namespace rg = ranges;
  namespace rv = ranges::views;
  namespace ra = ranges::actions;
  namespace depth = celaeno::graph::views::depth;
  namespace matrix_realization = celaeno::graph::matrix_realization;

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
  // 1 → level -> nodes
  // 2 → node -> level // not used in this algorithm
  auto [lvns,_] = depth::run(1, pred, succ);

  // Key type of the multimap
  using node_t = decltype(lvns)::key_type;

  // Lambda to obtain a layer by index
  auto get_level = [&lvns](node_t idx)
  {
    return lvns
      | rv::filter([&idx](auto&& e){ return e.first == idx; })
      | rv::values
      | rg::to<std::vector>
      | ra::sort;
  };

  // Lambda to verify if edge vu exists
  auto adjacent = [&graph](node_t v, node_t u)
  {
    if( graph.contains(v) )
    {
      auto rng {graph.equal_range(v)};
      for (auto it{rng.first}; it != rng.second; ++it)
      {
        if( it->second == u ) { return true; }
      } // for: it != rng.second
    }
    return false;
  };

  // Number of levels (depth) of the graph
  auto levels {(lvns | rv::keys | rv::unique | rg::to<std::vector>).size()};

  auto matrices { matrix_realization::run(get_level, adjacent, levels) };

  return EXIT_SUCCESS;
} // main
```

Result:

| M1  |    M2  |   M3   |
|:---:|:------:|:------:|
|[0,1]| [1,0,0]| [0,0,1]|
|[1,0]| [1,1,1]| [1,1,0]|
|[1,0]|        | [0,0,0]|
|[0,0]|        |        |

