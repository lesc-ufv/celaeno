// vim: set expandtab fdm=marker ts=2 sw=2 tw=80 et :
//
// @author      : Ruan E. Formigoni (ruanformigoni@gmail.com)
// @file        : svg
// @created     : monday set 14, 2020 21:21:02 -03
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

// Includes {{{
#include <iostream>
#include <cstdint>
#include <fstream>
#include <sstream>

#include <range/v3/all.hpp>
#include <spdlog/spdlog.h>
#include <fmt/core.h>

#include <celaeno/graph/graph.hpp>
#include <celaeno/graph/reader/verilog.hpp>
#include <celaeno/graph/draw/svg.hpp>
// }}}

// Using namespace {{{
using namespace celaeno::aliases;
// }}}

// namespaces {{{
namespace rg = ranges;

namespace ns_graph = celaeno::graph;
namespace ns_reader = celaeno::graph::reader::verilog;
namespace ns_svg = celaeno::graph::draw::svg;
// }}}

int main(int argc, char* argv[])
{
  char const* help_msg =
    "Usage:\n"
    "./svg -i input-file.v -o output-file.svg\n\n"
    "Options:\n"
    "  -h|--help: Show this message\n"
    "  -i: Input file\n"
    "  -o: Output file\n"
  ;

  // Check input arguments
  if( argc != 5 )
  {
    fmt::print(help_msg);
    exit(0);
  }

  spdlog::info("Input file {}", argv[2]);
  spdlog::info("Output file {}", argv[4]);

  // Read graph
  ns_graph::Graph<i64> g;
  auto emplace = [&g](auto&& e) -> void { g.emplace(e); };
  auto metadata {ns_reader::Reader{argv[2],emplace}};

  // Helpers
  auto f_p = [&g](auto v){ return g.predecessors(v); };
  auto f_s = [&g](auto v){ return g.successors(v); };
  auto f_a = [&g](auto u, auto v){ return g.adjacent(u,v); };
  auto f_l = [&g](auto e){ g.emplace(e); };
  auto f_u = [&g](auto e){ g.erase(e); };

  ns_graph::Ops ops(f_p, f_s, f_a, f_l, f_u);

  // Gate label
  auto f_label = [&](auto id) { return id; };

  // Test drawing
  ns_svg::run(1, ops, f_label, fmt::format("{}",argv[4]) );
} // main
