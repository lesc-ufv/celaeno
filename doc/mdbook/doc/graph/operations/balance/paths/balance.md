# Balance Paths

<!-- toc -->

## Definition

> The algorithm adds _pseudo_ vertices to remove intra-partition edges,
an edge _uv_ is intra-partition, if it spans through outer layers of a graph
\\(G(V,E)\\). As shown by Figure 1, edge _B_ is intra-partition, and edge
_A_ is inter-partition.

<center>
<img src=./img/intra-inter-edges.png>
</center>

## Example

> Figure 2a shows a graph \\(G(V,E)\\), with intra-edges _ia_ and _co_, Figure
2b shows the graph after the execution of the algorithm, this graph is of a
special class named _K-Layered Bipartite Graph_.

<center>
<img src=./img/path-balancing.png>
</center>

## Interface

```cpp
template<SignedIntegral T, typename P, typename S, typename L, typename U>
void run(T root, P&& f_pred, S&& f_succ, L&& f_link, U&& f_unlink )
  requires CallableWith<P,i64>
  && CallableWith<S,i64>
  && CallableWith<L,std::pair<i64,i64>>
  && CallableWith<U,std::pair<i64,i64>>
```

## Example with [Taygete](https://gitlab.com/formigoni/taygete)

```cpp
#include <iostream>
#include <cstdlib>
#include <fmt/ranges.h>
#include <taygete/graph/graph.hpp>
#include <celaeno/aliases.hpp>
#include <celaeno/graph/operations/balance/paths.hpp>
#include <celaeno/graph/views/depth.hpp>


// namespaces {{{
namespace graph = taygete::graph;
namespace depth = celaeno::graph::views::depth;
namespace balance = celaeno::graph::operations::balance::paths;
// }}}

int main()
{
  // Graph {{{
  graph::Graph<i64> g
  {
    {1,5},{2,4},{3,7},{6,11},
    {4,7},{5,7},{5,8},{5,9},
    {7,12},{8,10},{8,11},
  };
  // }}}

  // Required behaviors {{{
  auto p = [&g](auto&& u){ return g.predecessors(u); };
  auto s = [&g](auto&& u){ return g.successors(u); };
  auto l = [&g](auto&& uv) -> void { g.emplace(uv); };
  auto u = [&g](auto&& uv) -> void { g.erase(uv); };
  // }}}

  // Create a topological view to analyse graph {{{
  auto [layers,_] = depth::run(1,p,s);
  // }}}

  // Print before {{{
  fmt::print("Before balancing: \n");
  i32 prev{0};
  for (auto&& l : layers)
  {
    fmt::print("{} ",l.second);
    if( l.first > prev ){ prev = l.first; fmt::print("\n"); }
  } // for l : layers }}}
  fmt::print("\n-------\n");


  // Run algorithm {{{
  balance::run(1,p,s,l,u);
  // }}}

  // Create a topological view to analyse graph {{{
  layers = depth::run(1,p,s).first;
  // }}}

  // Print to after {{{
  fmt::print("After balancing: \n");
  prev = 0;
  for (auto&& l : layers)
  {
    fmt::print("{} ",l.second);
    if( l.first > prev ){ prev = l.first; fmt::print("\n"); }
  } // for l : layers }}}
  fmt::print("\n");

  return EXIT_SUCCESS;
} // main
```

Output:
```shell
Before balancing: 
1 3 2 6 5 
4 8 
9 7 10 
11 12 
-------
After balancing: 
1 3 2 6 5 
0 4 -1 8 
9 7 -2 10 
12 11
```

<!-- vim: set expandtab fdm=marker ts=2 sw=2 tw=80 et : -->

<!-- vim: set expandtab fdm=marker ts=2 sw=2 tw=80 et : -->
