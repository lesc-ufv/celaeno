// vim: set expandtab fdm=marker ts=2 sw=2 tw=80 et :
//
// @author      : Ruan E. Formigoni (ruanformigoni@gmail.com)
// @file        : minimal-basis
// @created     : Saturday Oct 23, 2021 14:20:53 -03
//
// BSD 2-Clause License

// Copyright (c) 2021, Ruan Evangelista Formigoni
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

#pragma once

#include <range/v3/all.hpp>
#include <fplus/fplus.hpp>

#include <celaeno/concepts.hpp>
#include <celaeno/aliases.hpp>
#include <celaeno/fun/fun.hpp>
#include <celaeno/err/err.hpp>
#include <celaeno/graph/graph.hpp>

namespace celaeno::graph::search::minimal_basis
{

// Using namespace {{{
using namespace celaeno::fun::fn;
using namespace celaeno::concepts;
using namespace celaeno::aliases;
// }}}

// namespaces {{{
namespace err = celaeno::err;
namespace rg = ranges;
namespace rv = ranges::views;

namespace ns_graph = celaeno::graph;
// }}}

// Aliases {{{
using Sink = std::shared_ptr<spdlog::logger>;
using Ops = ns_graph::Ops;
using Node = i64;
using Nodes = std::vector<Node>;
using Edge = std::pair<Node,Node>;
using Edges = std::vector<Edge>;
using PairsInterCycles = std::vector<std::pair<Nodes,Nodes>>;
// }}}

// fn: get_outer_cycle {{{
//
// Computes the outer cycle of a list of adjacent cycles
//
Nodes get_outer_cycle(PairsInterCycles const& cycles, Sink sink)
{
  // Init logger
  err::Logger logger{sink};

  // Outer cycle
  Nodes out;

  for (i64 i{}; auto [intersection,cycle] : cycles)
  {
    if( i++ == 0 )
    {
      // Push fist cycle as outer cycle
      rg::copy(cycle, std::back_inserter(out));

      // Log intersection and cycle
      logger.info()("Intersection: {}", intersection);
      logger.info()("Cycle: {}", cycle);

      // Skip iteration
      continue;
    } // if

    // Update intersection
    intersection = fn(cycle).in(out).vec();

    // Log intersection and cycle
    logger.info()("Intersection: {}", intersection);
    logger.info()("Cycle: {}", cycle);

    fmt::print("Intersection: {}\n", intersection);
    fmt::print("Cycle: {}\n", cycle);

    std::cerr << "Starting rot" << std::endl;

    // Make elements of intersection the first elements of cycle and out
    logger.info()("Cycle before rotation: {}", cycle);
    cycle = fn(cycle)
      .rot([&intersection=intersection](auto&& v)
      {
        return fn(intersection).has(v.front()) && ! fn(intersection).has(v.back());
      })
      .vec();

    std::cerr << "End rot" << std::endl;

    logger.info()("Cycle after rotation: {}", cycle);

    logger.info()("Outer cycle before rotation: {}", out);
    // Rotate the vector until there are not intersection elements in the end
    out = fn(out)
      .rot([&intersection=intersection](auto&& v)
      {
        return fn(intersection).has(v.front()) && ! fn(intersection).has(v.back());
      })
      .vec();
    logger.info()("Outer cycle after rotation: {}", out);

    // Set elements in the intersection in the same order as in cycle and out
    for (size_t i{}; i < intersection.size(); ++i)
    {
      intersection.at(i) = cycle.at(i);
    } // for
    logger.info()("Intersection after replacement: {}", intersection);

    // Find fst element of intersection
    auto it_fst{rg::find_if(out
      , [&,&intersection=intersection](Node u)
      {
        return fn(intersection).has(u);
      })
    };

    // Check if insertion points were found
    err::err({ it_fst != rg::end(out) })("Fst insertion point not found");

    // Find snd element, one past the intersection
    auto it_snd{rg::find_if(std::next(it_fst)
      , out.end()
      , [&,&intersection=intersection](Node u)
      {
        return ! fn(intersection).has(u);
      })
    };

    // Check if insertion points were found
    err::err({ it_snd != rg::end(out) })("Snd insertion point not found");

    // Set iterators to remove elements in-between intersection
    std::advance(it_fst,1);
    std::advance(it_snd,-1);

    // Check if insertion points were found
    err::err({ it_fst != rg::end(out) })("Fst insertion point not found");

    // Log
    logger.info()("it_fst: {}", *it_fst);
    logger.info()("it_snd: {}", *it_snd);

    logger.info()("Outer cycle before removal of elements: {}", out);

    // In outer cycle, erase all other elements until it_snd
    out.erase(it_fst, it_snd);

    // Log
    logger.info()("Outer cycle after removal of elements: {}", out);

    // Check if cycle requires reverse copy
    if( cycle.front() ==  *std::prev(it_fst) )
    {
      out.insert(it_fst,cycle.rbegin(),std::prev(cycle.rend(),intersection.size()));
    } // if
    else
    {
      out.insert(it_fst,cycle.begin()+intersection.size(),cycle.end());
    } // else

    logger.info()("Merged Cycle: {}", out);

  } // for

  return out;

} // fn: get_outer_cycle }}}

// fn: get_pivots {{{
//
// Verifies if there are unvisited edges for at least two nodes that are part of the outer cycle
// // TODO Make this a generator
//
template<typename F>
Nodes get_pivots(Nodes outer_cycle, std::set<Node> const& visited_pivots, F&& f, Sink sink)
{
  // Init logger
  err::Logger logger{sink};

  // Remove repeated back node
  if( outer_cycle.front() == outer_cycle.back() ){ outer_cycle.pop_back(); }

  // Log found external cycle
  logger.info()("External cycle: {}\n", outer_cycle);
  fmt::print("External cycle: {}\n", outer_cycle);

  // Path from fst to snd pivots
  Nodes out;

  // Create pivots
  auto fst_pivot{outer_cycle.end()};
  auto snd_pivot{outer_cycle.end()};

  // Search fst pivot
  fst_pivot = std::find_if(outer_cycle.begin(), outer_cycle.end()
    , [&](Node u){ return f(u) && ! visited_pivots.contains(u);
  });

  if( fst_pivot == outer_cycle.end() ){ return out; }

  // Rotate fst_pivot to become fst element
  std::rotate(outer_cycle.begin(), fst_pivot, outer_cycle.end());

  // Update fst pivot
  fst_pivot = outer_cycle.begin();

  fmt::print("fst_pivot: {}\n", *fst_pivot);

  // Search function, finds elements that has unvisited neighbors and are different to fst pivot
  auto f_search = [&](Node u) -> bool { return (u != *fst_pivot) && f(u); };

  // Search from begin to end
  auto it_forwards_search {rg::find_if(outer_cycle,f_search)};

  // Check if snd pivot was found
  if(it_forwards_search == outer_cycle.end()){ return out; }

  // Search in a cyclic manner from it towards begin
  auto view_reverse{rv::reverse(outer_cycle)};

  auto it_backwards_search {std::find_if(view_reverse.begin(), view_reverse.end(),f_search)};

  // Calculate distance
  auto dist_forwards{std::distance(fst_pivot,it_forwards_search)};

  // +1 to count pivot
  auto dist_backwards{std::distance(view_reverse.begin(),it_backwards_search)+1};

  fmt::print("outer_cycle: {}\n", outer_cycle);
  fmt::print("dist_forwards: {}\n", dist_forwards);
  fmt::print("dist_backwards: {}\n", dist_backwards);

  // Check which pivot is closer to fst_pivot
  if (dist_backwards < dist_forwards)
  {
    err::err({ *fst_pivot != *it_forwards_search })("Snd pivot must be different of fst");
    out.push_back(*fst_pivot);
    fmt::print("back: snd_pivot: {}\n", *it_backwards_search);
    rg::copy(view_reverse.begin(), std::next(it_backwards_search), std::back_inserter(out));
  } // if
  else
  {
    err::err({ *fst_pivot != *it_backwards_search })("Snd pivot must be different of fst");
    fmt::print("forw: snd_pivot: {}\n", *it_forwards_search);
    rg::copy(fst_pivot,std::next(it_forwards_search),std::back_inserter(out));
  } // else

  logger.info()("Out: {}", out);

  fmt::print("Out: {}\n", out);

  return out;

} // fn: get_pivots }}}

// fn: align_intersections {{{
template<Range R>
decltype(auto) align_intersections(R&& minimal_basis, Sink sink)
{
  // Init logger
  err::Logger logger{sink};

  err::err({ ! minimal_basis.empty() })("Minimal basis must not be empty");

  err::err({ minimal_basis.size() > 1 })("Minimal basis must contains at least two cycles");

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

} // namespace celaeno::graph::search::minimal_basis
