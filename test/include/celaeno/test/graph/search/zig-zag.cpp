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

#include "unbalance.hpp"

// TODO Remove
#include <celaeno/graph/operations/balance/outgoing.hpp>
#include <celaeno/graph/operations/count/crossings.hpp>

// Using namespace {{{
using namespace celaeno::fun::fn;
using namespace celaeno::concepts;
using namespace celaeno::aliases;
// }}}

// namespaces {{{
namespace err = celaeno::err;
namespace fp = fplus;
namespace rg = ranges;

namespace ns_graph = celaeno::graph;
namespace ns_draw = celaeno::graph::draw;
namespace ns_ops = celaeno::graph::operations;
namespace ns_heuristics = celaeno::heuristics;
namespace ns_io = celaeno::graph::io::verilog;
namespace ns_search = celaeno::graph::search;
namespace ns_views = celaeno::graph::views;
// }}}

// Aliases {{{
using Ops = ns_graph::Ops;
using Node = i64;
using Nodes = std::vector<Node>;
using Edge = std::pair<Node,Node>;
using Edges = std::vector<Edge>;
using Weight = i64;
using Distance = i64;
using Distances = std::set<i64>;
using Annotations = std::map<Node,Distances>;
using MNodeMAnnotations = std::map<Node,Annotations>;
using MEdgeWeight = std::map<Edge,Weight>;
using Tile = std::pair<i64,i64>;
using Tiles = std::vector<Tile>;
using GraphPath = std::deque<Node>;
using GraphPaths = std::vector<GraphPath>;
using GridPath = std::vector<std::pair<Node,Node>>;
using Cycles = std::vector<Node>;
using Placement = std::map<Node,Tile>;
using Occupation = std::set<Tile>;
// }}}

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
    auto targets{fp::keep_if(
      [&](Node v){ return ! contains(ve, Edge{u,v}, Edge{v,u}); }, neighbors(u)
    )};

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

// fn: incident_cycle {{{
decltype(auto) incident_cycle(Nodes const& nodes)
{
  // Sort with nlog(n) time
  Nodes sorted{fn(Nodes{nodes}).sort().vec()};

  Nodes delimiters{};

  // If two adjacent elements are equal, they are cycle delimiters
  for( auto e : fn(sorted).sliding(2).view )
  {
    if( e.at(0) == e.at(1) )
    {
      delimiters.push_back(e.at(0));
    } // if
  } // for

  // Get cycle size for each node that is a cycle delimiter
  auto delimiters_size{fn(delimiters)
    .as<std::vector<std::pair<Node,size_t>>>([&](Node u)
    {
      auto it_beg{std::ranges::find(nodes,u)};
      auto it_end{std::ranges::find(std::next(it_beg),nodes.end(),u)};

      err::err({ it_beg != nodes.end(), it_end != nodes.end() })("Delimiter range error");

      return std::make_pair(u,std::distance(it_beg,it_end));
    })
  };

  // Sort by size
  // Get delimiter with smallest distance, which is an incident cycle
  auto u{fn(delimiters_size)
    .sort({},[](auto e){ return e.second; })
    .first()
    .as([](auto e){ return e.first; })
    .vec()
    .at(0)
  };

  // Get cycle based on delimiter
  auto it_beg{std::ranges::find(nodes,u)};
  auto it_end{std::ranges::find(std::next(it_beg),nodes.end(),u)};

  err::err({ it_beg != nodes.end(), it_end != nodes.end() })("Delimiter range error");

  return Nodes{it_beg,std::next(it_end)};
} // fn: incident_cycle }}}

// fn: map_to_path {{{
Nodes map_to_path(std::map<Node,Node> m, Edge e, Nodes dest)
{
  // Resulting path
  Nodes path;

  // Add first element
  path.emplace_back(e.first);

  // Keep updating current edge, until one of dest is reached
  while( ! fn(dest).has(e.second) )
  {
    // Check if next edge exists
    err::err({ m.contains(e.second) })("Broken path");
    // Go to next edge
    e.first = std::exchange(e.second,m[e.second]);
    // Add next element
    path.emplace_back(e.first);
  } // while

  while ( fn(dest).has(e.second) )
  {
    // Check if next edge exists
    if( ! m.contains(e.second) ){ break; }
    // Go to next edge
    e.first = std::exchange(e.second,m[e.second]);
    // Add next element
    path.emplace_back(e.first);
  } // while

  path.emplace_back(e.second);

  return path;
} // fn: map_to_path }}}

