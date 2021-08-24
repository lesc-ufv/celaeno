// vim: set expandtab fdm=marker ts=2 sw=2 tw=80 et :
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

#include <fplus/fplus.hpp>
#include <range/v3/all.hpp>
#include <fmt/core.h>
#include <spdlog/spdlog.h>

#include <celaeno/aliases.hpp>
#include <celaeno/concepts.hpp>
#include <celaeno/fun/fun.hpp>
#include <celaeno/err/err.hpp>
#include <celaeno/heuristics/manhattan.hpp>
#include <celaeno/heuristics/chebyshev.hpp>
#include <celaeno/graph/graph.hpp>
#include <celaeno/graph/reader/verilog.hpp>
#include <celaeno/graph/draw/svg.hpp>
#include <celaeno/graph/search/zig-zag.hpp>
#include <celaeno/graph/search/bfs.hpp>

// Using namespace {{{
using namespace celaeno::concepts;
using namespace celaeno::aliases;
// }}}

// namespaces {{{
namespace err = celaeno::err;
namespace fn = celaeno::fun;
namespace fp = fplus;
namespace fw = fplus::fwd;
namespace rg = ranges;
namespace rv = ranges::views;

namespace ns_graph = celaeno::graph;
namespace ns_heuristics = celaeno::heuristics;
namespace ns_reader = celaeno::graph::reader::verilog;
namespace ns_search = celaeno::graph::search;
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
using Tile = std::pair<i64,i64>;
using Tiles = std::vector<Tile>;
using GraphPath = std::deque<Node>;
using GraphPaths = std::vector<GraphPath>;
using GridPath = std::vector<std::pair<Node,Node>>;
using Cycles = std::vector<Node>;
using Placement = std::map<Node,Tile>;
using Occupation = std::set<Tile>;
// }}}

// fn: print_stack {{{
template<typename Stack>
void print_stack(Stack s)
{
  fmt::print("Stack: ");
  while( ! s.empty() )
  {
    auto u{s.top()}; s.pop();
    fmt::print("{} → ", u);
  } // while
  fmt::print("\n");
} // function: print_stack }}}

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
    auto neighbors = [&](Node v){ return fp::append(ops.preds(v),ops.succs(v)); };

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
  Nodes sorted{fn::fn(Nodes{nodes}).sort().into<Nodes>()};

  Nodes delimiters{};

  // If two adjacent elements are equal, they are cycle delimiters
  for( auto e : fn::fn(sorted).sliding(2).view )
  {
    if( e.at(0) == e.at(1) )
    {
      delimiters.push_back(e.at(0));
    } // if
  } // for

  // Get cycle size for each node that is a cycle delimiter
  auto delimiters_size{fn::fn(delimiters)
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
  auto u{fn::fn(delimiters_size)
    .sort({},[](auto e){ return e.second; })
    .first()
    .template as<std::vector<size_t>>([](auto e){ return e.first; })
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
  while( ! fn::fn(dest).has(e.second) )
  {
    // Check if next edge exists
    err::err({ m.contains(e.second) })("Broken path");
    // Go to next edge
    e.first = std::exchange(e.second,m[e.second]);
    // Add next element
    path.emplace_back(e.first);
  } // while

  while ( fn::fn(dest).has(e.second) )
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
      m_e.emplace(e.second,e.first);

      fst_endpoint = Edge{e.second,e.first};

      snd_endpoint = Edge{e.second,m_e.at(e.second)};

      break;
    } // if
    else
    {
      v_d.insert(e.second);
    } // else

    m_e.emplace(e.second,e.first);

    auto edges = fn::fn(f_adjacent(e))
      .template keep<Edges>(
        [&](Edge e)
        {
          return ! v_e.contains(e);
        }
      );

    err::info()("-- Curr: {}\n", e);
    err::info()("-- Edges: {}\n", edges);

    for( auto f : edges ){ q.push(f); }

  } // while

 return fn::fn(map_to_path(m_e,fst_endpoint,Nodes{src.first,src.second}))
    .rev()
    .chain(map_to_path(m_e,snd_endpoint,Nodes{src.first,src.second}))
    .unique<Nodes>();

} // fn: p_bfs }}}

