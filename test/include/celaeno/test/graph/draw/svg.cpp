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
#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest/doctest.h>

#include <iostream>
#include <cstdint>

#include <celaeno/test/graph/data/synth-91.hpp>

#include <taygete/graph/graph.hpp>
#include <taygete/graph/reader.hpp>

#include <celaeno/graph/draw/svg.hpp>
// }}}

// Testcase: "celaeno::graph::draw::svg" {{{
TEST_CASE("celaeno::graph::draw::svg")
{

  // namespaces {{{
  namespace graph = taygete::graph;
  namespace reader = taygete::graph::reader;
  namespace svg = celaeno::graph::draw::svg;
  namespace circ = celaeno::test::graph::data;
  // }}}

  // Read graph {{{
  graph::Graph<i64> g;
  auto emplace = [&g](auto&& e) -> void { g.emplace(e); };
  auto metadata {reader::Reader{circ::synth_91::b1,emplace}};
  // }}}

  // Helpers {{{
  auto f_p = [&g](auto&& v){ return g.predecessors(v); };
  auto f_s = [&g](auto&& v){ return g.successors(v); };
  auto f_a = [&g](auto&& u, auto&& v){ return g.adjacent(u,v); };
  auto f_l = [&g](auto&& e){ g.emplace(e); };
  auto f_u = [&g](auto&& e){ g.erase(e); };
  // }}}

  // Gate type {{{
  auto f_label = [&metadata](auto id)
  {
    using Type = taygete::graph::reader::GateType;
    auto data{metadata.data()};
    if( ! data.contains(id) )  { return " "; }
    switch (data.at(id))
    {
      case Type::AND: return "∧";
      case Type::OR: return "∨";
      case Type::NAND: return "~∧";
      case Type::NOR: return "~∨";
      case Type::XOR: return "⊕";
      case Type::XNOR: return "~⊕";
      case Type::MAJ3: return "M";
    } // switch
  };
  // }}}

  // Test drawing {{{
  svg::run(1,f_p,f_s,f_a,f_l,f_u,f_label,"artifacts/b1-default.svg");
  // }}}


} // TEST_CASE: "celaeno::graph::draw::svg" }}}
