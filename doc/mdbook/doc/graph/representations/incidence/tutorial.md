# Tutorial

<!-- toc -->

## Example with [Taygete](https://gitlab.com/formigoni/taygete)

```cpp
#include <iostream>
#include <cstdlib>
#include <fmt/core.h>
#include <fmt/ranges.h>
#include <taygete/graph/graph.hpp>
#include <celaeno/aliases.hpp>
#include <celaeno/graph/representations/incidence.hpp>


// namespaces {{{
namespace graph = taygete::graph;
namespace incidence = celaeno::graph::representations::incidence;
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
  auto matrices {incidence::run(1,p,s)};
  // }}}

  // Print incidence matrices {{{
  fmt::print("Incidence Matrices: \n");
  for (auto&& m : matrices)
  {
    for (auto&& v : m)
    {
      fmt::print("{}\n",v);
    } // for v : m
    fmt::print("\n--------\n");
  } // for m : matrices }}}

  return EXIT_SUCCESS;
} // main
```

Result:
```shell
Incidence Matrices: 
{0, 1, 0}
{1, 0, 0}
{1, 0, 0}

--------
{1, 0, 0}
{1, 1, 1}
{0, 0, 1}

--------
{0, 0, 1}
{1, 1, 0}
{0, 0, 0}

--------
```