// fn: minimal_basis {{{
template<SignedIntegral I>
[[nodiscard]] decltype(auto) minimal_basis(I root, Ops const& ops)
{
  Edges ipath; // Path
  Cycles zz_c; // Cycles

  // Run zig-zag
  auto zz_o{ns_search::zig_zag::run(root,ops,ipath,zz_c
    , [&](auto){ return ! zz_c.empty(); }
  )};

  fmt::print("-- Smallest found cycle: {}\n", ipath);
  fmt::print("-- Closing node: {}\n", zz_c);

  // Get first incident cycle
  auto inodes{fn::fn(ipath)
    .as([](Edge e){ return e.first; })
    .chain(Nodes{ipath.back().second})
    .template unique<Nodes>()
  };

  fmt::print("-- Nodes: {}\n", inodes);

  inodes = incident_cycle(inodes);
  fmt::print("-- Smallest found cycle: {}\n", inodes);

  ipath = fp::overlapping_pairs(inodes);

  fmt::print("-- Smallest found path: {}\n", ipath);

  // Leaf nodes
  std::set<Node> leaves;

  // Populate leaves set
  (void) ns_search::bfs::run(0,ops,
  [&](auto u)
  {
    if( ops.preds(u).size() == 0 || ops.succs(u).size() == 0 )
    {
      leaves.insert(u);
    } // if
    return false;
  });

  // Check if both endpoints [u,v] are not leaves
  auto f_nin_leaves = [&](Edge e) { return ! fn::fn(leaves).has(e.first,e.second); };

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
    return fn::fn(ops.preds(u))
      .template chain<Nodes>(ops.succs(u));
  };

  // Return the degree of a node
  auto f_degree =
  [&](Node u) -> size_t
  {
    return fn::fn(ops.preds(u)).chain<Nodes>(ops.succs(u)).size();
  };

  // Given an edge [u,v], if there are edges adjacent of u and v, such that they
  // are not in visited set, return them
  auto f_adjacent = [&](Edge e) -> Edges
  {
    auto [u,v] = e;

    auto r1{fn::fn(f_neighbors(u))
      .as([u=u](Node w){ return Edge{u,w}; })
      .template keep<Edges>([&](Edge f){ return ! f_contains(f); })
    };

    // r1 = fn::fn(r1).template keep<Edges>([&](Edge e){ return f_nin_leaves(e); });

    r1 = fn::fn(r1).template keep<Edges>(
      [&](Edge e){ return (f_degree(e.first) > 1) && (f_degree(e.second) > 1); }
    );

    if( r1.empty() ){ return {}; }

    auto r2{fn::fn(f_neighbors(v))
      .as([v=v](Node w){ return Edge{v,w}; })
      .template keep<Edges>([&](Edge f){ return ! f_contains(f); })
    };

    r2 = fn::fn(r2).template keep<Edges>(
      [&](Edge e){ return (f_degree(e.first) > 1) && (f_degree(e.second) > 1); }
    );

    if( r2.empty() ){ return {}; }

    return fn::fn(r1).template chain<Edges>(r2);
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
    err::info()("-- Visiting: {}\n", edges);

    // Visit all edges
    (void) fn::fn(edges).ply([&](Edge e){ f_add_visited(e); });


    // Save unvisited
    Edges unvisited;

    // Unvisit edges with unvisited neighbors in both endpoints
    (void) fn::fn(edges).ply([&](Edge e)
    {
      auto adjacent{f_adjacent(e)};

      // Filter out leaves
      adjacent = fn::fn(adjacent).template keep<Edges>(
        [&](Edge e){ return (f_degree(e.first) > 1) && (f_degree(e.second) > 1); }
      );

      // At least one endpoint had no unvisited neighbors
      if( adjacent.size() == 0 ){ return; }

      // Unvisit edges [u,v] with unvisited neighboring edges
      f_rm_visited(e);
      unvisited.push_back(e);
    });

    return unvisited;
  };

  err::info()("-- Result: {}\n\n", ipath);

  // Use a queue to define the order to detect adjacent cycles
  std::deque<Edge> q;

  // Mark edges not in queue as visited
  // (void) fn::fn(ipath).dif(enqueued).ply([&](Edge e){ visited.insert(e); });
  for( auto e : f_visit(ipath) ){ q.push_back(e); }

  // Minimal basis paths
  std::vector<Edges> basis;

  // Push initial incident cycle to solution
  basis.push_back(ipath);

  // Keep searching for cycles while q is not empty
  while( ! q.empty() )
  {
    // Search next cycle from edge e[u,v]
    auto e{q.front()}; q.pop_front();

    if( visited.contains(e) ){ continue; }

    err::info()("-- e: {}\n", e);

    // Remove edges with unvisited preds or succs > 1
    auto nodes{p_bfs(e,
      [&](Edge e)
      {
        return fn::fn(f_adjacent(e)).keep(
          [&](Edge f)
          {
            // Discover actual direction (input/output) or (output)/(input),
            // reverse if necessary to make it always as (output)/(input)
            f = fn::fn(ops.preds(f.first)).has(f.second)? f : Edge{f.second,f.first};
            // Check valid count of edges for oi direction
            fn::fn(f_adjacent(f))
              .;
          });
      }
    )};


    // // Create pairs to check novel edges for unvisited neighbors
    // auto nodes{p_bfs(e,
    //   [&](Edge e)
    //   {
    //     return fn::fn(f_adjacent(e))
    //       .sort({},[&](Edge f){ return f_adjacent(f).size(); })
    //       .template into<Edges>();
    //   }
    // )};
    //
    // // Create pairs to check novel edges for unvisited neighbors
    // auto nodes{p_bfs(e,[&](Edge e){ return f_adjacent(e); })};

    err::info()("-- Result(n): {}\n", nodes);

    auto op{fp::overlapping_pairs(nodes)};

    // Include in solution
    basis.push_back(op);

    // Mark cycle as visited
    Edges next{f_visit(op)};

    err::info()("-- Next(n): {}\n", next);

    // Enqueue unvisited edges
    for( auto f : next ){ q.push_back(f); }

    fmt::print("\n");

  } // while

  return basis;
} // fn: minimal_basis }}}

