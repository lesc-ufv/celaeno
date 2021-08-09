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

// Macros {{{
#define assertm(exp, msg) assert(((void)msg, exp))
// }}}

// Aliases {{{
using Node = i64;
using Nodes = std::vector<Node>;
using Edge = std::pair<Node,Node>;
using Edges = std::vector<Edge>;
using Dist = i64;
using Annotations = std::map<Node,std::map<Node,Dist>>;
using Tile = std::pair<i64,i64>;
using Tiles = std::vector<Tile>;
using Path = std::vector<std::pair<Node,Node>>;
using Cycles = std::vector<Node>;
using Placement = std::map<Node,Tile>;
using Occupation = std::set<Tile>;
using Tree = std::multimap<Node,Node>;
// }}}

// fn: lookahead {{{
//
// Uses placement hash to determine how many tiles are still available
//
Tiles lookahead(Occupation const& o, Tiles const& ts)
{
  return Tiles{fp::keep_if([&](Tile t){ return ! o.contains(t); },ts)};
} // function: lookahead }}}

// fn: cycles_intersection {{{
//
// @creates a cycle tree, given an initial node in the graph
// @returns the intersection of the cycles.
//
Cycles cycles_intersection(ns_graph::Ops const& ops, Cycles const& cycles)
{
  // Novel intersection cycles
  Cycles out;

  // Cycles tree
  std::vector<Tree> vector_t;

  // Find out the greatest tree
  for (auto r : {cycles.at(0)})
  {
    // Current tree
    Tree t;

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

      fmt::print("u: {}\n", u);

      // Take its predecessors and successors
      auto [p,s] = std::make_pair(ops.preds(u),ops.succs(u));

      // Create a joined view of 'p' and 's'
      auto n{rv::concat(p,s)};

      fmt::print("Concat view: {}\n", n);

      // Take top of stack_i
      auto i{stack_i.top()};

      // pop i whilst u is not contained
      auto contains = []<Range R>(R&& r, Node u){ return rg::find(r,u) != rg::end(r); };

      // Rewind
      while( ! contains(i,u) )
      {
        // Check for errors
        if( stack_i.empty() )
        {
          spdlog::error("{}@{} Stack must never be empty", __FILE__,__LINE__);
          exit(1);
        } // if

        stack_i.pop();

        i = stack_i.top();
      }

      fmt::print("i: {}\n", i);

      // Check if leaf is the same id as root
      if( ! contains(i,r) && contains(n,r) )
      {
        fmt::print("First opt\n");
        // Create leaf edge
        auto e {Edge{u,r}};

        // Insert in tree
        t.emplace(e);

        stack_i.push(i);

        fmt::print("-------\n");
        // Finish current processing
        continue;
      } // if
      else
      {
        fmt::print("Second opt\n");
      } // else

      // Nodes to insert in stack_i
      Nodes nodes_i;

      // Insert u for next stack pop of i
      nodes_i.emplace_back(u);

      bool next{false};

      fmt::print("Edges: ");
      // Check for each node in n, if a novel edge may be created
      for (Node v : n)
      {
        if( v == r ){ continue; }

        // Create potetial edge
        auto e{Edge{u,v}};

        // Check if is valid for processing
        if( ( (! set_edge.contains(e)) or contains(cycles,v) ) && ! contains(i,v) )
        {
          fmt::print("{},", e);

          next = true;
          // Update tree
          t.emplace(e);
          // Mark edge as visited
          set_edge.emplace(e);
          // Include v for next previously visited nodes_i
          nodes_i.emplace_back(v);
          // Include v in the nodes stack
          stack_node.push(v);
        } // if
      } // for
      fmt::print("\n");

      if( next ) stack_i.push(nodes_i);

      fmt::print("-------\n");
    } // while

    vector_t.emplace_back(t);
  } // for


  rg::sort(vector_t,{},[](auto&& t){ return t.size(); });

  auto test{*vector_t.rbegin()};

  for (auto e : test)
  {
    fmt::print("{}\n", e);
  } // for

  // // Check if out is not empty
  // if( ! out.empty() )
  // {
  //   spdlog::error("{}@{} Result must not be empty", __FILE__,__LINE__);
  //   exit(1);
  // } // if

  return out;
} // function: cycles_intersection }}}

// fun: annotate {{{
Annotations annotate(ns_graph::Ops const& ops, Cycles const& zz_c)
{
  // Annotations
  Annotations zz_a;

  // Process all cycle nodes
  for (auto c : zz_c)
  {
    auto bfs{ns_search::bfs::run(c,ops.preds,ops.succs)};

    fmt::print("{} bfs: {}\n", c, bfs);

    // auto it{rg::find_if(bfs,[&](auto e){ return e == c; })};

    for (auto it{bfs.begin()}; it != bfs.end(); ++it )
    {
      // Get current node of ordered sequence
      i64 u{*it};

      // Set cycle distance to self to 0
      if( u == c ) { zz_a[c][c] = 0; continue; } // if

      // Get all nodes adjacent to 'u'
      auto n{fp::append(ops.preds(u),ops.succs(u))};

      // Keep annotated nodes
      // Return their distances
      auto d {fw::apply(n
        , fw::keep_if([&](auto e){ return zz_a[c].contains(e); })
        , fw::transform([&](auto e){ return zz_a[c][e]; })
      )};

      // If there is at least one node adjacent 'u' that is annotated
      // Annotated 'u' based on their annotations
      if( ! d.empty() )
      {
        zz_a[c][u] = fp::minimum(d)+1;
        // if( fp::abs_diff(fp::maximum(d),fp::minimum(d)) <= 2 )
        // {
        //   zz_a[c][u] = fp::minimum(d)+1;
        // }
        // else
        // {
        //   zz_a[c][u] = fp::maximum(d)+1;
        // } // else
      } // if
      else
      {
        zz_a[c][u] = 1;
      } // else
    } // for
  } // for

  return zz_a;
} // function: annotate }}}

