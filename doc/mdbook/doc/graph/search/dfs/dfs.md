# Depth-First Search

## Definition

<!-- toc -->

From the book Introduction to Algorithms, 3rd Edition:

> The strategy followed by depth-first search is, as its name implies, to search
“deeper” in the graph whenever possible. Depth-first search explores edges out
of the most recently discovered vertex  that still has unexplored edges
leaving it.  Once all of ’s edges have been explored, the search “backtracks”
to explore edges leaving the vertex from which  was discovered. This process
continues until we have discovered all the vertices that are reachable from the
original source vertex.

_Cormen, Thomas H., and Charles E. Leiserson. Introduction to Algorithms, 3rd
Edition. 2009._

## Interface

```cpp
template<SignedIntegral T, typename P, typename S, typename C = std::function<bool(int64_t)>>
std::vector<T> run(T root, P&& pred, S&& succ, C&& cb = [](auto&&){return false;})
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
#include <celaeno/graph/search/dfs.hpp>


// namespaces {{{
namespace fp = fplus;
namespace graph = taygete::graph;
namespace dfs = celaeno::graph::search::dfs;
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
  auto result {dfs::run(1,p,s)};
  // }}}

  // Print to after {{{
  fmt::print("Dfs ordering: {}\n", result);
  // }}}

  return EXIT_SUCCESS;
} // main
```

Output:
```shell
Dfs ordering: {1, 5, 9, 6, 8, 11, 10, 7, 12, 4, 3, 2}
```

<!-- vim: set conceallevel=0 expandtab fdm=marker ts=2 sw=2 tw=80 et : -->
