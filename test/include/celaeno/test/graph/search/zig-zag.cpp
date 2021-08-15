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

#include <fplus/fplus.hpp>
#include <range/v3/all.hpp>
#include <fmt/core.h>
#include <spdlog/spdlog.h>

#include <celaeno/aliases.hpp>
#include <celaeno/concepts.hpp>
#include <celaeno/heuristics/manhattan.hpp>
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
template<String S, typename... C>
decltype(auto) err_check_if(S&& msg, C&&... conds)
{
  if( ! ( conds && ... ) )
  {
    spdlog::error(msg);
    exit(1);
  } // if
} // function: check_if }}}

// Macros {{{
#define assertm(exp, msg) assert(((void)msg, exp))
// }}}

// Aliases {{{
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

// fn: lookahead {{{
//
// Uses placement hash to determine how many tiles are still available
//
Tiles lookahead(Occupation const& o, Tiles const& ts)
{
  return Tiles{fp::keep_if([&](Tile t){ return ! o.contains(t); },ts)};
} // function: lookahead }}}

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
[[nodiscard]] GraphPaths cyclic_paths(ns_graph::Ops const& ops, Cycles const& cycles)
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
[[nodiscard]] Annotations e_bfs(ns_graph::Ops const& ops
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
std::map<Edge,Weight> edge_weight(ns_graph::Ops const& ops,
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
      err_check_if(fmt::format("v_a and w_a must not be empty"),
        ! v_a.empty(), ! w_a.empty()
      );

      // Get max elements of v_a
      auto v_a_max {rg::max_element(v_a)};

      // Get max elements of w_a
      auto w_a_max {rg::max_element(w_a)};

      // Get max between abs diff
      auto diff{fp::abs_diff(*v_a_max,*w_a_max)};

      // If a previous value was assigned to the edge,
      // use min

      out[e] = (out.contains(e))? std::min(out.at(e),diff) : diff;
    }); // e_bfs
  } // for

  return out;
} // function: edge_weight }}}

// enum: Rotation {{{
enum class Rotation
{
  CLOCKWISE,
  COUNTERCLOCKWISE,
}; // enum: Rotation }}}

// struct: Adjacencies {{{
struct Adjacencies
{
  Tile up;
  Tile down;
  Tile left;
  Tile right;

  Adjacencies(Tile const& src);
  Tiles clockwise(Tile const& src);
  Tiles counterclockwise(Tile const& src);
};

Adjacencies::Adjacencies(Tile const& src)
  : up   (Tile{src.first  , src.second-1})
  , down (Tile{src.first  , src.second+1})
  , left (Tile{src.first-1, src.second})
  , right(Tile{src.first  , src.second+1})
{};

Tiles Adjacencies::clockwise(Tile const& src)
{
  return Tiles{up,left,down,right};
};

Tiles Adjacencies::counterclockwise(Tile const& src)
{
  return Tiles{up,right,down,left};
};
// struct: Adjacencies }}}

// fn: adjacent tiles {{{
Tiles adjacent_tiles(Tile const& src, Rotation const& r)
{
  static const Tiles clockwise         {{0,-1},{1,0},{0,-1},{-1,0}};
  static const Tiles counterclockwise  {{0,-1},{-1,0},{0,1},{1,0}};

  auto sum_tiles = [](Tile const& t1, Tile const& t2)
  {
    return std::make_pair(t1.first + t2.first, t1.second + t2.second);
  }; // lamb: sum_tiles

  if( r == Rotation::CLOCKWISE )
  {
    return fp::transform([&](Tile const& t){ return sum_tiles(t,src); }, clockwise);
  } // if

  return fp::transform([&](Tile const& t){ return sum_tiles(t,src); }, counterclockwise);
} // function: adjacent_tiles }}}

