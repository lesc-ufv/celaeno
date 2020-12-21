# Celaeno - A Graph Algorithms Library Written in C++20

<table>
  <tr>
    <td style="text-align: center" width="9999">
      <img width="200px" src="doc/character/celaeno.png">
    </td>
    <td style="text-align: justify" width="9999">
      Hi! My name is celæno, and I'm here to provide several algorithms
      to use with <b>your</b> data structures! Each algorithm is designed to work perfectly with <i>behaviors</i> passed as lambda expressions,
      complicated? Not at all! Check out the examples in the oficial documentation, and also, keep reading to see a preview the
      provided functionalities. But... <b> "What if I do not want to implement a data structure?"</b> :sweat:. Celæno has your back!
      "Nani?!" :open_mouth:. Yes, that is right, there is a very efficient graph class already implemented with the library! :heart_eyes:.
      And only for an <b>UNLIMITED</b> time you get, not one, not five, not seven, but ten algorithms! For free, forever!
      If you find an issue :scream:, no need for a tissue :cry:, open an issue! Thus, helping make this library better! "But, that one algorithm
      that I really need is not implemented". No worries, you can open an issue with algorithm request! Isn't that great guys and gals! Hmmm?!
    </td>
  </tr>
</table>


[[_TOC_]]

## Examples

### Breadth-First Search

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

Result:

`1,5,7,8,9,12,4,10,11,6,2,3,`


### Graph Topological View

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
  // 1 → level -> nodes
  // 2 → node -> level
  auto [lvns,nslv] = depth::run(1, pred, succ);

  return EXIT_SUCCESS;
} // main

```

Result:

`Level 0: [0,1] [0,6] [0,2] [0,3]`

`Level 1: [1,5] [1,4]`

`Level 2: [2,8] [2,9] [2,7]`

`Level 3: [3,10] [3,11] [3,12] `

### Adjacency Matrix

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

```
  M1      M2      M3
[0,1]  [1,0,0]  [0,0,1]
[1,0]  [1,1,1]  [1,1,0]
[1,0]           [0,0,0]
[0,0]
```

### Graph Edge Crossings Count

```cpp
#include <iostream>
#include <map>
#include <cstdlib>
#include <range/v3/all.hpp>
#include <celaeno/graph/matrix-realization.hpp>
#include <celaeno/graph/crossings.hpp>
#include <celaeno/graph/views/depth.hpp>

int main()
{
  namespace rg = ranges;
  namespace rv = ranges::views;
  namespace ra = ranges::actions;
  namespace depth = celaeno::graph::views::depth;
  namespace matrix_realization = celaeno::graph::matrix_realization;
  namespace crossings = celaeno::graph::crossings;

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

  // Adjacency matrices
  auto ms = matrix_realization::run(get_level, adjacent, levels);

  // Return the number of crossings for each adjacency matrix
  auto f_sum = []<typename... T>(T&&... m){return (crossings::run(std::forward<T>(m)) + ...);};

  // Total number of crossings in the graph
  auto sum = f_sum(ms.at(0), ms.at(1), ms.at(2));

  return EXIT_SUCCESS;
} // main
```

Result:

`Crossings: 4`

The drawing of the example graph already used an algorithm for edge crossing minimization, that is why it has no crossings.

### Graph SVG Writer


## Documentation

You can read the full API documentation in my  [gitlab pages](https://formigoni.gitlab.io/celaeno/).

For a quick summary, here's my list of implemented algorithms:

| Class                  | Algorithm | Execution |
|:----------------------:|:-----:|:---------:|
| Reader |  Verilog | ![](https://img.shields.io/static/v1?label=&message=Sequential&color=green) |
| Search | Breadth-First Search | ![](https://img.shields.io/static/v1?label=&message=Sequential&color=green) |
| Search |  Depth-First Search | ![](https://img.shields.io/static/v1?label=&message=Sequential&color=green) |
| Search |  Topological Sorting | ![](https://img.shields.io/static/v1?label=&message=Sequential&color=green) |
| View |  Topological | ![](https://img.shields.io/static/v1?label=&message=Sequential&color=green) |
| View |  Proximity | ![](https://img.shields.io/static/v1?label=&message=Sequential&color=green) |
| Representation |  Incidence | ![](https://img.shields.io/static/v1?label=&message=Concurrent&color=blue) |
| Operation |  Balance Outcoming Edges | ![](https://img.shields.io/static/v1?label=&message=Sequential&color=green) |
| Operation |  Balance Paths | ![](https://img.shields.io/static/v1?label=&message=Sequential&color=green) |
| Operation |  Count Crossings | ![](https://img.shields.io/static/v1?label=&message=Sequential&color=green) |
| Operation |  Minimize Crossings | ![](https://img.shields.io/static/v1?label=&message=Concurrent&color=blue) |

## Benchmarks

## Made Possible With

<img width="200px" src="https://raw.githubusercontent.com/onqtam/doctest/master/scripts/data/logo/logo_1.svg">
<img width="200px" src="doc/libraries-artwork/range-v3.png">
<img width="200px" src="doc/libraries-artwork/fmt.png">
<img width="200px" src="http://www.editgym.com/fplus-api-search/fplus.png">
<img width="200px" src="doc/libraries-artwork/spdlog.png">
