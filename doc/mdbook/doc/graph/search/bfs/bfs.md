# Breadth-First Search

## Definition

<!-- toc -->

From the book Introduction to Algorithms, 3rd Edition:

> Given a graph \\(G(V,E)\\) and a distinguished source vertex s, breadth-first
search systematically explores the edges of G to “discover” every vertex that
is reachable from s. It computes the distance (smallest number of edges) from s
to each reachable vertex. It also produces a “breadth-first tree” with root s
that contains all reachable vertices. For any vertex  reachable from s, the
simple path in the breadth-first tree from s to  corresponds to a “shortest
path” from s to in G, that is, a path containing the smallest number of edges.
The algorithm works on both directed and undirected graphs.

_Cormen, Thomas H., and Charles E. Leiserson. Introduction to Algorithms, 3rd
Edition. 2009._

## Interface

```cpp
template<SignedIntegral T, typename P, typename S, typename C = std::function<bool(i64)>>
std::vector<T> run(T root, P&& f_pred, S&& f_succ, C&& f_cb = [](auto&&){return false;})
  requires CallableWith<P,i64>
  && CallableWith<S,i64>
  && CallableWith<C,i64>
```
## Example with [Taygete](https://gitlab.com/formigoni/taygete)

```cpp
#include <iostream>
#include <cstdlib>
#include <fplus/fplus.hpp>
#include <fmt/ranges.h>
#include <taygete/graph/graph.hpp>
#include <celaeno/aliases.hpp>
#include <celaeno/graph/search/bfs.hpp>


// namespaces {{{
namespace fp = fplus;
namespace graph = taygete::graph;
namespace bfs = celaeno::graph::search::bfs;
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
  auto result {bfs::run(1,p,s)};
  // }}}

  // Print to after {{{
  fmt::print("Bfs ordering: {}\n", result);
  // }}}

  return EXIT_SUCCESS;
} // main
```

Output:
```shell
Bfs ordering: {1, 5, 7, 8, 9, 4, 12, 10, 11, 6, 2, 3}
```

<!-- vim: set expandtab fdm=marker ts=2 sw=2 tw=80 et : -->
