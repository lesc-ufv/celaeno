// vim: set expandtab fdm=marker ts=2 sw=2 tw=100 et :
//
// @author      : Ruan E. Formigoni (ruanformigoni@gmail.com)
// @file        : zig-zag
// @created     : Tuesday Jul 27, 2021 21:57:31 UTC
//
// BSD 2-Clause License

// Copyright (c) 2020, Ruan Evangelista Formigoni
// All rights reserved.

// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:

// * Redistributions of source code must retain the above copyright notice, this
//   list of conditions and the following disclaimer.

// * Redistributions in binary form must reproduce the above copyright notice,
//   this list of conditions and the following disclaimer in the documentation
//   and/or other materials provided with the distribution.

// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
// FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
// DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
// SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
// CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
// OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.


#include <algorithm>
#include <ranges>
#include <tuple>
#include <optional>
#include <variant>

#include <fmt/core.h>

#include <fplus/fplus.hpp>
#include <range/v3/all.hpp>
#include <spdlog/spdlog.h>
#include <cppcoro/generator.hpp>

#include <celaeno/aliases.hpp>
#include <celaeno/concepts.hpp>
#include <celaeno/err/err.hpp>
#include <celaeno/fun/fun.hpp>

#include <celaeno/graph/graph.hpp>
#include <celaeno/graph/io/verilog.hpp>

#include <celaeno/graph/views/depth.hpp>

#include <celaeno/graph/draw/svg.hpp>
#include <celaeno/graph/representations/grid.hpp>

#include <celaeno/graph/search/a-star.hpp>
#include <celaeno/graph/search/bfs.hpp>
#include <celaeno/graph/search/kahn.hpp>
#include <celaeno/graph/search/zig-zag.hpp>
#include <celaeno/graph/search/aps.hpp>

#include <celaeno/heuristics/chebyshev.hpp>
#include <celaeno/heuristics/manhattan.hpp>

#include <celaeno/graph/operations/balance/crossings.hpp>
#include <celaeno/graph/operations/minimize/crossings.hpp>

// #include "unbalance.hpp"

// TODO Remove
#include <celaeno/graph/operations/balance/outgoing.hpp>
#include <celaeno/graph/operations/balance/incoming.hpp>
#include <celaeno/graph/operations/count/crossings.hpp>

#include <pybind11/embed.h>
#include <pybind11/stl.h>

// Using namespace {{{
using namespace celaeno::fun::fn;
using namespace celaeno::concepts;
using namespace celaeno::aliases;
// }}}

// namespaces {{{
namespace py = pybind11;
namespace err = celaeno::err;
namespace fp = fplus;
namespace rg = ranges;
namespace rv = ranges::views;

namespace ns_graph = celaeno::graph;
namespace ns_draw = celaeno::graph::draw;
namespace ns_ops = celaeno::graph::operations;
namespace ns_heuristics = celaeno::heuristics;
namespace ns_io = celaeno::graph::io::verilog;
namespace ns_search = celaeno::graph::search;
namespace ns_views = celaeno::graph::views;
// }}}

// Aliases {{{
using Sink = std::shared_ptr<spdlog::logger>;
using Ops = ns_graph::Ops;
using Node = i64;
using Nodes = std::vector<Node>;
using Edge = std::pair<Node,Node>;
using Edges = std::vector<Edge>;
using Weight = i64;
using Distance = i64;
using Distances = std::set<i64>;
using PairsInterCycles = std::vector<std::pair<Nodes,Nodes>>;
using Annotations = std::map<Node,Distances>;
using MNodeMAnnotations = std::map<Node,Annotations>;
using MEdgeWeight = std::map<Edge,Weight>;
using Tile = std::pair<i64,i64>;
using Tiles = std::vector<Tile>;
using Paths = std::map<std::pair<Node,Node>,std::deque<Tile>>;
using Placement = std::map<Node,Tile>;
using Base = std::vector<Node>;
using Basis = std::vector<Base>;
using Location = celaeno::err::Location;
// }}}

// fn: f_timer {{{
template<typename F, typename... Args>
auto f_timer(Location const& loc, F&& f, Args&&... args)
{
  if constexpr(std::is_void_v<std::invoke_result_t<F, Args...>>)
  {
#ifdef DEBUG
    fmt::print("[exe] {}\n", loc.get());
    auto start {std::chrono::system_clock::now()};
    std::invoke(std::forward<F>(f), std::forward<Args>(args)...);
    auto end {std::chrono::system_clock::now()};
    std::chrono::duration<f64> dur {end-start};
    fmt::print("[end] {} took {} seconds\n", loc.get(), dur.count());
    return void();
#else
    std::invoke(std::forward<F>(f), std::forward<Args>(args)...);
    return void();
#endif
  }
  else
  {
#ifdef DEBUG
    fmt::print("[exe] {}\n", loc.get());
    auto start {std::chrono::system_clock::now()};
    auto result = std::invoke(std::forward<F>(f), std::forward<Args>(args)...);
    auto end {std::chrono::system_clock::now()};
    std::chrono::duration<f64> dur {end-start};
    fmt::print("[end] {} took {} seconds\n", loc.get(), dur.count());
    return result;
#else
    return std::invoke(std::forward<F>(f), std::forward<Args>(args)...);
#endif
  }
}; // fn: f_timer }}}

// fn: lowest_node_id {{{
i64 lowest_node_id(Ops const& ops)
{
  // Dummy vertex with lowest value
  i64 idx{};

  // Define function to compare values lt 0
  auto f_lowest = [&idx](auto&& e) { if(e < idx){ idx=e; } return false; };

  // Get the dummy vertex with the lowest value
  ns_search::bfs::run(0,ops.preds,ops.succs,f_lowest);

  return --idx;
} // function: lowest_node_id }}}