// // fn: place_intersection {{{
// Placement place_intersection(ns_graph::Ops const& ops,
//   Nodes const& i,
//   std::map<Node,Annotations> const& m_node_m_a,
//   Rotation r)
// {
//   Placement p;
//
//   // Check if intersection is not empty
//   assertm(! i.empty(),
//     fmt::format("{}@{} Intersection must not not be empty", __FILE__, __LINE__)
//   );
//
//   // Get nodes in i that are in p
//   auto f_i_placed = [&](Node v) -> Nodes
//   {
//     auto placed{fp::keep_if([&](Node w){ return p.contains(w); },i)};
//
//     assertm( ! placed.empty(),
//       fmt::format("{}@{} No placed nodes before {}"
//         , __FILE__
//         , __LINE__
//         , v
//     ));
//
//     return placed;
//   };
//
//   // Set predecessor as i[0]
//   Node pred{i.at(0)};
//
//   // Place it in (0,0)
//   p[pred] = {0,0};
//
//   // Perform placement of next nodes
//   for (auto it{std::next(i.begin())}; it != i.end(); ++it)
//   {
//     // Get current node
//     auto u{*it};
//
//     // Get potential positions from previous node
//     Tiles tiles{adjacent_tiles(p[pred],r)};
//
//     // Get nodes placed before u
//     auto i_placed{f_i_placed(u)};
//
//     // Keep tile t1, if it is at a annotated distance of tile t2 of v
//     fmt::print("Pred: {}\nCurrent node: {}\nPlaced nodes: {}\n", pred, u, i_placed);
//     tiles = fp::keep_if([&](Tile const& t1)
//     {
//       return rg::all_of(i_placed,[&](Node v)
//       {
//         Tile t2{p.at(v)};
//         auto dist{ns_heuristics::manhattan::run(t1,t2)};
//         auto allowed_dists{m_node_m_a.at(v).at(u)};
//         return rg::find(allowed_dists,dist) != rg::end(allowed_dists);
//       });
//     },tiles);
//     fmt::print("Viable positions: {}\n", tiles);
//
//     // Check if there are viable positions for u
//     assertm(! tiles.empty() ,fmt::format("No viable positions for node {}\n",u));
//
//     // Prefer second position if has number of predecessors equal to 2
//     if( ops.preds(u).size() > 1 && tiles.size() > 1 )
//     {
//       p[u] = tiles.at(1);
//     } // if
//     else
//     {
//       p[u] = tiles.at(0);
//     } // else
//
//   } // for
//
//   return p;
// } // function: place_intersection }}}

// // fn: place_cycle {{{
// Placement place_cycle(ns_graph::Ops ops,
//   Range auto&& i, // Intersection
//   Range auto&& cycle, // Full cycle
//   std::map<Node,Annotations> const& m_node_m_a,
//   Rotation r,
//   Placement& p
//   )
// {
//   // Keep track of placed nodes
//   std::set<Node> s_visited;
//
//   // Helper that checks is range r has node v
//   auto contains = []<Range R>(R&& r, Node v){ return rg::find(r,v) != rg::end(r); };
//
//   // Set cycle to start at intersection
//   auto it{rg::partition(cycle,[&](Node v){ return contains(i,v); })};
//
//   fmt::print("Placing partitions:\n\t{}\n\t{}\n"
//     , rg::subrange(rg::begin(cycle),it)
//     , rg::subrange(it,rg::end(cycle))
//   );
//
//   // Place intersection
//   {
//     auto rng_i{rg::subrange(rg::begin(cycle),it)};
//
//     // Check if not empty subrange
//     assertm( ! rng_i.empty(), "Intersection must not be empty");
//
//     // Place first element at position (0,0)
//     p[rng_i.at(0)] = {0,0};
//
//     Node pred{rng_i.at(0)};
//
//     for (auto it{rng_i.begin()}; it != rng_i.end(); ++it)
//     {
//
//     } // for
//   }
//
//   for (Node pred{}; auto u : rg::subrange(rg::begin(cycle),it))
//   {
//     // Handle first case
//     if( p.empty() )
//     {
//       pred = u;
//       p[u] = {0,0};
//       s_visited.insert(u);
//       continue;
//     }
//
//     // Check if node is already placed
//     if( s_visited.contains(u) ){ continue; }
//
//     // If node has 2 predecessors or sucessors, reverse rotation
//     auto is_in_cycle = [&](Node v){ return contains(cycle,v); };
//     auto neighbor_count = [&]<typename F>(F&& f, Node v)
//     {
//       return fp::keep_if([&](Node w) { return is_in_cycle(w); }, f(v) ).size();
//     };
//
//     if( (neighbor_count(ops.preds, u) > 1) or (neighbor_count(ops.succs,u) > 1) )
//     {
//       fmt::print("Reversed rotation for node {}\n", u);
//       r = (r == Rotation::CLOCKWISE)? Rotation::COUNTERCLOCKWISE : Rotation::CLOCKWISE;
//     } // if
//
//     // Get rotation based tiles
//     assertm(p.contains(pred), "Predecessor must be already in a tile");
//     // Tiles tiles{  }
//   } // for
//
//   return p;
//
// } // function: place_cycle }}}

// // enum: Priority {{{
// enum class Priority
// {
//   LOW,
//   HIGH,
// };
// // }}}

