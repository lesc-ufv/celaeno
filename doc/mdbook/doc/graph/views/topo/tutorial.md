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