// fn: p_bfs {{{
template<typename F>
Nodes p_bfs(Edge src, F f_adjacent)
{
  auto has_subcycle = [&](Nodes nodes)
  {
    return fn(nodes)
      .sort()
      .sliding(2)
      .count_if([](auto&& e){ return e.at(0) == e.at(1); }) > 1;
  };

  // Result
  Nodes out;

  // Queue of unvisited edges
  std::queue<Edge> q;

  // Enqueue initial edge
  q.push(src);

  // Edges hash table
  std::map<Node,Node> m_e;

  // Visited edges
  std::set<Edge> v_e;

  // Visited destination nodes
  std::set<Node> v_d;

  Edge fst_endpoint{}, snd_endpoint{};

  fmt::print("b_bfs: src: {}\n", src);

  while( ! q.empty() )
  {
    auto e{q.front()}; q.pop();

    fmt::print("b_bfs: edge: {}\n", e);

    if( ! v_e.contains(e) )
    {
      v_e.insert(e);
      v_e.insert(Edge{e.second,e.first});
    } // if
    else
    {
      continue;
    } // else

    if( v_d.contains(e.second) )
    {
      fst_endpoint = Edge{e.second,e.first};

      snd_endpoint = Edge{e.second,m_e.at(e.second)};

      out = fn(map_to_path(m_e,fst_endpoint,Nodes{src.first,src.second}))
        .rev()
        .chain(map_to_path(m_e,snd_endpoint,Nodes{src.first,src.second}))
        .unique()
        .vec();

      if( has_subcycle(out) ){ out = {}; continue; }

      m_e.emplace(e.second,e.first);

      break;
    } // if

    v_d.insert(e.second);

    m_e.emplace(e.second,e.first);

    auto edges {fn(f_adjacent(e)).keep([&](Edge e) { return ! v_e.contains(e); }).vec()};

    fmt::print("b_bfs: adjacent: {}\n", edges);

    for( auto f : edges ){ q.push(f); }

  } // while

 return out;

} // fn: p_bfs }}}

// fn: complete_dummy_edges {{{
template<SignedIntegral I>
void complete_dummy_edges(I root, Ops const& ops)
{
  // Create a node-layer view
  auto [m_layer_nodes, m_node_layer] {ns_views::depth::run(root, ops.preds, ops.succs)};

  auto f_dist_x = [&
    , &m_node_layer=m_node_layer
    , &m_layer_nodes=m_layer_nodes
  ](Node u, Node v)
  {
    // Get layer of u and v
    u64 u_layer{m_node_layer.at(u)};
    u64 v_layer{m_node_layer.at(v)};

    // Get layers of u and v
    auto&& u_rng{m_layer_nodes.at(u_layer)};
    auto&& v_rng{m_layer_nodes.at(v_layer)};

    // Get index of u and v
    auto u_idx{fn(u_rng).count_while([&](Node w){ return w != u; })};
    auto v_idx{fn(v_rng).count_while([&](Node w){ return w != v; })};

    // Adjust to point to u and v
    ++u_idx;
    ++v_idx;

    // Return the absolute diference of indices
    return fp::abs_diff(u_idx,v_idx);
  };

  // Steps to link cycle
  // 1. A node u has a predecessor v
  // 2. Node v has a successor w != u
  // 3. ∄ edge u -- w
  // 4. Link u -- w

  auto f_make_complete = [&](Node u) -> bool
  {
    bool is_modified{false};

    // 1. Get preds of u
    auto u_preds{ops.preds(u)};

    for (auto v : u_preds)
    {
      // 2. Get successors of v
      auto v_succs{ops.succs(v)};

      // 2. Remove u
      v_succs = fn(v_succs).keep([&](Node w){ return w != u; }).vec();

      if( v_succs.empty() ){ continue; }

      // Get the successor closest to u
      v_succs = fn(v_succs).sort({},[&](Node w){ return f_dist_x(w,u); }).vec();

      auto w{v_succs.at(0)};

      // 3. Check if edge u -- w ∄
      if( ! ops.adj(u,w) && ! ops.adj(w,u) )
      {
        is_modified = true;

        // Link u -- w
        ops.link(u,w);

        fmt::print("Inserted edge: {} -- {}\n", u, w);

      } // if

    } // for

    return is_modified;
  };

  // Populate a queue with all nodes
  std::queue<I> q;
  auto kahn{ns_search::kahn::run(root,ops.preds,ops.succs)};
  for( Node u : kahn ){ q.push(u); }

  fmt::print("kahn: {}\n", kahn);

  while( ! q.empty() )
  {
    auto u{q.front()}; q.pop();

    (void) f_make_complete(u);

  } // while

  for (auto nodes : m_layer_nodes)
  {
    fmt::print("Nodes: {}\n", nodes);
  } // for


} // fn: complete_dummy_edges }}}