// fn: e_bfs {{{
template<typename F = std::function<bool(Edge)>>
[[nodiscard]] Annotations e_bfs(Ops const& ops
  , Node r
  , F&& f = [](Edge) -> bool { return false; }
)
  requires Returns<bool,F,Edge>
{
  // Empty node queue
  std::queue<Node> q;

  // Push initial element
  q.push(r);

  // Visited Nodes
  std::set<Node> vn;

  // Visited edges
  std::set<Edge> ve;

  // Annotations
  Annotations h;

  // Push initial distance
  h[r] = {0};

  while (! q.empty())
  {
    // Get next node from queue front
    Node u{q.front()}; q.pop();

    // If node has been visited, skip iteration
    if( vn.contains(u) ){ continue; }

    // Else visit node
    vn.insert(u);

    // Helper to retrieve neighboring vertices of a vertex 'v'
    auto neighbors = [&](Node v){ return fn(ops.preds(v)).chain(ops.succs(v)).vec(); };

    // Helper to check if a set of edges has been visited
    auto contains = []<Range R, typename... E>(R&& r, E&&... e)
      requires IsPairsOf<Node,E...> // Edges must be node pairs
    {
      return (r.contains(e) or ...);
    };

    // Retrieve neighbors of 'u', remove ones that form visited edges
    auto targets{fn(neighbors(u))
      .keep([&](Node v){ return ! contains(ve, Edge{u,v}, Edge{v,u}); })
      .vec()
    };

    // Iterate throught target edge nodes
    bool stop{false};
    for (auto v : targets)
    {
      // Apply callback
      stop = f(Edge{u,v})? true : stop;
      // Inherit distances from u to v
      rg::for_each(h[u],[&,v=v](auto d){ h[v].insert(d+1); });
      // Visit edge
      ve.emplace(u,v);
      // Enqueue v if not visited
      if( ! vn.contains(v) ){ q.push(v); }
    } // for

    if( stop ){ break; }

  } // while: ! q.empty()

  return h;
} // function: e_bfs }}}

// fn: minimal_basis_2 {{{
template<SignedIntegral I, typename C>
[[nodiscard]] decltype(auto) minimal_basis_2(I root, Ops const& ops, C&& m_crossing_nodes, Sink sink)
{
  // Init logger
  err::Logger logger{sink};

  std::vector<Nodes> out;

  auto depth_view = ns_views::depth::run(root, ops.preds, ops.succs);

  std::set<Node> s_visited_global;

  for (auto [layer,nodes] : depth_view.ln)
  {
    // Check for nodes with predecessors == 2 (possible cicle endpoints)
    fmt::print("l: {} n: {}\n", layer, nodes);

    nodes = fn(nodes).keep([&](auto n){ return ops.preds(n).size() == 2; }).vec();

    fmt::print("closes: {}\n", nodes);

    for(Node n : nodes)
    {
      fmt::print("n: {}\n", n);
      // Traverse predecessor tree in bfs manner
      std::set<Node> s_visited_local;
      s_visited_local.insert(n);
      std::map<Node,Node> m_cycle;
      ns_search::bfs::run(n
          // Do not backtrack previously visited nodes
        , [&](Node n){ return ops.preds(n); }
          // Only go backwards in the directed graph
        , [](Node){ return std::vector<Node>{}; }
          // Main
        , [&](Node o)
        {
          // Skip source node
          if ( o == n ) { return false; }

          // Add node to visited set
          s_visited_local.insert(o);

          // Get parents of current node
          auto v_parents = fn(ops.succs(o)).in(s_visited_local).vec();

          // Update cycle map
          fn(v_parents).ply([&](auto e){ m_cycle[o] = e; });

          // Check if node has 2 parents in the visited set (possible cycle)
          if ( v_parents.size() == 2 )
          {
            // Check if cycle closes in 'n'
            Nodes v_fst_half;
            Nodes v_snd_half;

            // Create path from map
            auto f_make_path = [&](Node n)
            {
              Nodes out;
              out.push_back(n);
              while ( m_cycle.contains(n) )
              {
                n = m_cycle.at(n);
                out.push_back(n);
              }
              return out;
            };

            // Create path from outputs of 'o'
            v_fst_half = f_make_path(v_parents.at(0));
            v_snd_half = f_make_path(v_parents.at(1));

            // Closes in 'n'
            if( fn(v_fst_half).in(v_snd_half).vec().size() == 1 )
            {
              // Build cycle
              Nodes cycle;

              // Insert lower divergence
              cycle.push_back(o);

              // Insert cycle halves
              v_snd_half.pop_back();
              std::reverse(v_snd_half.begin(), v_snd_half.end());

              cycle = fn(cycle)
                .chain(v_fst_half)
                .chain(v_snd_half)
                .vec();

              fmt::print("cycle: {} → {}\n", o, cycle);
              out.push_back(cycle);
              // Break
              return true;
            }

            // Continue
            return false;
          }
          // Continue
          return false;
        }
      );
    }

  } // for

  exit(0);

  return out;

} // fn: minimal_basis_2 }}}

// fn: minimal_basis {{{
template<SignedIntegral I, typename C>
[[nodiscard]] decltype(auto) minimal_basis(I root, Ops const& ops, C&& m_crossing_nodes, Sink sink)
{
  // Init logger
  err::Logger logger{sink};

  // Start interpreter
  py::scoped_interpreter guard{};

  // Import networkx
  py::module_ networkx = py::module_::import("networkx");

  // Create networkx graph
  auto gnx{networkx.attr("Graph")()};

  // Insert edges
  auto bfs = ns_search::bfs::run(root,ops.preds,ops.succs,
    [&](Node u)
    {
      for( Node v : ops.succs(u) )
      {
        gnx.attr("add_edge")(u,v);
      }
      return false;
    });

  // Calculate basis
  auto basis {py::cast<std::vector<std::vector<Node>>>(networkx.attr("minimum_cycle_basis")(gnx))};

  // Adjust each base, such that, for [u,v], u is a predecessor of v
  for(auto& base : basis)
  {
    std::vector<Node> out;

    auto f_succs = [&](Node u){ return fn(ops.succs(u)).in(base).vec(); };

    // Get node with two outgoing edges that belong to base
    auto it{rg::find_if(base,[&](Node u){ return f_succs(u).size() == 2; })};

    err::err({ it != base.end() })("Outgoing base not found");

    // Include node with two outgoing edges in result
    out.push_back(*it);

    // Get both successors
    auto succs {f_succs(*it)};

    // For each create paths in out
    auto succ_fst{succs.front()};
    auto succ_snd{succs.back()};

    out.push_back(succ_fst);

    for(succs = f_succs(succ_fst); succs.size() != 0; succs = f_succs(succ_fst))
    {
      succ_fst = succs.front();
      out.push_back(succ_fst);
    }

    out.insert(out.begin(), succ_snd);

    for(succs = f_succs(succ_snd); succs.size() != 0; succs = f_succs(succ_snd))
    {
      succ_snd = succs.front();
      out.insert(out.begin(),succ_snd);
    }

    out.pop_back();

    base = out;
  }

  for (u64 i{}; auto e : basis)
  {
    logger.info()("Basis {}: {}", i++, e);
  } // for

  return basis;
} // fn: minimal_basis }}}

