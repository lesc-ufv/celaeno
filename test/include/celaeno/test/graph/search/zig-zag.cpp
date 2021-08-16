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
namespace fp = fplus;
namespace fw = fplus::fwd;
namespace rg = ranges;
namespace ra = ranges::actions;
namespace rv = ranges::views;

namespace ns_graph = celaeno::graph;
namespace ns_heuristics = celaeno::heuristics;
namespace ns_reader = celaeno::graph::reader::verilog;
namespace ns_search = celaeno::graph::search;
// }}}

// Error Handlers {{{
template<typename... C>
[[nodiscard]] auto err(C&&... conds)
{
  return
  [passed=(conds && ...)]<String S, Printable... Args>(S&& msg, Args&&... args)
  {
    if( ! passed )
    {
      spdlog::error(std::forward<S>(msg), std::forward<Args>(args)...);
      exit(1);
    } // if
  };
} // function: check_if }}}

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

// fn: cyclic_paths {{{
//
// Given nodes that are part of cycles
// Returns the paths that compose the cycles
//
[[nodiscard]] GraphPaths cyclic_paths(Ops const& ops, Cycles const& cycles)
{
  // Novel intersection cycles
  std::vector<GraphPaths> out;

  // Find out the greatest tree
  for (auto r : cycles)
  {
    // Current cyclic paths
    GraphPaths graph_paths;

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
        if( stack_i.empty() )
        {
          spdlog::error("{}@{} Stack must never be empty", __FILE__,__LINE__);
          exit(1);
        } // if

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

    out.emplace_back(graph_paths);
  } // for

  auto f_by_greatest_cycle =
  [](GraphPaths const& graph_paths)
  {
    return fw::apply(graph_paths
      , fw::transform([](GraphPath const& graph_path){ return graph_path.size(); })
      , fw::maximum()
    );
  };

  // Return by greatest cycles
  return fp::maximum_by([&](auto&& a, auto&& b){ return f_by_greatest_cycle(a) < f_by_greatest_cycle(b); },out);
} // function: cyclic_paths }}}