// fn: cyclic_paths {{{
//
// Given nodes that are part of cycles
// Returns the paths that compose the cycles
//
[[nodiscard]] GraphPaths cyclic_paths(Ops const& ops, Cycles const& cycles)
{
  // Novel intersection cycles
  GraphPaths graph_paths;

  // Find out the greatest tree
  for (auto r : cycles)
  {
    // Current path
    std::deque<Node> deque_path;

    // Stack to determine the visiting order for nodes
    std::stack<Node> stack_node;

    // Push initial node
    stack_node.push(r);

    // Save edges previously visited to avoid self cycles
    std::stack<Nodes> stack_i;

    // Push initial element for stack_edge
    stack_i.push({r});

    // Keep track of visited Edges
    std::set<Edge> set_edge;

    while( ! stack_node.empty() )
    {
      // Take node of stack top
      Node u{stack_node.top()}; stack_node.pop();

      // Take its adjacent nodes
      auto n {fp::append(ops.preds(u),ops.succs(u))};

      // Take top of stack_i
      auto i{stack_i.top()};

      // pop i whilst u is not contained
      auto contains = []<Range R>(R&& r, Node u){ return rg::find(r,u) != rg::end(r); };

      // Backtrace
      while( ! contains(i,u) )
      {
        // Check for errors
        err::err({! stack_i.empty()})( "Stack must never be empty" );

        stack_i.pop();

        // Backtrace path if went back a level in the tree
        if( i != stack_i.top() ){ deque_path.pop_front(); }

        // Update exclusion set
        i = stack_i.top();

      } // while

      // Include 'u' in path
      deque_path.push_front(u);

      // Check if leaf is the same id as root
      if( ! contains(i,r) && contains(n,r) )
      {
        // Save path
        deque_path.push_front(r);

        // Check elements for equality with reversed container to remove
        // duplicate paths
        bool exists{rg::any_of(graph_paths,
          [&,rv_deque_path = rv::reverse(deque_path)](auto&& e)
          {
            return rg::equal(e,rv_deque_path);
          }
        )};

        // Conditionally include in graph_paths
        if( ! exists ){ graph_paths.emplace_back(deque_path); }

        // Remove cycle marker
        deque_path.pop_front();

        // Traceback to predecessor of 'u'
        deque_path.pop_front();

        // Might need to test current node again
        stack_i.push(i);

        // Finish current processing
        continue;
      } // if

      // Nodes to insert in stack_i
      Nodes nodes_i;

      // Insert u for next stack pop of i
      nodes_i.emplace_back(u);

      // Check if depth increased
      bool next{false};

      // Check for each node in n, if a novel edge may be created
      for (Node v : n)
      {
        if( v == r ){ continue; }

        // Create potetial edge
        auto e{Edge{u,v}};

        // Check if is valid for processing
        if( ( (! set_edge.contains(e)) or contains(cycles,v) ) && ! contains(i,v) )
        {
          next = true;
          // Mark edge as visited
          set_edge.emplace(e);
          // Include v for next previously visited nodes_i
          nodes_i.emplace_back(v);
          // Include v in the nodes stack
          stack_node.push(v);
        } // if
      } // for

      // Update stack_i for new depth
      if( next ){ stack_i.push(nodes_i); }
      // Backtrace if dead-end
      else { deque_path.pop_front(); } // else

    } // while

  } // for


  return graph_paths;
} // function: cyclic_paths }}}