// fn: minimal_basis_bfs_ordering {{{
void minimal_basis_bfs_ordering(Basis& basis)
{
  // Take first base, use it as a starting point
  Basis::iterator cut{basis.begin()};

  // Partition the vector into adjacencies of cur
  for(auto it{basis.begin()}; it != basis.end(); ++it)
  {
    cut = std::partition(cut
      , basis.end()
      , [&](Base const& b) { return fn(b).in(*it).vec().size() >= 2; });
  }
} // function: minimal_basis_bfs_ordering }}}

// fn: align_intersections {{{
template<Range R>
decltype(auto) align_intersections(R&& minimal_basis, Sink sink)
{
  // Init logger
  err::Logger logger{sink};

  err::err({ ! minimal_basis.empty() })("Minimal basis must not be empty");

  err::err({ minimal_basis.size() > 1 })("Minimal basis must contain at least two cycles");

  PairsInterCycles out;

  std::queue<Nodes> q, q_restore;

  auto c1{minimal_basis.front()};

  logger.info()("c1: {}", c1);

  i64 i{};

  for (auto it{minimal_basis.begin()}; it != std::prev(minimal_basis.end()); ++it)
  {
    auto c2{*std::next(it)};

    auto intersection{fn(c1).in(c2).sort().unique().vec()};

    // Log data
    // Handle first element
    if( i == 0 )
    {
      err::err({ ! intersection.empty() })
        ("Intersection between first two elements must not be empty");

      err::err({ intersection.size() == 2 })("Intersection must have size of 2");

      // Remove last element of cycle, if it equas the first
      if( c1.front() == c1.back() ){ c1.pop_back(); }

      // Rotate elements until first two are the intersection
      c1 = fn(c1)
        .rot([&](auto v)
          {
            return (v.at(0) == intersection.at(0) && v.at(1) == intersection.at(1))
              or (v.at(0) == intersection.at(1) && v.at(1) == intersection.at(0));
          })
        .vec();

      logger.info()("Rotated c1: {}", c1);

      out.emplace_back(intersection,c1);
    } // if

    logger.info()("c2: {}", c2);

    while( intersection.empty() or intersection.size() < 2 )
    {
      // Check if there are more cycles to intersect with
      if( q.empty() ){ err::err()("No intersection for incident cycle"); }

      // Update c1
      c1 = q.front(); q.pop();

      q_restore.push(c1);

      logger.info()("Trying new c1: {}", c1);

      // Try to form intersection
      intersection = fn(c1).in(c2).sort().unique().vec();

      logger.info()("new_c1 ∩ c2: {}", intersection);

    } // while

    while( ! q_restore.empty() )
    {
      q.push(q_restore.front()); q_restore.pop();
    } // while

    logger.info()("c1 ∩ c2: {}", intersection);

    q.push(c2);

    // Remove last element of cycle, if it equals the first
    if( c2.front() == c2.back() ){ c2.pop_back(); }

    c2 = fn(c2)
      .rot([&](auto v)
        {
          return (v.at(0) == intersection.at(0) && v.at(1) == intersection.at(1))
            or (v.at(0) == intersection.at(1) && v.at(1) == intersection.at(0));
        })
      .vec();

    logger.info()("Rotated c2: {}", c2);

    out.emplace_back(intersection,c2);

    ++i;
  } // for

  return out;
} // fn: align_intersections }}}

// fn: edge_weights {{{
[[nodiscard]] std::map<Edge,Weight>
  edge_weights(Ops const& ops, Range auto&& cycle, auto&& view)
{
  // Map of graph edges and weights
  std::map<Edge,Weight> out;

  // Calculate weight for each node in intersection
  for (auto u : cycle )
  {
    // Add weight for placed neighbors of u and v
    for (auto v : fn(ops.preds(u)).chain(ops.succs(u)).vec())
    {
      auto weight {fp::abs_diff(view.at(u),view.at(v))};
      out.emplace(Edge{u,v},weight);
      out.emplace(Edge{v,u},weight);
    } // for
  } // for

  return out;
} // function: edge_weights }}}

// enum: Partition {{{
enum class Partition
{
  L,
  R,
}; // enum: Partition }}}

// struct: Adjacencies {{{
struct Adjacencies
{
  Tile src;
  i64 dist_exact;
  Tiles adjacencies;

  template<typename T>
  Adjacencies(T&& src, i64 dist_exact);
  Tiles tiles();
};

template<typename T>
Adjacencies::Adjacencies(T&& src, i64 dist_exact)
  : src(src)
  , dist_exact(dist_exact)
  , adjacencies({})
{};

Tiles Adjacencies::tiles()
{
  auto f_tiles_adjacent = [](Tile t) -> Tiles
  {
    return Tiles{
        Tile{t.first-1,t.second}
      , Tile{t.first+1,t.second}
      , Tile{t.first,t.second-1}
      , Tile{t.first,t.second+1}
      , Tile{t.first-1,t.second+1}
      , Tile{t.first+1,t.second-1}
      , Tile{t.first-1,t.second-1}
      , Tile{t.first+1,t.second+1}
    };
  };

  // Do a bfs to find all tiles with distance 'dist_exact' from src
  // Stop when a tile with distance > dist_exact is found, as all the following are increasingly more
  // distant
  (void) ns_search::bfs::run(
      this->src
    , f_tiles_adjacent
    , [](Tile t){ return Tiles{}; }
    , [&](Tile t)
      {
        auto dist_cur{ns_heuristics::chebyshev::run(this->src,t)};

        if( dist_cur > this->dist_exact+1 )
        {
          return true; // stop
        } // if

        if( dist_cur == this->dist_exact )
        {
          this->adjacencies.push_back(t);
        } // else if

        return false; // continue
      }
  );

  return this->adjacencies;
}

