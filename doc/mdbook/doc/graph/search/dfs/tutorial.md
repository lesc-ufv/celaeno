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
  auto n = [&p,&s](auto&& u){return fp::append(p(u),s(u));};
  // }}}

  // Run algorithm {{{
  auto result {dfs::run(1,n)};
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
