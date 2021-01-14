# SVG

<!-- toc -->

## Definition

Draws a topologically ordered graph into an _SVG_ file.

## Interface
```cpp
decltype(auto) run(
  S root,
  F1&& f_pred,
  F2&& f_succ,
  F3&& f_adj,
  F4&& f_link,
  F5&& f_unlink,
  F6&& f_label,
  Str&& fn
)
  requires CallableWith<F1,i64>
  && CallableWith<F2,i64>
  && CallableWith<F3,i64,i64>
```

## Samples

### Draw a circuit graph

A that represents a logic circuit, _pseudo nodes_ are blank, and logic gates
draw with boolean algebra symbols.

<center>
<img src=./img/1.png>
</center>

### Code

```cpp
  // namespaces {{{
  namespace graph = taygete::graph;
  namespace reader = taygete::graph::reader;
  namespace svg = celaeno::graph::draw::svg;
  namespace circ = maia::circuits;
  // }}}

  // Read graph {{{
  graph::Graph<i64> g;
  auto emplace = [&g](auto&& e) -> void { g.emplace(e); };
  auto metadata {reader::Reader{circ::synth_91::b1,emplace}};
  // }}}

  // Helpers {{{
  auto f_p = [&g](auto&& v){ return g.predecessors(v); };
  auto f_s = [&g](auto&& v){ return g.successors(v); };
  auto f_a = [&g](auto&& u, auto&& v){ return g.adjacent(u,v); };
  auto f_l = [&g](auto&& e){ g.emplace(e); };
  auto f_u = [&g](auto&& e){ g.erase(e); };
  // }}}

  // Gate type {{{
  auto f_label = [&metadata](auto id)
  {
    using Type = taygete::graph::reader::GateType;
    auto data{metadata.data()};
    if( ! data.contains(id) )  { return " "; }
    switch (data.at(id))
    {
      case Type::AND: return "∧";
      case Type::OR: return "∨";
      case Type::NAND: return "~∧";
      case Type::NOR: return "~∨";
      case Type::XOR: return "⊕";
      case Type::XNOR: return "~⊕";
      case Type::MAJ3: return "M";
    } // switch
  };
  // }}}

  // Test drawing {{{
  svg::run(1,f_p,f_s,f_a,f_l,f_u,f_label,"artifacts/b1-default.svg");
  // }}}
```