// struct: Adjacencies }}}

// fn: place_intersection {{{
auto place_intersection(Range auto intersection)
{
  // Initialize new placement
  Placement placement;

  // Check if intersection is empty
  err::err({! intersection.empty()})("Intersection must not be empty");

  // Intersection must have size less or eq to 2
  err::err({intersection.size() <= 2})("Intersection size must be two or less");

  // Place single-node intersection
  if( intersection.size() == 1 )
  {
    auto u{intersection.at(0)};
    placement[u] = std::make_pair(0,0);
    return placement;
  } // if

  // Update placement
  // for (i64 i{}; auto u : intersection)
  // {
  //   placement[u] = std::make_pair(i,--i);
  // } // for

  rg::reverse(intersection);
  placement[intersection.front()] = std::make_pair(0,0);
  placement[intersection.back()] = std::make_pair(1,-1);
  // if

  return placement;
} // function: place_intersection }}}

// fn: place_cycle {{{
cppcoro::generator<std::pair<Placement,Paths>> place_cycle(Ops const& ops
  , Placement p
  , Paths paths
  , Range auto cycle
  , Range auto inter
  , MEdgeWeight m_edge_weight
  , Partition r
  , Sink sink)
{
  // Init logger
  err::Logger logger{sink};

  // Get lowest node id
  static i64 lowest{lowest_node_id(ops)};

  // Save old state of p
  Placement p_backup{p};

  // Log
  {
    [[maybe_unused]] auto fold{logger.fold()};

    logger.info()("Placement:");
    for (auto e : p)
    {
      logger.info()("{}", e);
    } // for
  }

  // Check if a tile is occupied
  auto f_is_free = [&](Tile const& t){ return ! fn(p).val().has(t); };

  // TODO: Replace this O(n^2) function
  auto path{fp::nub(cycle)};

  // It is desirable to start from the intersection nodes in the cycle,
  // therefore:
  // - Find first occurrence of a node from inter in path
  // - Shift the path left, until the intersection elements are the first/last ones
  // - Remove intersection elements
  // - Create subrange
  Nodes slice {fn(path).rot(fn(path).find_first_of(inter)).dif(inter).vec()};

  // Log
  logger.info()("Cycle: {}", slice);
  logger.info()("Slice: {}", slice);

  // Keep a map with a list of possible tiles, for backtracking
  std::map<Node,Tiles> m_backtrack;

  // Keep track of unplaced and placed elements
  std::stack<Node> placed, unplaced;

  // Push all nodes to the stack
  fn(slice).ply([&](Node v){ if( ! p.contains(v) ){ unplaced.push(v); }  }).discard();

  // Check if cycle was already placed
  if(unplaced.empty()){ co_yield std::make_pair(p,paths); }

  bool stop{false};

  // Wire tiles
  std::unordered_map<Node,std::unordered_map<Node,Tile>> wires_of;

  while( ! unplaced.empty() )
  {
    [[maybe_unused]] auto fold{logger.fold()};

    // Save lowest & highest x coords
    std::pair<i64,i64> coords_low = p.begin()->second;
    std::pair<i64,i64> coords_high = p.begin()->second;
    fn(p).ply([&](auto const& e)
    {
      auto const& tile = e.second;
      if ( tile.first < coords_low.first ) { coords_low.first = tile.first; }
      if ( tile.second < coords_low.second ) { coords_low.second = tile.second; }
      if ( tile.first > coords_high.first ) { coords_high.first = tile.first; }
      if ( tile.second > coords_high.second ) { coords_high.second = tile.second; }
    });

    // Get current node
    auto u{unplaced.top()}; unplaced.pop();

    // Log
    logger.info()("u: {}", u);

    // Skip if placed
    if( p.contains(u) ){ continue; }

    // Get all neighbors
    auto f_neighbors = [&](Node v){ return fn(ops.preds(v)).chain(ops.succs(v)).vec(); };

    // Filter nodes that are not positioned
    Nodes neighbors_positioned{fn(f_neighbors(u)).in(p).vec()};

    // Log
    logger.info()("Nodes placed: {}", neighbors_positioned);

    // Get all possible positions adjacent to positions of neighbors
    Tiles candidates;

    if( ! m_backtrack.contains(u) )
    {
      auto f_get_candidates = [&](Tile const& t, i64 dist) { return Adjacencies{t,dist}.tiles(); };

      // Get all candidate positions from previously positioned neighbors
      // Remove occupied positions
      candidates = fn(neighbors_positioned)
        // .as([&](Node v){ return f_get_candidates(p[v],1); }) // nodes → tiles
        .as([&](Node v){ return f_get_candidates(p[v], m_edge_weight.at({u,v})); }) // nodes → tiles
        .squash() // merge [[tiles],[tiles]...] → [tiles]
        .drop([&](Tile t){ return ! f_is_free(t); })
        .sort()
        .unique()
        .vec();

      logger.info()("condidates initial: {}", candidates);

      // Remove candidates that are behind positioned input nodes
      candidates = fn(candidates).keep([&](Tile const& t)
      {
        for(Node const& n : neighbors_positioned)
        {
          Tile const& tn{p[n]};
          if( fn(ops.preds(u)).has(n) )
          {
            if( tn.second < t.second ) { return false; }
          }  // if
          else
          {
            if( tn.second > t.second ) { return false; }
          } // else
        }
        return true;
      }).vec();

      // For each tile in candidates, remove all that does not adhere to edge
      // constraints.
      auto f_target = [&](Node u, Node v) { return m_edge_weight.at({u,v}); };
      auto f_dist = [&](Tile a, Tile b) { return ns_heuristics::chebyshev::run(a,b); };

      // Keep a candidate if it satisfies edges constraints to all its placed
      // neighbors
      candidates = fn(candidates)
        .in_all(neighbors_positioned
          , [&](Tile t, Node v) { return f_target(u,v) == f_dist(t,p.at(v)); })
        .vec();

      logger.info()("condidates filtered by edges: {}", candidates);

      // Filter candidates by A*
      candidates = fn(candidates)
        .keep([&](Tile t)
        {
          for (auto v : neighbors_positioned)
          {
            // Calculate chebyshev distance
            auto d_chebyshev{f_dist(t,p.at(v))};
            // Calculate chebyshev-based A* path
            auto d_astar{ns_search::a_star::run(t
              , p.at(v)
              , [&](Tile dest) -> Tiles { return f_get_candidates(dest,1); }
              , [&](Tile dest) -> bool { return fn(p).val().has(dest); }
              , [](Tile t1, Tile t2){ return ns_heuristics::chebyshev::run(t1,t2); }
            )};
            // Log
            if( d_astar )
            {
              logger.info()("Tile {} Node {} - Neighbor {} - A*: {}", t, u, v, *d_astar);
            } // if
            // Check if path exists, and it is eq to chebyshev
            if( ! d_astar || static_cast<size_t>(d_chebyshev+1) != d_astar->size() )
            {
              return false;
            } // if
          } // for

          return true;
        })
        .vec();

      // Remove duplicate positions in candidates
      candidates = fn(candidates).sort().unique().vec();

      // Sort candidates by distance
      candidates = fn(candidates).sort({}, [&](Tile const& t)
      {
        // +x right
        // +y down

        // +x
        if ( t.first > coords_high.first )
        {
          // +y = Bottom right corner
          if ( t.second > coords_high.second )
          {
            return celaeno::heuristics::chebyshev::run(t, coords_high);
          }
          // -y = Top right corner
          if ( t.second < coords_low.second )
          {
            return celaeno::heuristics::chebyshev::run(t, Tile{coords_high.first, coords_low.second});
          }
          return std::abs(t.first - coords_high.first);
        }

        // -x
        if ( t.first < coords_low.first )
        {
          // +y Bottom left corner
          if ( t.second > coords_high.second )
          {
            return celaeno::heuristics::chebyshev::run(t, Tile{coords_low.first, coords_high.second});
          }
          // -y Top left corner
          if ( t.second < coords_low.second )
          {
            return celaeno::heuristics::chebyshev::run(t, coords_low);
          }
          return std::abs(coords_low.first - t.first);
        }

        // // only +y
        // if ( t.second > coords_high.second )
        // {
        //   return std::abs(t.second - coords_high.second);
        // }
        //
        // // only -y
        // if ( t.second < coords_low.second )
        // {
        //   return std::abs(coords_low.second - t.second);
        // }

        // Inside
        return i64{};
      }).vec();

    } // if
    else
    {
      candidates = m_backtrack.at(u);
    } // else

    // Log
    logger.info()("Candidates: {}", candidates);

    // Check if candidates are empty, if so, backtrack
    if (candidates.empty())
    {
      // Revert changes made by u
      // Move it back to unplaced stack
      unplaced.push(u);
      // Remove its positions in m_backtrack, this is due to them being
      // invalidated, since it is based on neighbors positions, which will now
      // change
      if( m_backtrack.contains(u) ){ m_backtrack.erase(u); }
      // Try reverse path
      if( placed.empty() )
      {
        break;
      } // if
      // Remove previous node from placed stack
      auto v{placed.top()}; placed.pop();
      // Remove wires of the previous node from placed stack
      for (auto&& [node,tile] : wires_of[v])
      {
        p.erase(node);
      } // for
      wires_of[v].clear();
      // Remove previous node from placement map
      if( p.contains(v) ){ p.erase(v); }
      // Include previous node in unplaced stack
      unplaced.push(v);
      continue;
    } // if

    // Get most promissing position
    Tile chosen{candidates.front()};

    // Block paths between u and candidate
    for (auto v : neighbors_positioned)
    {
      // Candidates
      auto f_get_candidates = [&](Tile const& t, i64 dist) { return Adjacencies{t,dist}.tiles(); };
      // Distance
      auto f_dist = [&](Tile a, Tile b) { return ns_heuristics::chebyshev::run(a,b); };
      // Calculate chebyshev distance
      auto d_chebyshev{f_dist(chosen,p.at(v))};
      // Calculate chebyshev-based A* path
      auto d_astar{ns_search::a_star::run(chosen
          , p.at(v)
          , [&](Tile dest) -> Tiles { return f_get_candidates(dest,1); }
          , [&](Tile dest) -> bool { return fn(p).val().has(dest); }
          , [](Tile t1, Tile t2){ return ns_heuristics::chebyshev::run(t1,t2); }
          )};

      err::err({d_astar != std::nullopt})("A* second run failed");

      // Check if path is reversed or not
      if( fn(ops.succs(u)).has(v) )
      {
        paths[{u,v}] = *d_astar;
      } // if
      else
      {
        paths[{v,u}] = fn(*d_astar).rev().deque();
      } // else

      d_astar->pop_front();
      d_astar->pop_back();

      for (Tile tile : *d_astar)
      {
        wires_of[u][--lowest] = tile;
      } // for
    } // for

    for (auto&& [node,tile] : wires_of[u])
    {
      p[node] = tile;
    } // for
    // if

    // Save node u in it
    p[u] = candidates.front();

    // Erase used position
    candidates.erase(candidates.begin());

    // Save other positions to backtracking map
    m_backtrack[u] = candidates;

    // Update placed stack
    placed.push(u);

    if( unplaced.empty() )
    {
      co_yield std::make_pair(p,paths);

      // Remove previous node from placed stack
      auto v{placed.top()}; placed.pop();

      // Remove wires
      for (auto&& [node,tile] : wires_of[v])
      {
        p.erase(node);
      } // for
      wires_of[v].clear();

      // Remove previous node from placement map
      if( p.contains(v) ){ p.erase(v); }

      // Include previous node in unplaced stack
      unplaced.push(v);
    } // if
  } // while

  // fmt::print("Reached partial solution\n");

} // function: place_cycle }}}