// // fn: Priority {{{
// Priority get_priority(i64 degree)
// {
//   return (degree <= 2)? Priority::LOW : Priority::HIGH;
// };
// // }}}

// // fun: tiles_from_annotations {{{
// std::optional<Tiles> tiles_from_annotations(
//   Node src,
//   Node dest,
//   ns_graph::Ops const& ops,
//   Annotations const& a,
//   Placement const& placement)
// {
//   // fmt::print("Placement: {}\n", placement); // TODO remove
//
//   // Set search offsets
//   static const std::vector<Tile> high_offsets {{0,1},{1,0},{0,-1},{-1,0}};
//   static const std::vector<Tile> low_offsets  {{0,-1},{-1,0},{1,0},{0,1}};
//
//   // Obtain degree priority of 'dest'
//   auto [preds,succs] = std::make_pair(ops.preds(dest),ops.succs(dest));
//
//   // Degree
//   auto degree{preds.size()+succs.size()};
//
//   // Priority
//   auto priority{get_priority(degree)};
//
//   // Check if for every cycle node k, there distance restriction for dest
//   std::vector<std::pair<Node,Distance>> dest_constraints;
//   for (auto [k,v] : a)
//   {
//     // Ignore self-distance
//     if( k == dest ){ continue; }
//
//     // Save all distances required to place 'dest' in respect to other nodes
//     auto constraints{fw::apply(v
//       , fw::keep_if([&](auto e){ return e.first == dest; })
//       , fw::map_to_pairs()
//       , fw::transform([&,k=k](auto e){ return std::make_pair(k,e.second); })
//     )};
//
//     // Insert in constraints container
//     rg::copy(constraints,std::back_inserter(dest_constraints));
//
//   } // for
//
//   // fmt::print("Annotations of {}: {}\n", dest, dest_constraints); // TODO remove
//
//   // Keep tiles that respect 'dest' annotations
//   auto f_sum_tiles =
//   [](Tile a, Tile b)
//   {
//     return std::make_pair(a.first+b.first,a.second+b.second);
//   };
//
//   // Canditate tiles sorted by priority
//   auto src_tile{placement.at(src)};
//   auto dest_tiles
//   {
//     (priority == Priority::HIGH)?
//       fp::transform([&](Tile t){ return f_sum_tiles(src_tile,t); },high_offsets)
//     :
//       fp::transform([&](Tile t){ return f_sum_tiles(src_tile,t); },low_offsets)
//   };
//
//   // Verify which tiles adhere to annotations
//   dest_tiles = fp::keep_if(
//     [&](Tile const& dest_tile)
//     {
//       for (auto [node,dist] : dest_constraints)
//       {
//         auto target_tile{placement.at(node)};
//         auto target_dist{ns_heuristics::manhattan::run(dest_tile,target_tile)};
//         if( target_dist != dist ){ return false; }
//       } // for
//       return true;
//     },dest_tiles);
//
//   // Remove occupied positions
//   auto filtered_dest_tiles {fw::apply(dest_tiles
//     , fw::keep_if([&](auto&& t)
//       {
//         return ! fp::is_elem_of(t,fp::get_map_values(placement));
//       })
//   )};
//
//
//   // fmt::print("dest_tiles: {}\n", dest_tiles); // TODO remove
//   //
//   // fmt::print("filtered_dest_tiles: {}\n", filtered_dest_tiles); // TODO remove
//   //
//
//   // If not viable position was found, return null
//   if( filtered_dest_tiles.empty() ) { return std::nullopt; } // if
//
//   return filtered_dest_tiles;
//
//
// } // function: tiles_from_annotations }}}

