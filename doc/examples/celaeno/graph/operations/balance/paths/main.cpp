// vim: set expandtab fdm=marker ts=2 sw=2 tw=100 et :
//
// @author      : Ruan E. Formigoni (ruanformigoni@gmail.com)
// @file        : test
// @created     : terça ago 18, 2020 17:31:57 -03
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

#include <iostream>
#include <cstdlib>
#include <fmt/ranges.h>
#include <taygete/graph/graph.hpp>
#include <celaeno/aliases.hpp>
#include <celaeno/graph/operations/balance/paths.hpp>
#include <celaeno/graph/views/depth.hpp>


// namespaces {{{
namespace graph = taygete::graph;
namespace depth = celaeno::graph::views::depth;
namespace balance = celaeno::graph::operations::balance::paths;
// }}}

int main()
{
  // Graph {{{
  graph::Graph<i64> g
  {
    {1,5},{2,4},{3,7},{6,11},
    {4,7},{5,7},{5,8},{5,9},
    {7,12},{8,10},{8,11},
  };
  // }}}

  // Required behaviors {{{
  auto p = [&g](auto&& u){ return g.predecessors(u); };
  auto s = [&g](auto&& u){ return g.successors(u); };
  auto l = [&g](auto&& uv) -> void { g.emplace(uv); };
  auto u = [&g](auto&& uv) -> void { g.erase(uv); };
  // }}}

  // Create a topological view to analyse graph {{{
  auto [layers,_] = depth::run(1,p,s);
  // }}}

  // Print before {{{
  fmt::print("Before balancing: \n");
  i32 prev{0};
  for (auto&& l : layers)
  {
    fmt::print("{} ",l.second);
    if( l.first > prev ){ prev = l.first; fmt::print("\n"); }
  } // for l : layers }}}
  fmt::print("\n-------\n");


  // Run algorithm {{{
  balance::run(1,p,s,l,u);
  // }}}

  // Create a topological view to analyse graph {{{
  layers = depth::run(1,p,s).first;
  // }}}

  // Print to after {{{
  fmt::print("After balancing: \n");
  prev = 0;
  for (auto&& l : layers)
  {
    fmt::print("{} ",l.second);
    if( l.first > prev ){ prev = l.first; fmt::print("\n"); }
  } // for l : layers }}}
  fmt::print("\n");

  return EXIT_SUCCESS;
} // main