// enum: Priority {{{
enum class Priority
{
  LOW,
  HIGH,
};
// }}}

// fn: Priority {{{
Priority get_priority(i64 degree)
{
  return (degree <= 2)? Priority::LOW : Priority::HIGH;
};
// }}}

// fun: tiles_from_annotations {{{
std::optional<Tiles> tiles_from_annotations(
  Node src,
  Node dest,
  ns_graph::Ops const& ops,
  Annotations const& a,
  Placement const& placement)
{
  // fmt::print("Placement: {}\n", placement); // TODO remove

  // Set search offsets
  static const std::vector<Tile> high_offsets {{0,1},{1,0},{0,-1},{-1,0}};
  static const std::vector<Tile> low_offsets  {{0,-1},{-1,0},{1,0},{0,1}};

  // Obtain degree priority of 'dest'
  auto [preds,succs] = std::make_pair(ops.preds(dest),ops.succs(dest));

  // Degree
  auto degree{preds.size()+succs.size()};

  // Priority
  auto priority{get_priority(degree)};

  // Check if for every cycle node k, there distance restriction for dest
  std::vector<std::pair<Node,Dist>> dest_constraints;
  for (auto [k,v] : a)
  {
    // Ignore self-distance
    if( k == dest ){ continue; }

    // Save all distances required to place 'dest' in respect to other nodes
    auto constraints{fw::apply(v
      , fw::keep_if([&](auto e){ return e.first == dest; })
      , fw::map_to_pairs()
      , fw::transform([&,k=k](auto e){ return std::make_pair(k,e.second); })
    )};

    // Insert in constraints container
    rg::copy(constraints,std::back_inserter(dest_constraints));

  } // for

  // fmt::print("Annotations of {}: {}\n", dest, dest_constraints); // TODO remove

  // Keep tiles that respect 'dest' annotations
  auto f_sum_tiles =
  [](Tile a, Tile b)
  {
    return std::make_pair(a.first+b.first,a.second+b.second);
  };

  // Canditate tiles sorted by priority
  auto src_tile{placement.at(src)};
  auto dest_tiles
  {
    (priority == Priority::HIGH)?
      fp::transform([&](Tile t){ return f_sum_tiles(src_tile,t); },high_offsets)
    :
      fp::transform([&](Tile t){ return f_sum_tiles(src_tile,t); },low_offsets)
  };

  // Verify which tiles adhere to annotations
  dest_tiles = fp::keep_if(
    [&](Tile const& dest_tile)
    {
      for (auto [node,dist] : dest_constraints)
      {
        auto target_tile{placement.at(node)};
        auto target_dist{ns_heuristics::manhattan::run(dest_tile,target_tile)};
        if( target_dist != dist ){ return false; }
      } // for
      return true;
    },dest_tiles);

  // Remove occupied positions
  auto filtered_dest_tiles {fw::apply(dest_tiles
    , fw::keep_if([&](auto&& t)
      {
        return ! fp::is_elem_of(t,fp::get_map_values(placement));
      })
  )};


  // fmt::print("dest_tiles: {}\n", dest_tiles); // TODO remove
  //
  // fmt::print("filtered_dest_tiles: {}\n", filtered_dest_tiles); // TODO remove
  //

  // If not viable position was found, return null
  if( filtered_dest_tiles.empty() ) { return std::nullopt; } // if

  return filtered_dest_tiles;


} // function: tiles_from_annotations }}}

// // fun: place {{{
// decltype(auto) place(ns_graph::Ops const& ops, Path const& m_p, Cycles const& v_c, Annotations const& m_a)
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

  Path zz_p; // Path
  Cycles zz_c; // Cycles

  // Run zig-zag
  auto zz_o{ns_search::zig_zag::run(outputs.at(0),ops,zz_p,zz_c)};

  // Annotations
  Annotations zz_a{annotate(ops, zz_c)};

  fmt::print("Ordering: {}\n", zz_o);
  fmt::print("Path: {}\n", fp::sort_by([](auto a, auto b){ return a.first < b.first; },zz_p));
  fmt::print("Rev Path: {}\n", fp::sort_by([](auto a, auto b){ return a.first < b.first; },fp::swap_pairs_elems(zz_p)));
  fmt::print("Cycles: {}\n", zz_c);
  fmt::print("Annotations:\n");
  rg::for_each(zz_a, [](auto e){ fmt::print("{}\n", e); });
  fmt::print("\n--------\n\n");

  Placement placement;

  auto it{zz_p.begin()};
  auto src{it->first};
  auto dest{it->second};

  placement[src] = std::make_pair(0,0);

  while(auto tiles{tiles_from_annotations(src, dest, ops, zz_a, placement)})
  {
    placement[dest] = tiles->at(0);

    if( it = std::next(it); it == zz_p.end() ){ break; }

    std::tie(src,dest) = std::tie(it->first,it->second);
  } // while

  fmt::print("Placement:\n{}\n", placement);

  cycles_intersection(ops,{7,6});

  return 0;
} // main }}}