// fn: minimal_basis {{{
template<SignedIntegral I>
[[nodiscard]] decltype(auto) minimal_basis(I root, Ops const& ops)
{
  // Create logger
  err::Log logger;

  // Initial path
  Edges path;

  // Initial cycle
  Cycles zz_c;

  // Run zig-zag until it founds a cycle
  auto zz_o{ns_search::zig_zag::run(root,ops,path,zz_c
    , [&](auto){ return ! zz_c.empty(); }
  )};

  // Get first incident cycle
  auto nodes{fn(path)
    .as([](Edge e){ return e.first; })
    .chain(Nodes{path.back().second})
    .unique()
    .vec()
  };

  // Initial nodes
  nodes = incident_cycle(nodes);

  // Initial edges
  path = fp::overlapping_pairs(nodes);

  // Log
  logger.info()("nodes: {}", nodes);
  logger.info()("path: {}", path);

  // Keep track of visited edges
  std::set<Edge> visited;

  // Get neighboring edges, given a node u
  auto f_neighbors = [&](Node u) -> Nodes
  {
    return fn(ops.preds(u)).chain(ops.succs(u)).vec();
  };

  // Return the degree of a node
  auto f_in_degree =
  [&](Node u) -> size_t
  {
    return fn(ops.preds(u)).chain<Nodes>(ops.preds(u)).size();
  };

  auto f_out_degree =
  [&](Node u) -> size_t
  {
    return fn(ops.preds(u)).chain<Nodes>(ops.succs(u)).size();
  };

  // Check if a node is leaf
  auto f_node_is_leaf = [&](Node u){ return f_in_degree(u) <= 1 && f_out_degree(u) <= 1; };

  // Check if an edge is leaf
  auto f_edge_is_leaf = [&](Edge e){ return f_node_is_leaf(e.first) or f_node_is_leaf(e.second); };

  // Return adjacent edges if both endpoints of edge 'e', has adjacent edges such
  // that they were not yet visited
  auto f_filter_neighbors = [&](Edge e) -> Edges
  {
    auto [u,v] = e;

    // For u and v, get neighboring edges which are
    // - Unvisited
    // - Not leafs
    // - Not e
    auto r1{fn(f_neighbors(u))
      .as([u=u](Node w){ return Edge{u,w}; })
      .keep([&](Edge f){ return ! visited.contains(f); })
      .keep([&](Edge f){ return ! f_edge_is_leaf(f); })
      .keep([&,v=v,u=u](Edge f){ return f != e && f != Edge{v,u}; })
      .vec()
    };

    auto r2{fn(f_neighbors(v))
      .as([v=v](Node w){ return Edge{v,w}; })
      .keep([&](Edge f){ return ! visited.contains(f); })
      .keep([&](Edge f){ return ! f_edge_is_leaf(f); })
      .keep([&,v=v,u=u](Edge f){ return f != e && f != Edge{v,u}; })
      .vec()
    };

    // If either of [u,v] endpoins are empty, return empty
    if( r1.empty() or r2.empty() )
    {
      return {};
    } // else if

    return fn(r1).chain(r2).vec();
  };

  // Add edge to visited set
  auto f_add_visited = [&](Edge e)
  {
    visited.insert(e);
    visited.insert(Edge{e.second,e.first});
  };

  // Remove edge from visited set
  auto f_rm_visited = [&](Edge e)
  {
    visited.erase(e);
    visited.erase(Edge{e.second,e.first});
  };

  // Conditionally visit edges, returns unvisited ones
  auto f_to_explore = [&](std::set<Edge> const& edges) -> std::set<Edge>
  {
    [[maybe_unused]] auto fold{logger.fold()};

    logger.info()("f_to_explore: Edges: {}", edges);

    // Save unvisited
    std::set<Edge> unvisited;

    // Visit cycle
    for( auto const& e : edges ){ f_add_visited(e); }

    // Check edges to visit
    for( Edge e : edges )
    {
      // Get neighbor edges of both endpoints, which are:
      // - Not visited
      // - Not leaves
      // - not part of current path
      Edges neighbors{fn(f_filter_neighbors(e))
        .keep([&](Edge f){ return ! edges.contains(f) && ! edges.contains(Edge{f.second,f.first}); })
        .vec()
      };

      logger.info()("Neighbors of {}: {}", e, neighbors);

      // At least one endpoint had no unvisited neighbors
      if( neighbors.empty() )
      {
        logger.info()("Marked edge {} as visited", e);
        // Leave current edge e visited,and try next
        continue;
      } // if
      else
      {
        logger.info()("Marked edge {} as unvisited", e);
        // Mark edge to be unvisited
        unvisited.insert(e);
      } // else
    } // for

    // Unvisit edges
    for(auto f : unvisited){ f_rm_visited(f); }

    logger.info()("f_to_explore: unvisited: {}", unvisited);

    return fn(unvisited).set();
  };

  // Use a queue to define the order to detect adjacent cycles
  std::deque<Edge> q;

  // Minimal basis paths
  std::vector<std::vector<Nodes>> basis;

  // Push first cycle to solution
  basis.push_back({nodes});

  for (auto e : path)
  {
    q.push_back(e);
  } // for

  // Mark edges not in queue as visited
  for( auto e : f_to_explore(fn(path).set()) ){ q.push_back(e); }

  // Current head of subsolution
  Nodes head{nodes};

  // Next heads of subsolutions
  std::queue<Nodes> q_heads;

  q_heads.push(nodes);

  logger.info()("-- Initial visited: {}", visited);

  i64 iteration{};

  // Keep searching for cycles while q is not empty
  while( ! q.empty() && iteration < 100)
  {
    // Search next cycle from edge e[u,v]
    auto e{q.front()}; q.pop_front();

    // if( visited.contains(e) ){ continue; }
    // Unvisit current edge
    f_rm_visited(e);

    // Create pairs to check novel edges for unvisited neighbors
    nodes = p_bfs(e,[&](Edge e){ return f_filter_neighbors(e); });

    logger.info()("-- e: {}\n", e);

    logger.info()("-- Visited: {}\n", visited);

    logger.info()("-- adjacent: {}\n", nodes);

    if( nodes.empty() )
    {
      f_add_visited(e);
      continue;
    }

    if( iteration != 0 )
    {
      // Get intersection of old and new cycle
      auto intersection{fn(fp::nub(nodes)).in(fp::nub(head)).set()};

      // Backtrack on q_heads until a cycle intersects with current cycle
      bool backtracked{false};

      while( intersection.size() < 2 )
      {
        backtracked = true;

        err::err({ ! q_heads.empty() })("q_heads must not be empty");

        head = q_heads.front(); q_heads.pop();

        intersection = fn(fp::nub(nodes)).in(fp::nub(head)).set();
      } // while

      if( backtracked )
      {
        basis.push_back({head});
      } // if

      // If the intersection is not an articulation point
      basis.rbegin()->push_back(nodes);
      q_heads.push(nodes);
    } // if
    else
    {
      basis.push_back({nodes});
    } // else

    // Get next edges to explore
    std::set<Edge> next{f_to_explore(fn(fp::overlapping_pairs(nodes)).set())};

    logger.info()("-- Unvisited: {}\n", next);

    // Enqueue unvisited edges
    for( auto f : next ){ q.push_back(f); }

    f_add_visited(e);

    ++iteration;
  } // while

  return basis;
} // fn: minimal_basis }}}

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
  Tile up;
  Tile down;
  Tile left;
  Tile right;

  template<typename T>
  Adjacencies(T&& src);
  Tiles from_left();
  Tiles from_right();
};