// fn: prox_view {{{
template<typename T>
decltype(auto) get_prox_view(Ops const& ops, T const& view)
{
  T out{view};

  std::queue<Node> q;

  for (auto [u,depth] : view)
  {
    // Get succs
    Nodes succs{ops.succs(u)};

    if( succs.empty() ){ continue; }

    // Check if smallest successor has a dist > 1
    auto v{fn(succs).as([&](Node v){ return std::make_pair(view.at(v),v); }).min().second};

    // If dist of v to u is gt than 1, make it 1
    if( auto depth_v{view.at(v)}; depth_v > 1 )
    {
      out.at(u) = depth_v - 1;

      // Enqueue predecessors of u
      for( auto w : ops.preds(u) ){ q.push(w); }
    } // if
  } // for

  while( ! q.empty() )
  {
    Node u{q.front()}; q.pop();

    // Get succs
    Nodes succs{ops.succs(u)};

    if( succs.empty() ){ continue; }

    // Check if smallest successor has a dist > 1
    auto v{fn(succs).as([&](Node v){ return std::make_pair(out.at(v),v); }).min().second};

    // If dist of v to u is gt than 1, make it 1
    if( auto depth_v{out.at(v)}; depth_v > 1 )
    {
      out.at(u) = depth_v - 1;

      // Enqueue predecessors of u
      for( auto w : ops.preds(u) ){ q.push(w); }
    } // if

  } // While

  return out;

} // fn: prox_view }}}

