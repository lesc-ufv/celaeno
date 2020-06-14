>  vim: set expandtab fdm=marker ts=2 sw=2 tw=80 et :

# A\* Algorithm

## Definition

A\* is a path finding algorithm, with many applications in graph-theory and
path-finding problems.

_Hart, P. E.; Nilsson, N. J.; Raphael, B. (1968). "A Formal Basis for the Heuristic Determination of Minimum Cost Paths". IEEE Transactions on Systems Science and Cybernetics._

For detailed information on how A\* works, watch the video below:
<iframe width="750" height="500" src="https://www.youtube.com/embed/ySN5Wnu88nE" frameborder="0" allow="accelerometer; autoplay; encrypted-media; gyroscope; picture-in-picture" allowfullscreen></iframe>

## Interface

> <i class="fa fa-bars" style="color:steelblue"></i>&nbsp;&nbsp;
> template<BaseType T, typename F1, typename F2, typename F3>
>
> <i class="fa fa-bars" style="color:steelblue"></i>&nbsp;&nbsp;
> a_star(T&& start, T&& end, F1&& f_neighbors, F2&& f_distance, F3&& f_heuristic)

## Parameters

> <i class="fa fa-cog" style="color:steelblue"></i>&nbsp;&nbsp;
> _start_: The initial node/point to start the search.
>
> <i class="fa fa-cog" style="color:steelblue"></i>&nbsp;&nbsp;
> _end_: The goal of the search.
>
> <i class="fa fa-cog" style="color:steelblue"></i>&nbsp;&nbsp;
> *f\_neighbors*: `T → [T...]`, given a vertex, returns adjacent vertices.
>
> <i class="fa fa-cog" style="color:steelblue"></i>&nbsp;&nbsp;
> *f\_distance*: `T → int`, given a vertex, returns the distance.
>
> <i class="fa fa-cog" style="color:steelblue"></i>&nbsp;&nbsp;
> *f\_heuristic*: `T → int` or `T → float`, given a vertex, returns the
heuristic cost.

### Constraints

```cpp
template<typename T>
concept SignedInt = std::signed_integral<T>;

template<typename T>
concept BaseType =
requires(T t)
{
  { t.first  } -> std::convertible_to<int64_t>;
  { t.second } -> std::convertible_to<int64_t>;
}
||
requires (T t)
{
  { t } -> std::signed_integral;
};

```

1. Parameters of type _T_ must be of _signed integrals_.
1. Parameters of type _T_ must be a user-defined _pair_ or the standard library
   _std::pair_ type.

The first constraint allows for A\* graph transversal, in contrast to the second
constraint, which allows for matrix transversal.

## Usage

For a complete example of CMake and dependencies integration, see:
[A\* Project Example](https://gitlab.com/formigoni/celaeno/-/raw/development/doc/mdbook/archives/sample-a-star.tar.xz)

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

#### A\* Example Project Result
> [0, 2] - [1, 2] - [2, 2] - [3, 2] - [4, 2] - [5, 2] - [6, 2] - [7, 2] - [8, 2] - [9, 2] - [10, 2] - [11, 2] - [12, 2] - [13, 2] - [14, 2] - [15, 2] - [16, 2] - [17, 2] - [18, 2] - [19, 2] - [20, 2] - [21, 2] - [22, 2] - [23, 2] - [24, 2] - [25, 2] - [26, 2] - [27, 2] - [28, 2] - [29, 2] - [30, 2] - [30, 1] - [30, 0] - [30, -1] - [30, -2] - [30, -3] - [30, -4] - [30, -5] - [30, -6] - [30, -7] - [30, -8] - [30, -9] - [30, -10]
