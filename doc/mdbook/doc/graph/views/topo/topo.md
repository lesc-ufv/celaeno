# Topological View

<!-- toc -->

## Definition

> Give a graph \\(G(V,E)\\) a topological view of a graph, is a representation,
such that, for every edge _uv_ of a vertex _u_, the layer of the vertex is
determined by the closest predecessor vertex.  Figure 1a shows the input graph
for the algorithm, and the output is represented is Figure 2a, which satisfies
the previously constraint for every vertex.

<img src="./img/topo-view.png">

_[1]  J. Nguyen, R. Ravichandran, S. K. Lim, and M. T. Niemier, “Global
Placement for Quantum-dot Cellular Automata Based Circuits,” 2003._

## Interface

```cpp
template<SignedIntegral T, typename P, typename S>
auto run(T root, P&& f_pred, S&& f_succ)
  -> std::pair< std::map<T,std::vector<T>>, std::map<T,T> >
  requires CallableWith<P,i64>
  && CallableWith<S,i64>
```

## Example with [Taygete](https://gitlab.com/formigoni/taygete)

```cpp
#include <iostream>
#include <cstdlib>
#include <fplus/fplus.hpp>
#include <fmt/ranges.h>
#include <taygete/graph/graph.hpp>
#include <celaeno/aliases.hpp>
#include <celaeno/graph/views/depth.hpp>


// namespaces {{{
namespace fp = fplus;
namespace graph = taygete::graph;
namespace depth = celaeno::graph::views::depth;
// }}}

int main()
{
  // Graph {{{
  graph::Graph<i64> g
  {
    {1,5},{2,4},{3,4},{6,9},
    {4,7},{5,7},{5,8},{5,9},
    {7,12},{8,10},{8,11},
  };
  // }}}

  // Required behaviors {{{
  auto p = [&g](auto&& u){ return g.predecessors(u); };
  auto s = [&g](auto&& u){ return g.successors(u); };
  // }}}

  // Run algorithm {{{
  auto depth_view {depth::run(1,p,s).first};
  // }}}

  // Print topological view {{{
  i64 curr_layer{0};
  for (auto&& layer : depth_view)
  {
    fmt::print("{} ", layer.second);
    if( layer.first != curr_layer )
    {
      curr_layer = layer.first;
      fmt::print("\n");
    }
  } // for layer : depth_view
  fmt::print("\n");
  // }}}

  return EXIT_SUCCESS;
} // main
```

Output:
```shell
1 6 2 3 5 
4 8 
9 7 10 
11 12
```

<!-- vim: set expandtab fdm=marker ts=2 sw=2 tw=80 et : -->
