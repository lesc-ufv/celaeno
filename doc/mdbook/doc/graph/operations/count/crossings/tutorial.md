# Tutorial

<!-- toc -->

## Example with [Taygete](https://gitlab.com/formigoni/taygete)

```cpp
#include <iostream>
#include <cstdlib>
#include <fmt/ranges.h>
#include <taygete/graph/graph.hpp>
#include <celaeno/aliases.hpp>
#include <celaeno/graph/operations/count/crossings.hpp>


// namespaces {{{
namespace graph = taygete::graph;
namespace count = celaeno::graph::operations::count::crossings;
// }}}

int main()
{
  // Graph {{{
  graph::Graph<i64> g
  {
    {1,5},{2,4},{3,4},{6,8},
    {4,7},{5,7},{5,8},{5,9},
    {7,12},{8,10},{8,11},
  };
  // }}}

  // Obtain predecessor and successor nodes {{{
  auto p = [&g](auto&& v){ return g.predecessors(v); };
  auto s = [&g](auto&& v){ return g.successors(v); };
  // }}}

  // Print to stdout {{{
  fmt::print("{}\n",count::run(1,p,s));
  // }}}

  return EXIT_SUCCESS;
} // main
```

Output:
```shell
5
```

<!-- vim: set expandtab fdm=marker ts=2 sw=2 tw=80 et : -->