template<typename T>
Adjacencies::Adjacencies(T&& src)
  : up   (Tile{src.first   , src.second-1})
  , down (Tile{src.first   , src.second+1})
  , left (Tile{src.first-1 , src.second})
  , right(Tile{src.first+1 , src.second})
{};

Tiles Adjacencies::from_left()
{
  return Tiles{down,left,up,right};
};

Tiles Adjacencies::from_right()
{
  return Tiles{down,right,up,left};
};
// struct: Adjacencies }}}

// fn: place_intersection {{{
auto place_intersection(Range auto intersection, bool backtracking)
{
  // Initialize new placement
  Placement placement;

  // Check if intersection is not empty
  err::err({! intersection.empty()})("Intersection must not be empty");

  // Placement must be empty
  err::err({ placement.empty()})("Placement must be empty");

  // Place single-node intersection
  if( intersection.size() == 1 )
  {
    auto u{intersection.at(0)};
    placement[u] = std::make_pair(0,0);
    return placement;
  } // if

  // Update placement
  if( ! backtracking )
  {
    for (i64 i{}; auto u : intersection)
    {
      placement[u] = std::make_pair(0,i++);
    } // for
  } // if
  else
  {
    for (i64 i{}; auto u : intersection)
    {
      placement[u] = std::make_pair(0,i++);
    } // for
  } // else

  return placement;
} // function: place_intersection }}}

