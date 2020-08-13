# Definition

> The algorithm adds _pseudo_ vertices to remove intra-partition edges,
an edge _uv_ is intra-partition, if it spans through outer layers of a graph
\\(G(V,E)\\). As shown by Figure 1, edge _B_ is intra-partition, and edge
_A_ is inter-partition.

<center>
<img src=./img/intra-inter-edges.png>
</center>

## Example

> Figure 2a shows a graph \\(G(V,E)\\), with intra-edges _ia_ and _co_, Figure
2b shows the graph after the execution of the algorithm, this graph is of a
special class named _K-Layered Bipartite Graph_.

<center>
<img src=./img/path-balancing.png>
</center>

<!-- vim: set expandtab fdm=marker ts=2 sw=2 tw=80 et : -->
