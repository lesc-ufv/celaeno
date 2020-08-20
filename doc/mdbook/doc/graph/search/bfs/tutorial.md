# Tutorial

<!-- toc -->

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