// struct: State {{{
struct State
{
  // Keep the slice the cycle was previously executed on
  Nodes slice;
  // Keep a map with a list of possible tiles, for backtracking
  std::map<Node,Tiles> m_backtrack;
  // Keep track of unplaced and placed elements
  std::stack<Node> placed, unplaced;
  // Keep track if reversed path was attempted
  bool b_reversed;
  // Check if is initialized
  bool b_backtracking;

  State()
    : slice()
    , m_backtrack()
    , placed()
    , unplaced()
    , b_reversed{false}
    , b_backtracking{false}
  {}
};

using State = struct State;
// struct: State }}}

// fn: place_cycle {{{
bool place_cycle(Ops const& ops
  , Placement& p
  , Range auto&& cycle
  , Range auto&& inter
  , MNodeMAnnotations const& m_node_m_a
  , MEdgeWeight m_edge_weight
  , Partition r
  , State& state)
{
  // Save old state of p
  Placement p_backup{p};

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
  Nodes& slice = state.slice;

  // Avoid recomputation if slice is already saved in state
  if( ! state.b_backtracking )
  {
    slice = fn(path).rot(fn(path).find_first_of(inter)).dif(inter).vec();
  } // if

  // Keep a map with a list of possible tiles, for backtracking
  std::map<Node,Tiles>& m_backtrack = state.m_backtrack;

  // Keep track of unplaced and placed elements
  std::stack<Node>& placed = state.placed;
  std::stack<Node>& unplaced = state.unplaced;

  // Try with reverse path if fails
  bool& b_reversed = state.b_reversed;

  // If not backtracking, push all nodes to the stack
  if( ! state.b_backtracking )
  {
    fn(slice).ply([&](Node v){ if( ! p.contains(v) ){ unplaced.push(v); }  });
  } // if
  // Otherwise
  // - Remove top of placed from placement map
  // - Push top of placed to unplaced
  // - Pop top of placed
  else
  {
    Node u{placed.top()};
    p.erase(u);
    unplaced.push(u);
    placed.pop();
  } // else

  // Helper to reverse path if current fails
  auto f_try_reverse_path = [&]
  {
    m_backtrack.clear();
    unplaced = std::stack<Node>{};
    placed = std::stack<Node>{};
    b_reversed = true;
    p = p_backup;
    fn(slice).rev().ply([&](Node v){ unplaced.push(v); });
  };

  while( ! unplaced.empty() )
  {
    // Get current node
    auto u{unplaced.top()}; unplaced.pop();

    // Skip if placed
    if( p.contains(u) ){ continue; }

    // Get all neighbors
    auto f_neighbors = [&](Node v) { return fn(ops.preds(v)).chain(ops.succs(v)).vec(); };

    // Filter nodes that are not positioned
    Nodes nodes_placed{fn(f_neighbors(u)).in(p).vec()};

    // Get all possible positions adjacent to positions of neighbors
    Tiles candidates;

    if( ! m_backtrack.contains(u) )
    {

      using F = typename std::function<Tiles(Tile)>;

      auto f_get_candidates = ( r == Partition::L )?
          F([&](Tile const& t) { return Adjacencies{t}.from_left(); })
       :  F([&](Tile const& t) { return Adjacencies{t}.from_right(); });

      // Get all candidate positions from previously positioned neighbors
      // Remove occupied positions
      candidates = fn(nodes_placed)
        .as([&](Node v){ return f_get_candidates(p[v]); }) // nodes → tiles
        .squash() // merge [[tiles],[tiles]...] → [tiles]
        .drop([&](Tile t){ return ! f_is_free(t); })
        .vec();

      // Define the quality of a tiles based on annotations
      auto f_quality = [&](Tile t)
      {
        // Count how many distance constraints from intersection it adheres, and
        // use this as a method for sorting best positions
        return - rg::count_if(inter,
        [&](Node v)
        {
          auto&& targets{m_node_m_a.at(v).at(u)};
          auto   dist{ns_heuristics::manhattan::run(t,p[v])};
          return fn(targets).has(dist);
        });
      };

      // Sort candidates by quality
      candidates = fn(candidates).sort({},f_quality).vec();

      // For each tile in candidates, remove all that does not adhere to edge
      // constraints.
      auto f_target = [&](Node u, Node v)
      {
        return ( m_edge_weight.contains({u,v}) )?
            m_edge_weight.at({u,v})
          : m_edge_weight.at({v,u});
      };

      auto f_dist = [&](Tile a, Tile b) { return ns_heuristics::manhattan::run(a,b); };

      // Keep a candidate if it satisfies edges constraints to all its placed
      // neighbors
      candidates = fn(candidates)
        .in_all(nodes_placed
          , [&](Tile t, Node v){ return f_target(u,v) == f_dist(t,p.at(v)); })
        .vec();

      // Filter candidates by A*
      candidates = fn(candidates)
        .keep([&](Tile t)
        {
          for (auto v : nodes_placed)
          {
            // Calculate manhattan distance
            auto d_manhattan{f_dist(t,p.at(v))};
            // Calculate manhattan-based A* path
            auto d_astar{ns_search::a_star::run(t
              , p.at(v)
              , [&](Tile dest) -> Tiles { return f_get_candidates(dest); }
              , [&](Tile dest) -> bool { return fn(p).val().has(dest); }
            )};
            // Check if path exists, and it is eq to manhattan
            if( ! d_astar || static_cast<size_t>(d_manhattan+1) != d_astar->size() )
            {
              return false;
            } // if
          } // for

          return true;
        })
        .vec();

      // Remove duplicate positions in candidates
      candidates = fn(candidates).sort().unique().vec();

      // Sort resulting candidates by smalest chebyshev distance for all
      // intersection nodes
      rg::sort(candidates,{},
      [&](Tile t)
      {
        return fn(inter)
          .as([&](Node v){ return ns_heuristics::chebyshev::run(t,p[v]); })
          .sum();
      });

    } // if
    else
    {
      candidates = m_backtrack.at(u);
    } // else

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
        if( b_reversed ){ return false; }

        f_try_reverse_path(); continue;
      } // if
      // Remove previous node from placed stack
      auto v{placed.top()}; placed.pop();
      // Remove previous node from placement map
      if( p.contains(v) ){ p.erase(v); }
      // Include previous node in unplaced stack
      unplaced.push(v);

      continue;
    } // if

    // Save node u in most promissing position
    p[u] = candidates.front();

    // Erase used position
    candidates.erase(candidates.begin());

    // Save other positions to backtracking map
    m_backtrack[u] = candidates;

    // Update placed stack
    placed.push(u);
  } // while

  return true;
} // function: place_cycle }}}