// // fun: place {{{
// decltype(auto) place(ns_graph::Ops const& ops, GridPath const& m_p, Cycles const& v_c, Annotations const& m_a)
// {
//   //
//   // Pick a random cycle node
//   //
//   assertm(! v_c.empty(), "Cycle vector must not be empty!");
//
//   auto f_rand_index =
//   [](auto const& c)
//   {
//     std::uniform_int_distribution<Node> dist(0,c.size());
//     std::mt19937 gen{std::random_device{}()};
//     return dist(gen);
//   };
//
//   //
//   // Create ownership table
//   //
//   using Tile = std::pair<i64,i64>;
//   std::map<Tile,Node> ot;
//
//
//   //
//   // Degree matcher
//   //
//   auto f_filter_positions =
//   [&](Node u, Tile const& t, i64 degree)
//   {
//     // Positions
//     auto f_left  = [](Tile const& t) -> Tile { return std::make_pair(t.first-1,t.second); };
//     auto f_right = [](Tile const& t) -> Tile { return std::make_pair(t.first+1,t.second); };
//     auto f_up    = [](Tile const& t) -> Tile { return std::make_pair(t.first,t.second-1); };
//     auto f_down  = [](Tile const& t) -> Tile { return std::make_pair(t.first,t.second+1); };
//
//     // Get priority
//     Priority p{get_priority(degree)};
//
//     // Populate positions based on priority
//     std::vector<Tile> positions;
//
//     auto emplace_back = [&]<typename... T>(T&&... t){ (positions.emplace_back(std::forward<T>(t)),...); };
//
//     if(p == Priority::LOW)
//     {
//       emplace_back(f_up(t),f_left(t),f_right(t),f_down(t));
//     } // if
//     else
//     {
//       emplace_back(f_down(t),f_right(t),f_up(t),f_left(t));
//     } // else
//
//     // Filter invalid positions
//     // Valid position:
//     // cond1: is free?
//     // cond2: is owner adjacent to u? (u → v) or (v → u)
//     // cond3: TODO Are available positions enough for u?
//     // Must consider: (cond1 or cond2) and cond3
//     auto is_valid_pos =
//     [&](Tile const& t) -> bool
//     {
//       return
//           ( ! ot.contains(t) )? true
//         : ( ops.adj(u,ot.at(t)) or ops.adj(ot.at(t),u) )? true
//         : false;
//     };
//
//     return fp::keep_if(is_valid_pos,positions);
//   }; // lamb: f_filter_positions
//
//   //
//   // Get preferred position by annotations
//   //
//   auto f_get_position =
//   [&](Node u)
//   {
//     using Requirements = std::map<Node,Dist>;
//
//     Requirements req;
//
//     // Get requirements
//     for (Node c : v_c)
//     {
//       auto e_rng{m_a.equal_range(c)};
//
//       for (auto it{e_rng.first}; it != e_rng.second; ++it)
//       {
//
//       } // for
//     } // for
//
//     // Get valid positions relative to required distances from cycle nodes
//   };
//
//   //
//   // Place node and update ownership table
//   //
//   auto place_and_reserve =
//   [&](Node p, Node u)
//   {
//     // Get nodes adjacent to u
//     auto preds{ops.preds(u)};
//     auto succs{ops.succs(u)};
//
//     // Get u's priority
//     Priority priority{get_priority(preds.size() + succs.size())};
//
//     // Get preferred tile by annotations
//
//     // Get positions ordered by priority and filtered by degree
//     // auto positions{f_get_positions(u,tile,)};
//
//   };
//
//   // Position initial node
//   Node u{f_rand_index(v_c)};
//
//   for (auto e : m_p)
//   {
//
//   } // for
//
// } // function: place }}}

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
  ns_graph::Ops ops(f_p, f_s, f_a, f_l, f_u);

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
  std::map<Node,Annotations> m_annotations;
  fmt::print("eBFS:\n");
  for (Node r : intersection)
  {
    fmt::print("- Table for {}:\n", r);

    auto annotations{e_bfs(ops,r)};

    m_annotations[r] = annotations;

    for (auto&& [k,v] : annotations)
    {
      fmt::print("-- {} → {}\n", k,v);
    } // for
  } // for

  // 5. Annotated edges with e_bfs
  fmt::print("------\n");
  fmt::print("Edge Weights:\n");
  auto weights{edge_weight(ops,intersection,m_annotations)};

  for (auto [k,v] : weights)
  {
    fmt::print("{} → {}\n", k ,v);
  } // for

  fmt::print("------\n");

  // 6. Place intersection
  fmt::print("------\n");
  // Placement placement{
  //   place_intersection(ops,
  //     fp::convert_container<std::vector<Node>>(intersection),
  //     m_annotations,
  //     Rotation::COUNTERCLOCKWISE
  // )};
  // fmt::print("Placement of intersection: {}\n", placement);
  fmt::print("------\n");

  // 7. Place each cycle separately
  fmt::print("------\n");
  // Placement p;
  // place_cycle(ops
  //   , intersection
  //   , paths.at(2)
  //   , m_annotations
  //   , Rotation::COUNTERCLOCKWISE
  //   , p
  // );
  fmt::print("------\n");

  // 8. Merge Cycles
  fmt::print("------\n");
  fmt::print("------\n");


  fmt::print("⊂ and ⊄\n");
  return EXIT_SUCCESS;
} // main }}}
