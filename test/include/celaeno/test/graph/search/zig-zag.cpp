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

#include <fmt/core.h>
#include <fplus/fplus.hpp>
#include <range/v3/all.hpp>
#include <spdlog/spdlog.h>

#include <celaeno/aliases.hpp>
#include <celaeno/concepts.hpp>
#include <celaeno/err/err.hpp>
#include <celaeno/fun/fun.hpp>

#include <celaeno/graph/graph.hpp>
#include <celaeno/graph/reader/verilog.hpp>

#include <celaeno/graph/views/depth.hpp>

#include <celaeno/graph/draw/svg.hpp>
#include <celaeno/graph/representations/grid.hpp>

#include <celaeno/graph/search/a-star.hpp>
#include <celaeno/graph/search/bfs.hpp>
#include <celaeno/graph/search/zig-zag.hpp>

#include <celaeno/heuristics/chebyshev.hpp>
#include <celaeno/heuristics/manhattan.hpp>

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
namespace ns_grid = celaeno::graph::representations::grid;
namespace ns_heuristics = celaeno::heuristics;
namespace ns_reader = celaeno::graph::reader::verilog;
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

// fn: edges_to_nodes {{{
Nodes edges_to_nodes(Edges const& edges)
{
  Nodes nodes;

  for( auto&& e : edges ){ nodes.push_back(e.first); }

  nodes.push_back(edges.back().second);

  return nodes;
} // fn: edges_to_nodes }}}

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

  while( ! q.empty() )
  {
    auto e{q.front()}; q.pop();

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

    auto edges = fn(f_adjacent(e)).keep([&](Edge e) { return ! v_e.contains(e); }).vec();

    for( auto f : edges ){ q.push(f); }

  } // while

 return out;

} // fn: p_bfs }}}