// fn: place {{{
template<typename View>
bool place(Ops const& ops
  , Placement& placement
  , View const& depth_view
  , Nodes const& intersection
  , Nodes const& cycle
  , Partition partition
  , State& state)
{
  // Annotate Based on intersection
  std::map<Node,Annotations> m_n_m_a;
  for (Node r : intersection)
  {
    auto annotations{e_bfs(ops,r)};
    m_n_m_a[r] = annotations;
  } // for

  // Get edge weights
  auto weights{edge_weights(ops, cycle, depth_view)};

  // Perform placement
  return place_cycle(ops
    , placement
    , cycle
    , intersection
    , m_n_m_a
    , weights
    , partition
    , state
  );
} // fn: place }}}

template<typename C>
decltype(auto) global_backtracking(Ops const& ops, C&& crossings)
{
  // Solution
  Placement placement;

  // Get minimal basis
  auto basis{minimal_basis(i64{},ops)};

  for (auto const& base : basis)
  {
    err::info()("-- Base: {}\n", base);
  } // for

  return placement;

  // // Calculate graph depth-view
  // auto depth_view{ns_views::depth::run(0,ops.preds,ops.succs).nl};
  //
  // std::stack<std::pair<Nodes,Nodes>> unfinished;
  //
  // // Move values to unfinished
  // for (auto it1{basis.begin()}; it1 != basis.end(); ++it1)
  // {
  //   for (auto it2{std::next(it1->begin())}; it2 != it1->end(); ++it2)
  //   {
  //     // Calculate intersection with head cycle
  //     Nodes intersection{fn(fn(it1->at(0)).in(*it2).vec()).sort().unique().vec()};
  //
  //     if( it1 == basis.begin() && it2 == std::next(it1->begin()) )
  //     {
  //       placement = place_intersection(intersection,false);
  //     } // if
  //
  //     // Place intersection and cycle in stack
  //     unfinished.push(std::make_pair(intersection, *it2));
  //   } // for
  // } // for
  //
  // // Reverse unfinished stack
  // {
  //   std::stack<std::pair<Nodes,Nodes>> stack_rev;
  //   while( ! unfinished.empty() ){ stack_rev.push(unfinished.top()); unfinished.pop(); }
  //   unfinished = stack_rev;
  // }
  //
  // while( ! unfinished.empty() )
  // {
  //   State state;
  //
  //   auto [i,p] = unfinished.top(); unfinished.pop();
  //
  //
  //   fmt::print("Placement Before\n");
  //   for (auto p : placement)
  //   {
  //     fmt::print("{}\n", p);
  //   } // for
  //   fmt::print("\n");
  //
  //   bool result{place(ops,placement,depth_view,i,p,Partition::L,state)};
  //
  //   fmt::print("Placement After\n");
  //   for (auto p : placement)
  //   {
  //     fmt::print("{}\n", p);
  //   } // for
  //   fmt::print("\n");
  //
  //
  //   if( ! result ){ err::err()("Failed"); }
  // } // while

  return placement;

} // fn: global_backtracking