// fn: detect_ears {{{
template<Range R>
decltype(auto) detect_ears(Ops const& ops, R const& basis)
{
  std::vector<Nodes> out{basis.begin(), basis.end()};

  for (auto it{out.begin()}; it != out.end(); ++it)
  {
    auto base{*it};

    auto intersections = fn(out)
      .keep([&](auto&& e){ return e != base; })
      .as([&](auto&& e) { return std::make_pair(fn(e).in(base).vec(), e); })
      .keep([&](auto&& e){ return e.first.size() > 1; })
      .vec();

    // fmt::print("Cycle: {}\n", base);

    if ( intersections.size() > 1 )
    {
      auto largest = fn(intersections).max({}, [](auto e){ return e.first.size(); });
      // for (auto&& e : intersections) { fmt::print("Intersected: {}\n", e); } // for
      // fmt::print("Largest: {}\n", largest);

      bool is_subsets = fn(intersections)
        .all([&](auto e){ return fn(e.first).in(largest.first).vec().size() > 1; });

      if ( is_subsets )
      {
        // fmt::print("Ear: {}\n", base);
        // Largest cycle
        auto largest_cycle = largest.second;
        // fmt::print("Larger cycle: {}\n", largest_cycle);
        // Largest intersection
        auto largest_inter = largest.first;
        // fmt::print("Larger inter: {}\n", largest_inter);
        // Base intersection
        auto base_inter = fn(base).in(largest.first).rot([&](auto v){ return v.front() == largest_inter.front(); }).vec();
        // fmt::print("Base inter: {}\n", base_inter);
        // Save position of it
        auto it_largest = std::find(out.begin(), out.end(), largest_cycle);
        err::err({it_largest != out.end()})("Could not find cycle in basis");
        // Align intersection
        largest_cycle = fn(largest_cycle).rot([&](auto&& v){ return v.front() == largest_inter.front(); }).vec();
        // fmt::print("Aligned Larger cycle: {}\n", largest_cycle);
        // Subtract the intersection
        largest_cycle = fn(largest_cycle).dif(fn(largest_inter).pop_front(1).pop_back(1).vec()).vec();
        // fmt::print("Subtracted with largest_int: {}\n", largest_cycle);
        // Add the dif with ear
        if ( base_inter == largest_inter )
        {
          largest_cycle = fn(largest_cycle).rot(1).chain_front(fn(base).dif(base_inter).rev().vec()).vec();
        }
        else
        {
          largest_cycle = fn(largest_cycle).rot(1).chain_front(fn(base).dif(base_inter).vec()).vec();
        } // else
        // fmt::print("Dif with ear: {}\n", largest_cycle);
        // Replace cycle
        *it_largest = largest_cycle;
      }
    }
  } // for
  
  for (auto& base : out)
  {
    base = fn(base).rot([&](auto&& v){ return fn(ops.preds(v.front())).in(v).vec().size() == 2; }).vec();
  } // for
  
  return out;

} // function: detect_ears }}}