// fn: e_bfs {{{
template<typename F = std::function<void(Edge)>>
[[nodiscard]] Annotations e_bfs(Ops const& ops
  , Node r
  , F&& f = [](Edge) -> void {}
)
  requires Returns<void,F,Edge>
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
    for (auto v : targets)
    {
      // Apply callback
      f(Edge{u,v});
      // Inherit distances from u to v
      rg::for_each(h[u],[&,v=v](auto d){ h[v].insert(d+1); });
      // Visit edge
      ve.emplace(u,v);
      // Enqueue v if not visited
      if( ! vn.contains(v) ){ q.push(v); }
    } // for

  } // while: ! q.empty()

  return h;
} // function: e_bfs }}}

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
    [&](Edge e) -> void
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
      err( ! v_a.empty(), ! w_a.empty()) ("v_a and w_a must not be empty");

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
  return Tiles{up,right,down};
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
  err(! nodes.empty())("Intersection must not not be empty");

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
    auto u_preds{fp::keep_if(
      [&](Node v)
      {
        return rg::contains(cycles,v);
      }
      ,ops.preds(u)
    )};

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
  auto f_is_occupied = [&](Tile const& t){ return rg::contains(rv::values(p),t); };

  // TODO: Replace this O(n^2) function
  auto path{fp::nub(cycle)};

  // Helper to indicate if an element v is present in a range r
  auto has = [](Range auto&& r, Node v){ return rg::contains(r,v); };

  // It is desirable to start from the intersection nodes in the cycle,
  // therefore:
  // - Find first occurrence of a node from inter in path
  // - Shift the path left, until the intersection elements are the first ones
  rg::rotate(path, rg::find_first_of(path,inter) );

  // Create subrange for first partition
  auto slice{rv::slice(path,std::distance(path.begin(),path.begin()+inter.size()),path.size())};

  // Keep a map with a list of possible tiles, for backtracking
  std::map<Node,Tiles> m_backtrack;

  // Keep track of unplaced and placed elements
  std::stack<Node> placed, unplaced;

  // Try with reverse path if fails
  bool b_reversed{false};

  // Fill stack with unplaced elements
  rg::for_each(rv::reverse(slice), [&](Node v){ unplaced.push(v); });

  // Helper to reverse path if current fails
  auto f_try_reverse_path = [&]
  {
    err(! b_reversed)("Failure to find a feasible solution");
    m_backtrack.clear();
    unplaced = std::stack<Node>{};
    placed = std::stack<Node>{};
    b_reversed = true;
    rg::for_each(slice, [&](Node v){ p.erase(v); });
    rg::for_each(slice, [&](Node v){ unplaced.push(v); });
  };

  while( ! unplaced.empty() )
  {
    // Get current node
    auto u{unplaced.top()}; unplaced.pop();

    // Skip if placed
    if( p.contains(u) ){ continue; }

    // Get all neighbors
    auto f_neighbors = [&](Node v){ return fp::append(ops.preds(v),ops.succs(v)); };

    // Filter nodes that are not positioned
    auto neighbors_positioned{
      fp::keep_if([&](Node v){ return p.contains(v); },f_neighbors(u))
    };

    // Get all possible positions adjacent to positions of neighbors
    Tiles candidates;

    if( ! m_backtrack.contains(u) )
    {

      using F = typename std::function<Tiles(Tile)>;

      auto f_get_candidates = ( r == Partition::R )?
          F([&](Tile const& t) { return Adjacencies{t}.from_left(); })
       :  F([&](Tile const& t) { return Adjacencies{t}.from_right(); });

      for (auto v : neighbors_positioned)
      {
        rg::copy(f_get_candidates(p[v]),std::back_inserter(candidates));
      } // for

      // Filter out occupied positions
      candidates = fp::keep_if([&](Tile t){ return ! f_is_occupied(t); }, candidates);

      // For each tile in candidates
      rg::sort(candidates,{},[&](Tile t)
      {
        // Count how many distance constraints from intersection it adheres, and
        // use this as a method for sorting best positions
        return - rg::count_if(inter,
        [&](Node v)
        {
          auto const& annotations{m_node_m_a.at(v).at(u)};
          auto dist{ns_heuristics::manhattan::run(t,p[v])};
          return has(annotations,dist);
        });
      });

      // For each tile in candidates, prioritise all that adhere to edge
      // constraints.
      candidates = fp::keep_if([&](Tile t)
      {
        // Count how many distance constraints from intersection it adheres, and
        // use this as a method for sorting best positions
        return rg::all_of(neighbors_positioned,
        [&](Node v)
        {
          i64 req_dist = ( m_edge_weight.contains({u,v}) )?
            m_edge_weight.at({u,v}) : m_edge_weight.at({v,u});

          auto cur_dist{ns_heuristics::manhattan::run(t,p.at(v))};

          return req_dist == cur_dist;
        });
      }, candidates);

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

  // Get outputs
  std::vector<i64> outputs;
  ns_search::bfs::run(0, ops,
  [&](auto e)
  {
    if( ops.succs(e).size() == 0 ){ outputs.push_back(e); }
    return false;
  });

  [[maybe_unused]] GridPath zz_p; // Path
  Cycles zz_c; // Cycles

  // 1. Run zig-zag
  auto zz_o{ns_search::zig_zag::run(outputs.at(0),ops,zz_p,zz_c)};

  // 2. Get cyclic paths
  auto paths{cyclic_paths(ops,zz_c)};

  fmt::print("------\n");

  for (i64 i{}; auto&& path : paths){ fmt::print("Path {}: {}\n", i++, path); } // for

  fmt::print("------\n");

  // 3. Get intersection of two smallest inner sub-cycles if they exist
  auto intersection{fw::apply(paths
    , fw::transform([](auto e){ return fp::trim(7,e); })
    , fw::sort_by([](auto a, auto b){ return a.size() < b.size(); })
    , fw::take_exact(2)
    , fw::sets_intersection()
  )};

  fmt::print("Intersection: {}\n", intersection);
  fmt::print("------\n");

  // 4. Annotate with eBFS
  std::map<Node,Annotations> m_n_m_a;
  fmt::print("eBFS:\n");
  for (Node r : intersection)
  {
    fmt::print("- Table for {}:\n", r);

    auto annotations{e_bfs(ops,r)};

    m_n_m_a[r] = annotations;

    for (auto&& [k,v] : annotations)
    {
      fmt::print("-- {} → {}\n", k,v);
    } // for
  } // for
  fmt::print("------\n");

  // 5. Annotated edges with e_bfs
  fmt::print("Edge Weights:\n");
  auto m_edge_weight{edge_weight(ops,intersection,m_n_m_a)};

  for (auto [k,v] : m_edge_weight)
  {
    fmt::print("{} → {}\n", k ,v);
  } // for
  fmt::print("------\n");

  // 6. Place intersection
  Placement placement{place_intersection(ops
    , intersection
    , rv::concat(paths.at(2),paths.at(0))
    , Partition::R
  )};

  fmt::print("Placement of intersection: {}\n", placement);
  fmt::print("------\n");

  // 7. Place each cycle separately
  place_cycle(ops
    , placement
    , paths.at(2)
    , intersection
    , m_n_m_a
    , m_edge_weight
    , Partition::L
  );

  place_cycle(ops
    , placement
    , paths.at(0)
    , intersection
    , m_n_m_a
    , m_edge_weight
    , Partition::R
  );

  fmt::print("Placement of cycles: {}\n", placement);
  fmt::print("------\n");

  // 8. Merge Cycles
  fmt::print("------\n");
  fmt::print("------\n");


  fmt::print("⊂ and ⊄\n");
  return EXIT_SUCCESS;
} // main }}}