// // fn: global_backtracking {{{
// decltype(auto) global_backtracking(Ops const& ops)
// {
//   // Solution
//   Placement placement;
//
//   // Get minimal basis
//   auto basis{minimal_basis(i64{},ops)};
//
//   for (auto const& base : basis)
//   {
//     err::info()("-- Base: {}\n", base);
//   } // for
//
//   // Calculate graph depth-view
//   auto depth_view{ns_views::depth::run(0,ops.preds,ops.succs).nl};
//
//   // Maintain a stacks of unfinished and finished sub-basis
//   std::stack<std::pair<Nodes,Nodes>> unfinished, finished;
//
//   // Maintain a stack of previous states of placed cycles
//   std::stack<State> stack_states;
//
//   // Maintain a stack of previous solutions
//   std::stack<Placement> stack_placement;
//
//   std::stack<std::pair<Placement,State>> stack_placement_state;
//
//   // Move values to unfinished
//   for (auto it1{basis.begin()}; it1 != basis.end(); ++it1)
//   {
//     for (auto it2{std::next(it1->begin())}; it2 != it1->end(); ++it2)
//     {
//       // Calculate intersection with head cycle
//       Nodes intersection{fn(fn(it1->at(0)).in(*it2).vec()).sort().unique().vec()};
//       // Place intersection and cycle in stack
//       unfinished.push(std::make_pair(intersection, *it2));
//     } // for
//   } // for
//
//   // Reverse unfinished stack
//   {
//     std::stack<std::pair<Nodes,Nodes>> stack_rev;
//     while( ! unfinished.empty() ){ stack_rev.push(unfinished.top()); unfinished.pop(); }
//     unfinished = stack_rev;
//   }
//
//   bool b_backtracking{false};
//
//   err::info()("-- Started global backtracking...\n");
//
//   // Place intersection and first cycle
//   {
//     auto fst{basis.at(0).at(0)};
//     auto snd{basis.at(0).at(1)};
//
//     auto intersection{fn(fn(fst).in<Nodes>(snd)).sort().unique<Nodes>()};
//
//     // Push initial cycle
//     unfinished.push(std::make_pair(intersection,fst));
//   } // if
//
//   // Generate a new state for possible backtracking
//   State state;
//
//   // Get next element
//   auto sub{unfinished.top()};
//
//   while( ! unfinished.empty() )
//   {
//     // Get intersection
//     auto const& [i,p] = std::tie(sub.first,sub.second);
//
//     err::info()("-- Started placement...\n");
//
//     err::info()("-- Intersection: {}\n", i);
//     err::info()("-- Nodes to position: {}\n", p);
//
//     fmt::print("Placement Before\n");
//     for (auto p : placement)
//     {
//       fmt::print("{}\n", p);
//     } // for
//     fmt::print("\n");
//
//     if( b_backtracking )
//     {
//       b_backtracking = false;
//       state.b_backtracking = true;
//     } // if
//
//     Placement placement_prev{placement};
//
//     auto print_stack = []<typename S>(S s)
//       requires requires(S t){ t.top(); }
//       && requires(S t){ t.pop(); }
//       && requires(S t){ t.empty(); }
//     {
//       while ( ! s.empty() )
//       {
//         auto e{s.top()}; s.pop();
//         fmt::print("{}, ", e);
//       } // while
//     };
//
//
//     fmt::print("----------\nState prev:\n");
//     fmt::print("b_backtracking: {}\n", state.b_backtracking);
//     fmt::print("b_reversed: {}\n", state.b_reversed);
//     fmt::print("m_backtrack: {}\n", state.m_backtrack);
//     fmt::print("placed: "); print_stack(state.placed);
//     fmt::print("\nunplaced: "); print_stack(state.unplaced);
//     fmt::print("\nslice: {}\n", state.slice);
//     fmt::print("----------\n");
//
//     // Perform placement based on intersection
//     bool result{place(ops,placement,depth_view,i,p,Partition::L,state)};
//
//     fmt::print("----------\nState after:\n");
//     fmt::print("b_backtracking: {}\n", state.b_backtracking);
//     fmt::print("b_reversed: {}\n", state.b_reversed);
//     fmt::print("m_backtrack: {}\n", state.m_backtrack);
//     fmt::print("placed: "); print_stack(state.placed);
//     fmt::print("\nunplaced: "); print_stack(state.unplaced);
//     fmt::print("\nslice: {}\n", state.slice);
//     fmt::print("----------\n");
//
//     err::info()("-- Finished placement...\n");
//
//     if( result )
//     {
//       err::info()("-- Success\n");
//     } // if
//     else
//     {
//       err::info()("-- Failure\n");
//     } // else
//
//
//     fmt::print("Placement After\n");
//     for (auto p : placement)
//     {
//       fmt::print("{}\n", p);
//     } // for
//     fmt::print("\n");
//
//     // If operation was successful
//     // - Include top of unfinished stack in finished stack
//     // - Pop unfinished stack
//     // - Add state to stack of states
//     // - Set backtracking flag to false
//     if( result )
//     {
//       err::err({ ! unfinished.empty() })("Unfinished stack must not be empty!");
//
//       // Save new solution
//       stack_placement_state.push(std::make_pair(placement_prev,state));
//
//       // Push top of unfinished stack to finished stack
//       finished.push(sub);
//
//       // Pop unfinished stack
//       unfinished.pop();
//
//       // Reset state
//       state = {};
//
//       if( ! unfinished.empty() )
//       {
//         // Get next intersection and cycle
//         sub = unfinished.top();
//       } // if
//
//     } // if
//     // Otherwise, backtrack
//     // - Pop current placement from stack
//     // - Include top of finished stack in unfinished
//     // - Pop finished stack
//     else
//     {
//       err::info()("-- Backtracking!!!!!!!!!...\n");
//
//       err::err({ ! stack_placement_state.empty() })("placement_state stack must not be empty!");
//
//       err::err({ ! finished.empty() })("Finished stack must not be empty!");
//
//       // Discard current solution, since new solution from it is not viable
//       stack_placement_state.pop();
//
//       // Get previous placement and state
//       std::tie(placement,state) = stack_placement_state.top();
//
//       // Set state to backtracking mode
//       state.b_backtracking = true;
//
//       // Push top of finished stack top of unfinished stack
//       unfinished.push(finished.top());
//
//       // Pop finished stack
//       finished.pop();
//
//       // Update next element
//       sub = unfinished.top();
//
//       b_backtracking = true;
//
//     } // else
//
//   } // while
//
//   err::info()("-- Finished global backtracking...\n");
//
//   return placement;
// } // function: global_backtracking
//
// // fn: global_backtracking }}}

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

  // Create ops
  Ops ops(f_p, f_s, f_a, f_l, f_u);

  ns_io::Writer(metadata.data(), f_p, f_s, "0-out.v");

  celaeno::graph::operations::balance::outgoing::run(0,ops);

  ns_io::Writer(metadata.data(), f_p, f_s, "1-out.v");

  celaeno::graph::operations::balance::paths::run(0,ops);

  ns_io::Writer(metadata.data(), f_p, f_s, "2-out.v");

  auto crossings{celaeno::graph::operations::balance::crossings::run(0,ops)};

  unbalance(0,ops);

  ns_io::Writer(metadata.data(), f_p, f_s, "3-out.v");

  // celaeno::graph::operations::balance::paths::run(0,ops);
  //
  // ns_io::Writer(metadata.data(), f_p, f_s, "4-out.v");
  //
  // auto layers{ns_ops::minimize::crossings::run(0, ops.preds, ops.succs, ops.adj, ops.link, ops.unlink)};

  //
  // ns_io::Writer(metadata.data(), f_p, f_s, "4-out.v");

  fmt::print("Graph:\n");
  for (auto e : g.data())
  {
    fmt::print("{}\n", e);
  } // for

  // return 0;

  // Perform placement
  // std::cerr << "Started computation\n";
  auto start {std::chrono::system_clock::now()};
  Placement placement {global_backtracking(ops,crossings)};
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


  fmt::print("\n------\n");
  for( auto&& e : placement ){ fmt::print("{}\n", e); }
  fmt::print("------\n");

  // Draw
  // ns_draw::svg::svg(fmt::format("{}.svg",argv[1])
  //   , placement
  //   , std::vector<std::vector<i64>>{}
  //   , [](auto i){ return i; }
  // );


  return EXIT_SUCCESS;
} // main }}}
