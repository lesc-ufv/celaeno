// vim: set expandtab fdm=marker ts=2 sw=2 tw=80 et :
//
// @author      : Ruan E. Formigoni (ruanformigoni@gmail.com)
// @file        : celaeno
// @created     : Wednesday Aug 18, 2021 18:10:25 UTC
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

// External libs
#include <argparse/argparse.hpp>
#include <fplus/fplus.hpp>
#include <fmt/ranges.h>

// Utils
#include <celaeno/aliases.hpp>
#include <celaeno/concepts.hpp>

// Data structure and reader
#include <celaeno/graph/graph.hpp>
#include <celaeno/graph/reader/verilog.hpp>

// Algorithms
#include <celaeno/svg/graph.hpp>
#include <celaeno/graph/operations/count/crossings.hpp>

// namespaces {{{
namespace fp = fplus;

namespace ns_graph = celaeno::graph;
namespace ns_reader = celaeno::graph::reader::verilog;
namespace ns_operations = celaeno::graph::operations;
namespace ns_svg = celaeno::svg;
// }}}

// using namespaces {{{
using namespace celaeno::aliases;
using namespace celaeno::concepts;
// }}}

// Aliases {{{
template<Arithmetic T>
using Graph = ns_graph::Graph<T>;
// }}}

// fn: g_read {{{
template<Arithmetic T, String S>
void g_read(Graph<T>& g, S&& i)
{
  auto emplace = [&g](auto&& e) -> void { g.emplace(e); };

  [[maybe_unused]] auto metadata {ns_reader::Reader{i,emplace}};
} // function: g_read }}}

// fn: g_ops {{{
decltype(auto) g_ops(ns_graph::Graph<i64>& g)
{
  // Helpers
  auto f_p = [&g](auto v){ return g.predecessors(v); };
  auto f_s = [&g](auto v){ return g.successors(v); };
  auto f_a = [&g](auto u, auto v){ return g.adjacent(u,v); };
  auto f_l = [&g](auto e){ g.emplace(e); };
  auto f_u = [&g](auto e){ g.erase(e); };

  return ns_graph::Ops(f_p, f_s, f_a, f_l, f_u);
} // function: g_ops }}}

// fn: g_draw {{{
template<String S>
void g_draw(S&& i, S&& o)
{
  Graph<i64> g;
  g_read(g,i);
  auto ops{g_ops(g)};

  ns_svg::graph::run(0,ops,o);
} // function: g_draw }}}

// fn: g_crossings {{{
template<String S>
i64 g_crossings(S&& i)
{
  Graph<i64> g;
  g_read(g,i);
  auto ops{g_ops(g)};

  return ns_operations::count::crossings::run(0,ops);
} // function: g_crossings }}}

// fn: main {{{
int main(int argc, char const* argv[])
{
  argparse::ArgumentParser program("celaeno");

  program.add_argument("-d", "--draw")
    .help("Draw a graph from an input, to an output file")
    .nargs(2);

  program.add_argument("-c", "--crossings")
    .help("Count the number of crossings in the input file")
    .nargs(1);

  try
  {
    program.parse_args(argc,argv);
  } // try
  catch (std::runtime_error const& e)
  {
    std::cout << e.what() << std::endl;
    std::cout << program;
    exit(1);
  } // catch

  //
  // Graph drawing
  //
  try
  {
    auto files{program.get<std::vector<std::string>>("--draw")};

    g_draw(files.at(0),files.at(1));
  } // try
  catch(std::logic_error const&){}

  //
  // Crossing count
  //
  try
  {
    auto files{program.get<std::vector<std::string>>("--crossings")};

    fmt::print("Crossings: {}\n", g_crossings(files.at(0)));
  } // try
  catch(std::logic_error const&){}

  return 0;
} // main }}}
