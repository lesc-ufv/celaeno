# Count Crossings

<!-- toc -->

## Definition

Consider the graph shown in Figure 1, between \\(L_0\\) and \\(L_1\\), there are
two wire crossings from the edge that starts from vertex \\(A\\); Similarly,
between levels \\(L_1\\) and \\(L_2\\), there is one crossing, originated from
the outgoing edge of vertex \\(A\\); Lastly, amidst level \\(L_2\\) and
\\(L_3\\), there are two wire crossings, due to the outgoing edge from vertex
\\(G\\). This algorithm counts all the wire crossings of a graph, **in a sorted
order with respect to the id**, in this case, the *ids* are sorted alphabetic
characters.

<center>
<img width="300px" src=./img/crossings.png>
</center>

> [1] Sugiyama, Kozo; Tagawa, Shôjirô; Toda, Mitsuhiko (1981),
> "Methods for visual understanding of hierarchical system structures", 
> IEEE Transactions on Systems, Man, and Cybernetics, SMC-11 (2): 109–125, 

## Interface

```cpp
template<SignedIntegral T = i64, Range R, typename S>
T run(R&& l1, R&& l2, S&& f_succ) requires CallableWith<S,i64>;
```

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
