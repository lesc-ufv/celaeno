# Celaeno - Research in Field-Coupled Nanocomputing



<!-- <img width="100px" src="doc/character/celaeno.png"> -->

Table of Contents:

[[_TOC_]]


## Implemented Algorithms

- Reader
  - [x] Verilog
- Search
  - [x] Breadth-First Search
  - [x] Depth-First Search
  - [x] Topological Search
  - [x] A*
- Views
  - [x] Topological
- Representations
  - [x] Grid
  - [x] Incidence
- Operations
  - Count
    - [x] [Crossings](https://link.springer.com/chapter/10.1007/3-540-36151-0_13)
  - Minimize
    - [x] [Crossings](https://ieeexplore.ieee.org/document/4308636)
    - [x] Edge Length
    - [x] Dummy Nodes
- Draw
  - [x] SVG
- Tech
  - NML
    - [x] NanoSCL

## CMake Flags

| CMake Flag       | Description                              | Default|
| :----:           | :----:                                   | :----: |
| `DEBUG_SVG_HPP`  | " Enables logging for svg.hpp          " | OFF    |
| `DEBUG_SHOW_ALG` | " Displays executed algorithms orderly " | OFF    |
| `DEBUG_SHOW_HDL` | " Displays read HDL expressions        " | OFF    |
| `OPT_SEQ`        | " Single threaded mode                 " | OFF    |

## Compiling

### Docker (Recommended)

**Step 1: Build the docker image**

```sh
docker build -t celaeno:latest -f docker/Dockertests .
```

`Optional: Enable CMake flags`

To enable any of the [CMake flags](#cmake-flags), use them as a build argument, e.g.:

```sh
docker build -t celaeno:latest --build-arg OPT_SEQ=true -f docker/Dockertests .
```

`Optional: Include pre-configured neovim`

```sh
docker build -t celaeno:latest --build-arg VIM=true -f docker/Dockertests .
```

To enable clangd inside vim, use `:PluginInstall` and `:CocInstall coc-clangd`

**Step 2: Run the docker image**

```sh
docker container run -it --rm celaeno:latest
```


## How to Use Examples

### HDL to SVG

To use the svg drawing algorithm:

```sh
./build/test/svg -i test/data/synth-91/b1.v -o b1
```

### HDL to [Nanomagnetic Logic Simulator (NMLSIM) 1.0](https://gitlab.com/formigoni/docker-eda)

To use the svg drawing algorithm:

```sh
./build/test/nanoscl -i test/data/synth-91/b1.v -o ./shared/b1
```

Several sample circuits are available under the `data` directory.

## Documentation

:speech_balloon: You can read the full library documentation [here](https://formigoni.gitlab.io/celaeno/).

Algorithms information:

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
      Breadth-First Search
      <img width="800px" src="doc/benchmarks/benchmark-bfs.png">
    </td>
    <td style="text-align: justify" width="9999">
      Depth-First Search
      <img width="800px" src="doc/benchmarks/benchmark-dfs.png">
    </td>
    <td style="text-align: justify" width="9999">
      Topological Search
      <img width="800px" src="doc/benchmarks/benchmark-kahn.png">
    </td>
  </tr>
  <tr>
    <td style="text-align: center" width="9999">
      Balance Paths
      <img width="800px" src="doc/benchmarks/benchmark-balance-paths.png">
    </td>
    <td style="text-align: justify" width="9999">
      Count Edge Crossings
      <img width="800px" src="doc/benchmarks/benchmark-count-crossings.png">
    </td>
    <td style="text-align: justify" width="9999">
      Minimize Edge Crossings
      <img width="800px" src="doc/benchmarks/benchmark-minimize-crossings.png">
    </td>
  </tr>
  <tr>
    <td style="text-align: center" width="9999">
      Topological (Depth) View
      <img width="800px" src="doc/benchmarks/benchmark-depth.png">
    </td>
    <td style="text-align: justify" width="9999">
      Proximity View
      <img width="800px" src="doc/benchmarks/benchmark-proximity.png">
    </td>
  </tr>
</table>


## Made Possible With

<table>
<tr>
<td style="text-align: center" width="9999">
[doctest](https://github.com/onqtam/doctest)
</td>
<td style="text-align: center" width="9999">
[range-v3](https://github.com/ericniebler/range-v3)
</td>
<td style="text-align: center" width="9999">
[functionalplus](https://github.com/Dobiasd/FunctionalPlus/)
</td>
<td style="text-align: center" width="9999">
[fmtlib](https://github.com/fmtlib/fmt)
</td>
<td style="text-align: center" width="9999">
[spdlog](https://github.com/gabime/spdlog)
</td>
</tr>
</table>