// fn: edge_weights {{{
[[nodiscard]] std::map<Edge,Weight> edge_weight(Ops const& ops,
  Range auto&& nodes,
  std::map<Node,Annotations> const& m_n_m_a)
{
  // Map of graph edges and weights
  std::map<Edge,Weight> out;

  // Perform e_bfs for each node u in nodes
  for (Node u : nodes)
  {
    (void) e_bfs(ops,u,
    [&](Edge e) -> bool
    {
      // Decompose edge
      auto [v,w] = e;

      // Get annotations for u
      auto const& u_annotations{m_n_m_a.at(u)};

      // Take the maximum and minimum values of v and w annotations, and
      // calculate the absolute difference
      auto v_a{u_annotations.at(v)};
      auto w_a{u_annotations.at(w)};

      // Check for errors
      err::err({! v_a.empty(), ! w_a.empty()}) ("v_a and w_a must not be empty");

      // Get max elements of v_a
      auto v_a_max {rg::max_element(v_a)};

      // Get max elements of w_a
      auto w_a_max {rg::max_element(w_a)};

      // Get max between abs diff
      auto diff{fp::abs_diff(*v_a_max,*w_a_max)};

      // If a previous value was assigned to the edge, use min
      auto best{(out.contains(e))? std::min(out.at(e),diff) : diff};

      // Check if reverse edge exists, if so, update it with the maximum between
      // it and current result
      auto e_rev{Edge{e.second,e.first}};

      if(out.contains(e_rev))
      {
        out[e_rev] = std::max(out[e_rev],best);
      } // if
      else
      {
        out[e] = best;
      } // else

      return false;
    }); // e_bfs
  } // for

  return out;
} // function: edge_weight }}}

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
  return Tiles{down,left,up};
};

Tiles Adjacencies::from_right()
{
  return Tiles{down,right,up};
};
// struct: Adjacencies }}}

// fn: place_intersection {{{
[[nodiscard]] Placement place_intersection(Ops const& ops,
  Range auto&& nodes,
  Range auto&& cycles,
  Partition r)
{
  // Positions for nodes nodes
  Placement p;

  // Check if intersection is not empty
  err::err({! nodes.empty()})("Intersection must not be empty");

  // Set predecessor as nodes[0]
  Node pred{nodes.at(0)};

  // Place it in (0,0)
  p[pred] = {0,0};

  // Perform placement of next nodes
  for (auto it{std::next(nodes.begin())}; it != nodes.end(); ++it)
  {
    // Get current node
    auto u{*it};

    // Get adjacent tiles from predecessor position
    Adjacencies adj{p[pred]};

    // Check if there are predecessors of u contained in cycles,
    // if no, and is last, position it to the right
    // else up
    auto u_preds{fn::fn(ops.preds(u)).in(cycles).template into<Nodes>()};

    // Go up if is not last, otherwise left/right based on partition
    p[u] = (! u_preds.empty())? adj.up : (r == Partition::R)? adj.right : adj.left;
  } // for

  return p;
} // function: place_intersection }}}

