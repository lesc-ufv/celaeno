# Celaeno - A Graph Algorithms Library Written in C++20

<table>
  <tr>
    <td style="text-align: center" width="9999">
      <img width="200px" src="doc/character/celaeno.png">
    </td>
    <td style="text-align: justify" width="9999">
      Hi! My name is celæno, and I'm here to provide several algorithms to use with your data structures!
      Each algorithm is designed to work perfectly with behaviors passed as lambda expressions, complicated?
      Not at all! Check out all examples in the official documentation, and also, see the remainder of this document
      for a preview of the provided functionalities. But...  "What if I do not want to implement a data structure?".
      Celæno has your back! There is a very efficient graph class already implemented with the library!
      If you find an issue, no need for a tissue, open an issue! Thus, helping make this library better!
      Issues of algorithm requests are also most welcome!
    </td>
  </tr>
</table>


[[_TOC_]]

## Examples

### Breadth-First Search

```cpp
#include <fmt/ranges.h>

#include <celaeno/aliases.hpp>
#include <celaeno/graph/graph.hpp>
#include <celaeno/graph/search/bfs.hpp>

// Using declarations
using namespace celaeno::aliases;

// namespaces
namespace graph = celaeno::graph;
namespace bfs = celaeno::graph::search::bfs;

i32 main()
{
  // Graph
  graph::Graph<i64> g
  {
    {1,5},{2,4},{3,4},{6,9},
    {4,7},{5,7},{5,8},{5,9},
    {7,12},{8,10},{8,11},
  };

  // Required behaviors
  auto f_pred = [&g](auto&& u){ return g.predecessors(u); };
  auto f_succ = [&g](auto&& u){ return g.successors(u); };

  // Run algorithm
  auto search {bfs::run(1,f_pred,f_succ)};

  // Print Result
  fmt::print("Bfs ordering: {}\n", search);

  return 0;
} // main
```

Result:

`1,5,7,8,9,12,4,10,11,6,2,3,`

The drawing of the example graph already used an algorithm for edge crossing minimization, that is why it has no crossings.

### Graph SVG Writer

**TODO**

## Documentation

You can read the full API documentation in my  [gitlab pages](https://formigoni.gitlab.io/celaeno/).

For a quick summary, here's my list of implemented algorithms:

| Class                  | Algorithm | Execution |
|:----------------------:|:-----:|:---------:|
| Reader |  Verilog | ![](https://img.shields.io/static/v1?label=&message=Sequential&color=green) |
| Search | Breadth-First Search | ![](https://img.shields.io/static/v1?label=&message=Sequential&color=green) |
| Search |  Depth-First Search | ![](https://img.shields.io/static/v1?label=&message=Sequential&color=green) |
| Search |  Topological Sorting | ![](https://img.shields.io/static/v1?label=&message=Sequential&color=green) |
| View |  Topological | ![](https://img.shields.io/static/v1?label=&message=Sequential&color=green) |
| View |  Proximity | ![](https://img.shields.io/static/v1?label=&message=Sequential&color=green) |
| Representation |  Incidence | ![](https://img.shields.io/static/v1?label=&message=Concurrent&color=blue) |
| Operation |  Balance Outcoming Edges | ![](https://img.shields.io/static/v1?label=&message=Sequential&color=green) |
| Operation |  Balance Paths | ![](https://img.shields.io/static/v1?label=&message=Sequential&color=green) |
| Operation |  Count Crossings | ![](https://img.shields.io/static/v1?label=&message=Sequential&color=green) |
| Operation |  Minimize Crossings | ![](https://img.shields.io/static/v1?label=&message=Concurrent&color=blue) |

## Benchmarks


<table>
  <tr>
    <td style="text-align: center" width="9999">
      <h3>Breadth-First Search</h3>
      <img width="800px" src="doc/benchmarks/benchmark-bfs.png">
    </td>
    <td style="text-align: justify" width="9999">
      <h3>Depth-First Search</h3>
      <img width="800px" src="doc/benchmarks/benchmark-dfs.png">
    </td>
    <td style="text-align: justify" width="9999">
      <h3>Topological Search</h3>
      <img width="800px" src="doc/benchmarks/benchmark-kahn.png">
    </td>
  </tr>
  <tr>
    <td style="text-align: center" width="9999">
      <h3>Balance Paths</h3>
      <img width="800px" src="doc/benchmarks/benchmark-balance-paths.png">
    </td>
    <td style="text-align: justify" width="9999">
      <h3>Count Edge Crossings</h3>
      <img width="800px" src="doc/benchmarks/benchmark-count-crossings.png">
    </td>
    <td style="text-align: justify" width="9999">
      <h3>Minimize Edge Crossings</h3>
      <img width="800px" src="doc/benchmarks/benchmark-minimize-crossings.png">
    </td>
  </tr>
  <tr>
    <td style="text-align: center" width="9999">
      <h3>Topological (Depth) View</h3>
      <img width="800px" src="doc/benchmarks/benchmark-depth.png">
    </td>
    <td style="text-align: justify" width="9999">
      <h3>Proximity View</h3>
      <img width="800px" src="doc/benchmarks/benchmark-proximity.png">
    </td>
  </tr>
</table>

## Made Possible With

<img width="200px" src="https://raw.githubusercontent.com/onqtam/doctest/master/scripts/data/logo/logo_1.svg">
<img width="200px" src="doc/libraries-artwork/range-v3.png">
<img width="200px" src="doc/libraries-artwork/fmt.png">
<img width="200px" src="http://www.editgym.com/fplus-api-search/fplus.png">
<img width="200px" src="doc/libraries-artwork/spdlog.png">
