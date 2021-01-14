# Minimize Crossings

<!-- toc -->

## Definition

Reorders the layers of an input graph, in such a way that it minimizes edge crossings
between layers \\(L_0...L_n\\).


> [1] K. Sugiyama, S. Tagawa and M. Toda, "Methods for Visual Understanding of
> Hierarchical System Structures," in IEEE Transactions on Systems, Man, and
> Cybernetics, vol. 11, no. 2, pp. 109-125, Feb. 1981, doi:
> 10.1109/TSMC.1981.4308636.

## Interface

```cpp
template<SignedIntegral S, typename N1, typename N2, typename N3, typename E1, typename E2>
decltype(auto) run(S root, N1&& f_p, N2&& f_s, N3&& f_a, E1&& f_l, E2&& f_u)
  requires CallableWith<N1,i64>
  && CallableWith<N2,i64>
  && CallableWith<N3,i64,i64>
  && CallableWith<E1,std::pair<i64,i64>>
  && CallableWith<E2,std::pair<i64,i64>>
```

## Example with [Taygete](https://gitlab.com/formigoni/taygete)

```cpp
// Read graph {{{
graph::Graph<i64> g
{
{6,8},{2,4},{3,4},{1,5},
{4,7},{5,7},{5,8},{5,9},
{7,12},{8,10},{8,11},
};
// }}}

// vertices_count > 0 {{{
assertm(g.vertices_count() > 0, "Empty input graph");
// }}}

// Test minimize crossings {{{
auto f_p = [&g](auto&& v){ return g.predecessors(v); };
auto f_s = [&g](auto&& v){ return g.successors(v); };
auto f_l = [&g](auto&& e){ g.emplace(e); };
auto f_u = [&g](auto&& e){ g.erase(e); };


// Print ordered {{{
mc::run(1,f_p,f_s,f_l,f_u);
// }}}
```


<!--   vim: set expandtab fdm=marker ts=2 sw=2 tw=80 et : -->