// fn: global_backtracking {{{
template<typename C>
decltype(auto) global_backtracking(Ops const& ops, C&& m_crossing_nodes, Sink sink)
{
  err::Logger logger{sink};

  // Solution
  Placement placement;
  Paths paths;

  // Get minimal basis
  auto basis =
    f_timer({}, [&] { return minimal_basis_2(i64{},ops,m_crossing_nodes,logger.sink()); });

  basis = f_timer({}, [&] { return detect_ears(ops, basis); });

  for (auto&& base : basis)
  {
    // fmt::print("-- Base: {}\n", base);
  } // for

  f_timer({}, [&] { minimal_basis_bfs_ordering(basis); });

  // Process into adjacent intersection
  for (i32 i{}; auto const& base : basis)
  {
    logger.info()("-- Base {}: {}", i++, base);
  } // for

  auto pair_intersection_basis =
    f_timer({}, [&] { return align_intersections(basis,logger.sink()); });

  // Process into adjacent intersection
  for (auto const& base : pair_intersection_basis)
  {
    logger.info()("-- Int-Base: {}", base);
  } // for

  // Calculate graph depth-view
  auto depth_view =
    f_timer({}, [&] { return ns_views::depth::run(0,ops.preds,ops.succs).nl; });

  auto prox_view =
    f_timer({}, [&] { return get_prox_view(ops,depth_view); });

  // Get edge weights for each basis
  MEdgeWeight m_edge_weight;
  for( auto const& cycle : basis )
  {
    auto m_cycle_edge_weight {edge_weights(ops, cycle, prox_view)};
    fn(m_cycle_edge_weight).ply([&](auto e)
    {
      m_edge_weight[e.first] = e.second;
    });
  }

  // Log
  for (auto e : m_edge_weight)
  {
    logger.info()("Weight: {}", e);
  } // for


  // for (auto e : prox_view)
  // {
  //   fmt::print("prox e: {}\n", e);
  // } // for

  f_timer({}, [&] { std::reverse(pair_intersection_basis.begin(), pair_intersection_basis.end()); });

  // Get initial intersection
  auto const& [intersection,cycle] = pair_intersection_basis.back();

  // Place initial intersection
  placement =
    f_timer({}, [&] { return place_intersection(intersection); });

  // Stack generators
  std::stack<cppcoro::generator<std::pair<Placement,Paths>>> st_generator;

  // Stack of processed solutions
  std::stack<std::pair<Nodes,Nodes>> st_solutions;

  // Push intersection
  if(auto [u,v] = std::make_pair(intersection.front(),intersection.back());
    fn(ops.succs(u)).has(v) )
  {
    paths[{u,v}] = std::deque<Tile>({placement.at(intersection.front()), placement.at(intersection.back())});
  } // if
  else
  {
    paths[{v,u}] = std::deque<Tile>({placement.at(intersection.back()), placement.at(intersection.front())});
  } // else

  bool b_backtrack{false};

  // Save lowest id to remove path-blocking nodes inserted by cycle placement
  // algorithm
  i64 id_lowest{lowest_node_id(ops)};

  i64 i{};

  // Loop until a solution is found for every incident cycle
  while ( ! pair_intersection_basis.empty() )
  {
    // Get next pair intersection/placement
    Nodes intersection;
    Nodes cycle;

    if( ! b_backtrack )
    {
      intersection = pair_intersection_basis.back().first;
      cycle = pair_intersection_basis.back().second;
      pair_intersection_basis.pop_back();
    } // if
    else
    {
      err::err({ ! st_solutions.empty() })("Solution not found");

      err::err({ st_solutions.size() == st_generator.size() })
        ("Number of solutions differ from number of generators");

      auto e{st_solutions.top()}; st_solutions.pop();

      intersection = e.first;
      cycle = e.second;
    } // else

    cppcoro::generator<std::pair<Placement,Paths>> generator;

    if( ! b_backtrack )
    {
      // Create generator
      generator = f_timer({}, [&]
      {
        return place_cycle(ops
            , placement
            , paths
            , cycle
            , intersection
            , m_edge_weight
            , Partition::L
            , logger.sink()
          );
      });
    } // if
    else
    {
      // Get previous generator
      generator = std::move(st_generator.top()); st_generator.pop();

      // Disable backtracking
      b_backtrack = false;
    } // else

    // If a solution is found from current placement
    // - Update final solution
    // - Push generator to backtracking stack
    // - Push incident cycle to backtracking stack
    if( auto it{generator.begin()}; it != generator.end() )
    {
      // fmt::print("Draw of {}\n", i);

      placement = it->first;
      paths = it->second;

      if ( ! placement.empty() )
      {
        Placement _placement {placement};
        Paths _paths {paths};

        // Offset coordinates to remove negative values
        auto x_min{rg::min_element(_placement,{},[](auto e){ return e.second.first; })->second.first};
        auto y_min{rg::min_element(_placement,{},[](auto e){ return e.second.second; })->second.second};

        for (auto& [n,p] : _placement)
        {
          auto& [x,y] = p;

          x += std::abs(x_min);
          y += std::abs(y_min);
        } // for

        // Expand grid to solve crossings
        for (auto& [n,p] : _placement)
        {
          auto& [x,y] = p;

          x *= 2;
          y *= 2;
        } // for

        for(auto& [pair,path] : _paths)
        {
          for(Tile& tile : path)
          {
            tile.first += std::abs(x_min);
            tile.second += std::abs(y_min);
            tile.first *= 2;
            tile.second *= 2;
          }
        }

        // Draw
#ifdef DEBUG
        logger.info()("-- Start draw");
        ns_draw::svg::svg(ops
            , fmt::format("out/step-{}.svg", ++i)
            , _placement
            , _paths
            , [](auto e){ return e; }
            );
        logger.info()("-- End draw");
#endif
      }

      st_generator.push(std::move(generator));
      st_solutions.push(std::make_pair(intersection,cycle));

      // // Draw
      // ns_draw::svg::svg(ops,fmt::format("steps/{}-1.svg", i)
      //   , Placement{placement}
      //   , std::vector<std::vector<i64>>{}
      //   , [](auto e){ return e; }
      // );

      logger.info()("-- Success for:");
      for (auto e : cycle)
      {
        logger.info()("e: {}", e);
      } // for

      logger.info()("-- Placement:");
      for (auto e : placement)
      {
        logger.info()("e: {}", e);
      } // for

    } // if
    // Else if a solution is not found
    // - Push the incident basis onto unfinished stack
    // - Enable backtracking
    else
    {
      pair_intersection_basis.push_back(std::make_pair(intersection,cycle));
      b_backtrack = true;
      logger.info()("-- Failed for:");
      for (auto e : cycle)
      {
        logger.info()("e: {}", e);
      } // for

    } // else

  } // while


  // bool result{place(ops,placement,depth_view,i,p,Partition::L)};
  std::erase_if(placement,[&](auto&& e){ return e.first < id_lowest; });

  return std::make_pair(placement,paths);

} // fn: global_backtracking }}}

