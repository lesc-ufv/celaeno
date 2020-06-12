# Breadth-First Search

## Definition

From the book Introduction to Algorithms, 3rd Edition:

> Given a graph G = (V,E) and a distinguished source vertex s, breadth-first
search systematically explores the edges of G to “discover” every vertex that
is reachable from s. It computes the distance (smallest number of edges) from s
to each reachable vertex. It also produces a “breadth-first tree” with root s
that contains all reachable vertices. For any vertex  reachable from s, the
simple path in the breadth-first tree from s to  corresponds to a “shortest
path” from s to in G, that is, a path containing the smallest number of edges.
The algorithm works on both directed and undirected graphs.

_Cormen, Thomas H., and Charles E. Leiserson. Introduction to Algorithms, 3rd
Edition. 2009._

## Instructional Video:

<iframe width="750" height="500" src="https://www.youtube.com/embed/oDqjPvD54Ss" frameborder="0" allow="accelerometer; autoplay; encrypted-media; gyroscope; picture-in-picture" allowfullscreen></iframe>


## Parameters

> <i class="fa fa-bars" style="color:steelblue"></i>&nbsp;&nbsp;
std::vector<T> bfs(T root, F1&& adj, F2&& cb = [](auto&&){return false;})

> <i class="fa fa-cog" style="color:steelblue"></i>&nbsp;&nbsp;
> root: A Signed integral that returns the root node.
>
> <i class="fa fa-cog" style="color:steelblue"></i>&nbsp;&nbsp;
> adj: A lambda to return adjacent nodes.
>
> <i class="fa fa-cog" style="color:steelblue"></i>&nbsp;&nbsp;
> cb: An optional callback to call in a breadth-first manner.
> The search stops if the condition returns true or all nodes
> have been explored.

## Usage

For a complete example of CMake and dependencies integration, see:
[BFS Project Example](https://gitlab.com/formigoni/celaeno/-/raw/master/doc/mdbook/archives/sample-bfs.tar.xz)

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

> BFS Result: 0,2,3,7,9,4,1,14,6,13,8,10,11,5,12,15,16
>
> _Transversed graph:_
> 
> ![bfs](./figures/bfs-b1.png)