// fn: place_cycle {{{
void place_cycle(Ops const& ops
  , Placement& p
  , Range auto&& cycle
  , Range auto&& inter
  , std::map<Node,Annotations> const& m_node_m_a
  , std::map<Edge,Weight> m_edge_weight
  , Partition r)
{
  // Check if a tile is occupied
  auto f_is_free = [&](Tile const& t){ return ! fn::fn(p).val().has(t); };

  // TODO: Replace this O(n^2) function
  auto path{fp::nub(cycle)};

  // Helper to indicate if an element v is present in a range r
  auto has = [](Range auto&& r, Node v){ return fn::fn(r).has(v); };

  // It is desirable to start from the intersection nodes in the cycle,
  // therefore:
  // - Find first occurrence of a node from inter in path
  // - Shift the path left, until the intersection elements are the first ones
  // - Create subrange for second partition
  auto slice = fn::fn(path)
    .rot(fn::fn(path).find_first_of(inter))
    .template cut<Nodes>(inter.size(),path.size());

  // Keep a map with a list of possible tiles, for backtracking
  std::map<Node,Tiles> m_backtrack;

  // Keep track of unplaced and placed elements
  std::stack<Node> placed, unplaced;

  // Try with reverse path if fails
  bool b_reversed{false};

  // Fill stack with unplaced elements
  fn::fn(slice).rev().ply([&](Node v){ unplaced.push(v); });

  // Helper to reverse path if current fails
  auto f_try_reverse_path = [&]
  {
    err::err({! b_reversed})("Failure to find a feasible solution");
    m_backtrack.clear();
    unplaced = std::stack<Node>{};
    placed = std::stack<Node>{};
    b_reversed = true;
    fn::fn(slice).ply([&](Node v){ p.erase(v); unplaced.push(v); });
  };

  while( ! unplaced.empty() )
  {
    // Get current node
    auto u{unplaced.top()}; unplaced.pop();

    // Skip if placed
    if( p.contains(u) ){ continue; }

    // Get all neighbors
    auto f_neighbors = [&](Node v)
    {
      return fn::fn(ops.preds(v)).chain(ops.succs(v)).into<Nodes>();
    };

    // Filter nodes that are not positioned
    Nodes nodes_placed{fn::fn(f_neighbors(u)).template in<Nodes>(p)};

    // Get all possible positions adjacent to positions of neighbors
    Tiles candidates;

    if( ! m_backtrack.contains(u) )
    {

      using F = typename std::function<Tiles(Tile)>;

      auto f_get_candidates = ( r == Partition::R )?
          F([&](Tile const& t) { return Adjacencies{t}.from_left(); })
       :  F([&](Tile const& t) { return Adjacencies{t}.from_right(); });

      // Get all candidate positions from previously positioned neighbors
      // Remove occupied positions
      candidates = fn::fn(nodes_placed)
        .as([&](Node v){ return f_get_candidates(p[v]); }) // nodes → tiles
        .squash() // merge [[tiles],[tiles]...] → [tiles]
        .template drop<Tiles>([&](Tile t){ return ! f_is_free(t); });

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
          return has(targets,dist);
        });
      };

      // Sort candidates by quality
      candidates = fn::fn(candidates).sort({},f_quality).template into<Tiles>();

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
      candidates = fn::fn(candidates).template in_all<Tiles>(nodes_placed,
        [&](Tile t, Node v){ return f_target(u,v) == f_dist(t,p.at(v));
      });
    } // if
    else
    {
      candidates = m_backtrack.at(u);
    } // else

    fmt::print("{} - Candidates: {}\n", u, candidates);

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
      if( placed.empty() ){ f_try_reverse_path(); continue; };
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


} // function: place_cycle }}}

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

  // // Get outputs
  // std::vector<i64> outputs;
  // ns_search::bfs::run(0, ops,
  // [&](auto e)
  // {
  //   if( ops.succs(e).size() == 0 ){ outputs.push_back(e); }
  //   return false;
  // });
  //
  // [[maybe_unused]] GridPath zz_p; // Path
  // Cycles zz_c; // Cycles
  //
  // // 1. Run zig-zag
  // fmt::print("------\n");
  //
  // auto zz_o{ns_search::zig_zag::run(outputs.at(0),ops,zz_p,zz_c
  // , [&](auto)
  //   {
  //     return ! zz_c.empty();
  //   }
  // )};
  //
  // fmt::print("Revisited node: {}\n", zz_c);
  // fmt::print("Incident cycle: {}\n", zz_p);
  //
  // zz_o = ns_search::zig_zag::run(outputs.at(0),ops,zz_p,zz_c);

  fmt::print("------\n");

  auto basis{minimal_basis(i64{},ops)};

  for( auto&& b : basis ){ err::info()("Base: {}\n", b); }

  // // 2. Get cyclic paths
  // auto paths{cyclic_paths(ops,zz_c)};
  //
  // fmt::print("------\n");
  //
  // for (i64 i{}; auto&& path : paths){ fmt::print("Path {}: {}\n", i++, path); } // for
  //
  // fmt::print("------\n");
  //
  // // 3. Get intersection of two smallest inner sub-cycles if they exist
  // auto intersection{fw::apply(paths
  //   , fw::transform([](auto e){ return fp::trim(7,e); })
  //   , fw::sort_by([](auto a, auto b){ return a.size() < b.size(); })
  //   , fw::take_exact(2)
  //   , fw::sets_intersection()
  // )};
  //
  // fmt::print("Intersection: {}\n", intersection);
  // fmt::print("------\n");
  //
  // // 4. Annotate with eBFS
  // std::map<Node,Annotations> m_n_m_a;
  // fmt::print("eBFS:\n");
  // for (Node r : intersection)
  // {
  //   fmt::print("- Table for {}:\n", r);
  //
  //   auto annotations{e_bfs(ops,r)};
  //
  //   m_n_m_a[r] = annotations;
  //
  //   for (auto&& [k,v] : annotations)
  //   {
  //     fmt::print("-- {} → {}\n", k,v);
  //   } // for
  // } // for
  // fmt::print("------\n");
  //
  // // 5. Annotated edges with e_bfs
  // fmt::print("Edge Weights:\n");
  // auto m_edge_weight{edge_weight(ops,intersection,m_n_m_a)};
  //
  // for (auto [k,v] : m_edge_weight)
  // {
  //   fmt::print("{} → {}\n", k ,v);
  // } // for
  // fmt::print("------\n");


  // // 6. Place intersection
  // Placement placement{place_intersection(ops
  //   , intersection
  //   , rv::concat(paths.at(2),paths.at(0))
  //   , Partition::R
  // )};
  //
  // fmt::print("Placement of intersection: {}\n", placement);
  // fmt::print("------\n");
  //
  // // 7. Place each cycle separately
  // place_cycle(ops
  //   , placement
  //   , paths.at(2)
  //   , intersection
  //   , m_n_m_a
  //   , m_edge_weight
  //   , Partition::L
  // );
  //
  // place_cycle(ops
  //   , placement
  //   , paths.at(0)
  //   , intersection
  //   , m_n_m_a
  //   , m_edge_weight
  //   , Partition::R
  // );
  //
  // fmt::print("Placement of cycles: {}\n", placement);
  // fmt::print("------\n");
  //
  // // 8. Merge Cycles
  // fmt::print("------\n");
  // fmt::print("------\n");
  //
  //
  // fmt::print("⊂ and ⊄\n");
  return EXIT_SUCCESS;
} // main }}}
