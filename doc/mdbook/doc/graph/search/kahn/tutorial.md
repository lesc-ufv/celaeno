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
#include <celaeno/graph/search/kahn.hpp>


// namespaces {{{
namespace fp = fplus;
namespace graph = taygete::graph;
namespace kahn = celaeno::graph::search::kahn;
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
  auto result {kahn::run(1,p,s)};
  // }}}

  // Print topological ordering {{{
  fmt::print("Topological ordering: {}\n", result);
  // }}}

  return EXIT_SUCCESS;
} // main
```

Output:
```shell
Topological ordering: {1, 6, 2, 3, 5, 4, 8, 9, 7, 10, 11, 12}
```

<!-- vim: set expandtab conceallevel=0 fdm=marker ts=2 sw=2 tw=80 et : -->
