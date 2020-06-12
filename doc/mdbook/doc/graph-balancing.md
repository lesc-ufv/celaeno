>  vim: set expandtab fdm=marker ts=2 sw=2 tw=80 et :

# Graph Balancing

## Definition

> This algorithm adds pseudo nodes to avoid inter-partition edges, this concept
> is show in the figure below:

<center>
<img src=./figures/graph-balancing/graph-partitions.png>
</center>

> After the insertion of the pseudo nodes to remove external partitions. The
> result is a class of graph known as _K-Layered Bipartite Graph_.
> 
<center>
<img src=./figures/graph-balancing/graph-reconvergence.png>
</center>

## Visual example

Consider the graph below, the node 8 reaches the node 9 before the node 0; this
is because 0 has to go through 2 first. Therefore, the paths are _unbalanced_.
<p align="center">
<img src=./figures/graph-balancing/original.png>
</p>

The algorithms reworks the graph with _pseudonodes_, labeled as negative
numbers, these nodes represent wires, and are useful to amortize the complexity
of many algorithms.

<p align="center">
<img src=./figures/graph-balancing/KLBG.png width=50%>
</p>

## Parameters

> <i class="fa fa-bars" style="color:steelblue"></i>&nbsp;&nbsp;
void balance(T root, F1&& pred, F2&& succ, F3&& link, F4&& unlink )

> <i class="fa fa-cog" style="color:steelblue"></i>&nbsp;&nbsp;
> root: A Signed integral.
>
> <i class="fa fa-cog" style="color:steelblue"></i>&nbsp;&nbsp;
> pred: A lambda that returns the predecessors of a given node.
>
> <i class="fa fa-cog" style="color:steelblue"></i>&nbsp;&nbsp;
> succ: A lambda that returns the successors of a given node.
>
> <i class="fa fa-cog" style="color:steelblue"></i>&nbsp;&nbsp;
> link: A lambda to insert an edge between two nodes.
>
> <i class="fa fa-cog" style="color:steelblue"></i>&nbsp;&nbsp;
> unlink: A lambda to remove an edge between two nodes.

## Usage

For a complete example of CMake and dependencies integration, see:
[Graph Balancing Example](https://gitlab.com/formigoni/celaeno/-/raw/development/doc/mdbook/archives/sample-balance.tar.xz).

To compile the example use:

```sh
// Create build directory
mkdir build && cd build
// Configure conan
conan install -s compiler=gcc -s compiler.version=10 -s compiler.libcxx=libstdc++11 .. && cd ..
// Configure CMake
cmake -H. -Bbuild
// Build the project
cmake --build build
// Run the executable
./build/bin/main
```