// fun: main  {{{
int main([[maybe_unused]] int argc, char const* argv[])
{
  // Read graph
  ns_graph::Graph<i64> g;
  auto emplace = [&g](auto&& e) -> void { g.emplace(e); };

  auto metadata {ns_io::Reader{argv[1],emplace}};

  // Helpers
  auto f_p = [&g](auto v){ return g.predecessors(v); };
  auto f_s = [&g](auto v){ return g.successors(v); };
  auto f_a = [&g](auto u, auto v){ return g.adjacent(u,v); };
  auto f_l = [&g](auto u, auto v){ g.emplace(std::make_pair(u,v)); };
  auto f_u = [&g](auto u, auto v){ g.erase(std::make_pair(u,v)); };

  err::Logger logger;


  // Create ops
  Ops ops(f_p, f_s, f_a, f_l, f_u);

  auto f_call_writer = [&]<typename... Args>(Args&&... args)
  {
    ns_io::Writer(std::forward<Args>(args)...);
  };

  f_timer({}, f_call_writer, metadata.data(), f_p, f_s, "out/0-out.v");

  f_timer({}, [&]{ celaeno::graph::operations::balance::outgoing::run(0,ops); });

  f_timer({}, f_call_writer, metadata.data(), f_p, f_s, "out/1-out.v");

  f_timer({}, [&]{ celaeno::graph::operations::balance::paths::run(0,ops); });

  f_timer({}, f_call_writer, metadata.data(), f_p, f_s, "out/2-out.v");

  auto m_crossing_nodes = f_timer({}, [&]{ return celaeno::graph::operations::balance::crossings::run(0,ops); });

  f_timer({}, f_call_writer, metadata.data(), f_p, f_s, "out/3-out.v");

  // celaeno::graph::operations::balance::paths::run(0,ops);
  //
  // f_timer({}, f_call_writer, metadata.data(), f_p, f_s, "out/4-out.v");
  //
  // // Get minimal basis
  // auto basis = f_timer({}, [&] { return minimal_basis(i64{},ops,m_crossing_nodes,logger.sink()); });
  // basis = f_timer({}, [&] { return detect_ears(ops, basis); });
  //
  // // Include additional edges for cycles with size > 4
  // for(auto const& base : basis)
  // {
  //   if (base.size() > 4)
  //   {
  //     auto c1 = fn(base)
  //       .take_while(
  //         [&](auto n){ return fn(ops.succs(n)).in(base).vec().size() < 2; })
  //       .rev()
  //       .vec();
  //
  //     auto c2 = fn(base).dif(c1).vec();
  //
  //     c1.pop_back();
  //     c2.erase(c2.begin());
  //
  //     fmt::print("Base: {}\n", base);
  //     fmt::print("c1: {}\n", c1);
  //     fmt::print("c2: {}\n", c2);
  //
  //     // Insert additional edges
  //     // // Check which yields most cuts (c1 → c2 or c2 → c1)
  //     auto f_degree_out = [&](Node n) { return ops.succs(n).size(); };
  //     auto f_degree_in = [&](Node n) { return ops.preds(n).size(); };
  //     auto f_degree = [&](Node n) { return f_degree_out(n) + f_degree_in(n); };
  //     auto cuts_c1 = fn(c1)
  //       .zip(fn(c2).pop_front(1).vec())
  //       .keep([&](auto e){ return f_degree_out(e.first) < 2 && f_degree_in(e.second) < 2; })
  //       .vec();
  //     auto cuts_c2 = fn(c2)
  //       .zip(fn(c1).pop_front(1).vec())
  //       .keep([&](auto e){ return f_degree_out(e.first) < 2 && f_degree_in(e.second) < 2; })
  //       .vec();
  //     fmt::print("c1|c2: {}\n", cuts_c1);
  //     fmt::print("c1|c2: {}\n", cuts_c2);
  //
  //     if ( cuts_c1.empty() && cuts_c2.empty() ) { continue; }
  //
  //     auto cuts = ( cuts_c1.size() > cuts_c2.size() )? cuts_c1 : cuts_c2;
  //
  //     fn(cuts).ply([&](auto c){ ops.link(c.first, c.second); });
  //
  //     // :
  //   } // if base.size() > 4
  // }
  //
  // // Process into adjacent intersection
  // for (i32 i{}; auto const& base : basis)
  // {
  //   logger.info()("-- Base {}: {}", i++, base);
  // } // for
  //
  // f_timer({}, f_call_writer, metadata.data(), f_p, f_s, "out/5-out.v");

  // exit(0);


  // unbalance(0,ops);

  // ns_io::Writer(metadata.data(), f_p, f_s, "out/3-out.v");

  // celaeno::graph::operations::balance::paths::run(0,ops);
  //
  // ns_io::Writer(metadata.data(), f_p, f_s, "4-out.v");
  //
  // auto layers{ns_ops::minimize::crossings::run(0, ops.preds, ops.succs, ops.adj, ops.link, ops.unlink)};

  //
  // ns_io::Writer(metadata.data(), f_p, f_s, "4-out.v");

  // fmt::print("Graph:\n");
  // for (auto e : g.data())
  // {
  //   fmt::print("{}\n", e);
  // } // for

  // Perform placement
  // std::cerr << "Started computation\n";
  auto start {std::chrono::system_clock::now()};
  auto [placement, routing] {global_backtracking(ops,m_crossing_nodes,logger.sink())};
  auto end {std::chrono::system_clock::now()};
  // std::cerr << "Finished computation\n";
  std::chrono::duration<f64> dur {end-start};
  std::stringstream ss; ss << dur.count();

  fmt::print("Circuit: {}\n", argv[1]);
  fmt::print("Time(s): {}\n", ss.str());

  // Offset coordinates to remove negative values
  auto x_min{rg::min_element(placement,{},[](auto e){ return e.second.first; })->second.first};
  auto y_min{rg::min_element(placement,{},[](auto e){ return e.second.second; })->second.second};

  for (auto& [n,p] : placement)
  {
    auto& [x,y] = p;

    x += std::abs(x_min);
    y += std::abs(y_min);
  } // for

  // Expand grid to solve crossings
  for (auto& [n,p] : placement)
  {
    auto& [x,y] = p;

    x *= 2;
    y *= 2;
  } // for

  for(auto& [pair,path] : routing)
  {
    for(Tile& tile : path)
    {
      tile.first += std::abs(x_min);
      tile.second += std::abs(y_min);
      tile.first *= 2;
      tile.second *= 2;
    }
  }

  // fmt::print("\n------\n");
  // for( auto&& e : placement ){ fmt::print("{}\n", e); }
  // fmt::print("------\n");

  // Print area
  std::pair<int,int> pair_min = placement.begin()->second;
  std::pair<int,int> pair_max = placement.begin()->second;
  fn(placement).ply([&](auto const& e)
  {
    auto const& tile = e.second;
    if ( tile.first < pair_min.first ) { pair_min.first = tile.first; }
    if ( tile.second < pair_min.second ) { pair_min.second = tile.second; }
    if ( tile.first > pair_max.first ) { pair_max.first = tile.first; }
    if ( tile.second > pair_max.second ) { pair_max.second = tile.second; }
  });

  fmt::print("Area: {}x{}\n"
    , std::abs(pair_max.first - pair_min.first)
    , std::abs(pair_max.second - pair_min.second)
  );

  // Draw
  ns_draw::svg::svg(ops
    , "out/out.svg"
    , placement
    , routing
    , [](auto i){ return i; }
  );


  return EXIT_SUCCESS;
} // main }}}