// fn: minimal_basis {{{
template<SignedIntegral I>
[[nodiscard]] decltype(auto) minimal_basis(I root, Ops const& ops)
{
  // Initial path
  Edges ipath;

  // Initial cycle
  Cycles zz_c;

  // Run zig-zag until it founds a cycle
  auto zz_o{ns_search::zig_zag::run(root,ops,ipath,zz_c
    , [&](auto){ return ! zz_c.empty(); }
  )};

  // Get first incident cycle
  auto inodes{fn(ipath)
    .as([](Edge e){ return e.first; })
    .chain(Nodes{ipath.back().second})
    .unique()
    .vec()
  };

  // Initial nodes
  inodes = incident_cycle(inodes);

  err::info()("inodes: {}", inodes);

  // Initial edges
  ipath = fp::overlapping_pairs(inodes);

  // Keep track of visited edges
  std::set<Edge> visited;

  // Check if an edge is in visited set
  auto f_contains =
  [&]<typename... E>(E&&... e) -> bool
  {
    return (visited.contains(e) || ...);
  };

  // Get neighboring edges, given a node u
  auto f_neighbors = [&](Node u) -> Nodes
  {
    return fn(ops.preds(u))
      .chain(ops.succs(u))
      .vec();
  };

  // Return the degree of a node
  auto f_degree =
  [&](Node u) -> size_t
  {
    return fn(ops.preds(u)).chain<Nodes>(ops.succs(u)).size();
  };

  // Given an edge [u,v], if there are edges adjacent of u and v, such that they
  // are not in visited set, return them
  auto f_not_in_adjacent = [&]<typename F>(Node u, F f) -> Edges
  {
    auto edges{fn(f(u))
      .as([u=u](Node w){ return Edge{u,w}; })
      .keep([&](Edge f){ return ! f_contains(f); })
      .vec()
    };

    return fn(edges).keep(
      [&](Edge e){ return (f_degree(e.first) > 1) && (f_degree(e.second) > 1); }
    )
    .vec();
  };

  // Return adjacent edges if both endpoints of edge 'e', has adjacent edges such
  // that they were not yet visited
  auto f_adjacent = [&](Edge e) -> Edges
  {
    auto r1{f_not_in_adjacent(e.first,f_neighbors)};

    // if( r1.empty() ){ return {}; }

    auto r2{f_not_in_adjacent(e.second,f_neighbors)};

    // if( r2.empty() ){ return {}; }

    // Filter out ones in visited
    // Filter out ones with leaves
    return fn(r1)
      .chain(r2)
      .dif(visited)
      .keep([&](Edge e){ return (f_degree(e.first) > 1) && (f_degree(e.second) > 1); })
      .vec();
  };

  // Remove edge from visited set
  auto f_rm_visited = [&](Edge e)
  {
    visited.erase(e);
    visited.erase(Edge{e.second,e.first});
  };

  // Add edge to visited set
  auto f_add_visited = [&](Edge e)
  {
    visited.insert(e);
    visited.insert(Edge{e.second,e.first});
  };

  // Conditionally visit edges, returns unvisited ones
  auto f_visit = [&](Edges const& edges) -> Edges
  {
    // Visit all edges
    (void) fn(edges).ply([&](Edge e){ f_add_visited(e); });

    // Save unvisited
    Edges unvisited;

    // Given an edge uv, save unvisited adjacent edges uw,vx
    for( Edge e : edges )
    {
      auto adjacent{f_adjacent(e)};

      fmt::print("1. e: {} Adjacent: {}\n", e, adjacent);

      fmt::print("2. e: {} Adjacent: {}\n", e, adjacent);

      // At least one endpoint had no unvisited neighbors
      if( adjacent.size() == 0 ){ continue; }

      // If both endpoints of e, uv, have unvisited edges, then e is unvisited
      if( fn(adjacent).unique([&](Edge e1, Edge e2){ return e1.first == e2.first; }).vec().size() > 2 )
      {
        f_rm_visited(e);
        unvisited.push_back(e);
      } // if

      // Save unvisited adjacent edges
      for( Edge f : adjacent ){ unvisited.push_back(f); }
    } // for

    return unvisited;
  };

  // Use a queue to define the order to detect adjacent cycles
  std::deque<Edge> q;

  // Minimal basis paths
  std::vector<std::vector<Nodes>> basis;

  // Verify when to switch to next basis vector
  std::set<Edge> v_edges;

  // Push unvisited edges to queue
  for( auto e : f_visit(ipath) )
  {
    err::info()("Unvisited {}\n", e);
    q.push_back(e);
  }

  // Current head of subsolution
  Nodes head{inodes};

  // Next heads of subsolutions
  std::queue<Nodes> q_heads;

  // Keep searching for cycles while q is not empty
  while( ! q.empty() )
  {
    // Search next cycle from edge e[u,v]
    auto e{q.front()}; q.pop_front();

    if( visited.contains(e) ){ continue; }

    err::info()("Search from {} → {}\n", e.first, e.second);

    // Create pairs to check novel edges for unvisited neighbors
    auto nodes{p_bfs(e,[&](Edge e){ return f_adjacent(e); })};

    // Include in solution
    // If no intersection is found between current path and head, make previous
    // path the new head and add nodes
    if( fn(fp::nub(nodes)).in(fp::nub(head)).vec().size() < 2 )
    {
      do
      {
        if( q_heads.empty() ){ break; }
        head = q_heads.front(); q_heads.pop();
      } // do
      while( fn(fp::nub(nodes)).in(fp::nub(head)).vec().size() < 2 );

      basis.push_back({head});

      basis.rbegin()->push_back(nodes);

      q_heads.push(nodes);
    } // if
    else
    {
      q_heads.push(nodes);
      basis.rbegin()->push_back(nodes);
    } // else

    // Mark cycle as visited
    Edges next{f_visit(fp::overlapping_pairs(nodes))};

    // Enqueue unvisited edges
    for( auto f : next ){ q.push_back(f); v_edges.insert(f); }

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
    : b_reversed{false}
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

// fn: global_backtracking {{{
decltype(auto) global_backtracking(Ops const& ops)
{
  // Solution
  Placement placement;

  // Get minimal basis
  auto basis{minimal_basis(i64{},ops)};

  for (auto const& base : basis)
  {
    fmt::print("-- Base: {}\n", base);
  } // for

  // Calculate graph depth-view
  auto depth_view{ns_views::depth::run(0,ops.preds,ops.succs).nl};

  // Maintain a stacks of unfinished and finished sub-basis
  std::stack<std::pair<Nodes,Nodes>> unfinished, finished;

  // Maintain a stack of previous states of placed cycles
  std::stack<State> stack_states;

  // Maintain a stack of previous solutions
  std::stack<Placement> stack_placement;

  // Push initial empty solution to stack
  stack_placement.push(placement);

  // Move values to unfinished
  for (auto it1{basis.begin()}; it1 != basis.end(); ++it1)
  {
    for (auto it2{std::next(it1->begin())}; it2 != it1->end(); ++it2)
    {
      // Calculate intersection with head cycle
      Nodes intersection{fn(fn(it1->at(0)).in(*it2).vec()).sort().unique().vec()};
      // Place intersection and cycle in stack
      unfinished.push(std::make_pair(intersection, *it2));
    } // for
  } // for

  // Reverse unfinished stack
  {
    std::stack<std::pair<Nodes,Nodes>> stack_rev;
    while( ! unfinished.empty() ){ stack_rev.push(unfinished.top()); unfinished.pop(); }
    unfinished = stack_rev;
  }

  bool b_backtracking{false};

  while( ! unfinished.empty() )
  {
    if( placement.empty() )
    {
      auto fst{basis.at(0).at(0)};
      auto snd{basis.at(0).at(1)};

      auto intersection{fn(fn(fst).in<Nodes>(snd)).sort().unique<Nodes>()};

      placement = place_intersection(intersection, b_backtracking);

      State state;

      auto result{place(ops,placement,depth_view,intersection,fst,Partition::R,state)};

      if( ! result )
      {
        err::err()("Failure to find a feasible solution at first incident cycle");
      } // if
    } // if

    // Get next element
    auto sub{unfinished.top()};

    // Get intersection
    auto const& [i,p] = std::tie(sub.first,sub.second);

    // Generate a new state for possible backtracking
    State state;

    if( b_backtracking )
    {
      state = stack_states.top();
      stack_states.pop();
      state.b_backtracking = true;
      placement = stack_placement.top();
      stack_placement.pop();
    } // if
    else
    {
      stack_placement.push(placement);
    } // else

    // Perform placement based on intersection
    auto result{place(ops,placement,depth_view,i,p,Partition::L,state)};

    // If operation was successful
    // - Include top of unfinished stack in finished stack
    // - Pop unfinished stack
    // - Add state to stack of states
    // - Set backtracking flag to false
    if( result )
    {
      finished.push(sub);
      unfinished.pop();
      stack_states.push(std::move(state));
      b_backtracking = false;
    } // if
    // Otherwise, backtrack
    // - Pop current placement from stack
    // - Include top of finished stack in unfinished
    // - Pop finished stack
    // - Set b_backtracking to true
    else
    {
      err::err({ ! finished.empty() })("Failure to find a feasible solution");
      stack_placement.pop();
      unfinished.push(finished.top());
      finished.pop();
      b_backtracking = true;
    } // else
  } // while

  return placement;
} // function: global_backtracking

// fn: global_backtracking }}}

// fun: main  {{{
int main([[maybe_unused]] int argc, char const* argv[])
{
  // Read graph
  ns_graph::Graph<i64> g;
  auto emplace = [&g](auto&& e) -> void { g.emplace(e); };
  auto metadata {ns_reader::Reader{argv[1],emplace}};

  // Helpers
  auto f_p = [&g](auto v){ return g.predecessors(v); };
  auto f_s = [&g](auto v){ return g.successors(v); };
  auto f_a = [&g](auto u, auto v){ return g.adjacent(u,v); };
  auto f_l = [&g](auto e){ g.emplace(e); };
  auto f_u = [&g](auto e){ g.erase(e); };

  // Create ops
  Ops ops(f_p, f_s, f_a, f_l, f_u);

  fmt::print("Graph:\n");
  for (auto const& e : g.data())
  {
    fmt::print("-- {}\n", e);
  } // for

  // Perform placement
  Placement placement{global_backtracking(ops)};

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
  ns_draw::svg::svg(fmt::format("{}.svg",argv[1])
    , placement
    , std::vector<std::vector<i64>>{}
    , [](auto i){ return i; }
  );


  return EXIT_SUCCESS;
} // main }}}
