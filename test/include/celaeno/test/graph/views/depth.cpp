// vim: set expandtab fdm=marker ts=2 sw=2 tw=100 et :
//
// @author      : Ruan E. Formigoni (ruanformigoni@gmail.com)
// @file        : depth
// @created     : Wednesday Apr 08, 2020 16:04:13 -03
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

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest/doctest.h>
#include <chrono>
#include <vector>
#include <range/v3/all.hpp>
#include <spdlog/spdlog.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <celaeno/graph/views/depth.hpp>
#include <celaeno/aliases.hpp>
#include <taygete/graph/graph.hpp>
#include <taygete/graph/reader/verilog.hpp>
#include <maia/circuits/iscas.hpp>
#include <maia/circuits/synth-91.hpp>


// namespace celaeno::graph::views::depth::test {{{
namespace celaeno::graph::views::depth::test
{

// Namespaces {{{
namespace cir = maia::circuits;
namespace graph = taygete::graph;
namespace reader = taygete::graph::reader::verilog;
namespace depth = celaeno::graph::views::depth;
namespace rg = ranges;
namespace rv = ranges::views;
namespace ra = ranges::actions;
// }}}

// Concepts {{{
template<typename T>
concept String = requires(T t){ std::string{t}; };
// }}}

// test case celaeno::graph::views::depth  {{{

TEST_CASE("celaeno::graph::views::depth")
{
  // Logger {{{
  auto logger {spdlog::basic_logger_mt("graph::views::depth", "logs/celaeno/graph/views/depth.csv", true)};
  spdlog::set_default_logger(logger);
  spdlog::set_pattern("%v");
  spdlog::info("date,time,vertices,edges,runtime");
  spdlog::set_pattern("%d/%m/%Y,%T,%v");
  // }}}

  // Expected values array {{{
  constexpr std::array<i8,24> const ev
  {{
    40,42,35,15,14,4,9,4,6,4,13,9,8,3,5,7,8,18,13,9,20,3,49
  }};
  i64 ridx{};
  // }}}

  // test lambda {{{
  auto test = [&]<String S>(S&& str)
  {
    // Read Graph {{{
    graph::Graph<i64> g;
    auto emplace = [&g](auto&& pair){ g.emplace(pair); };
    reader::Reader{str,emplace};
    // }}}

    // Create depth view {{{
    // Helpers
    auto pred = [&g](auto&& v){ return g.predecessors(v); };
    auto succ = [&g](auto&& v){ return g.successors(v); };
    // Start chrono
    auto start {std::chrono::system_clock::now()};
    auto dv {depth::run(0, pred, succ).first};
    // Stop chrono
    auto end {std::chrono::system_clock::now()};
    std::chrono::duration<f64> dur {end-start};
    std::stringstream ss; ss << dur.count();
    // }}}


    // lamb: get_depth {{{
    auto fdepth = [&]()
    {
      return  fw::apply(dv,fw::get_map_keys(),fw::maximum());
    }; // lamb: fdepth
    // }}}

    // Test Depth {{{
    REQUIRE(fdepth() == ev.at(ridx++));
    // }}}

    // Log results {{{
    spdlog::info("{},{},{}", g.vertices_count(), g.edges_count(), ss.str());
    // }}}
  }; // lamb: test }}}

  // Forwarding test folding lambda {{{
  auto tests = [&]<String... S>(S&&... strs) -> void { (test(std::forward<S>(strs)), ...); };
  // }}}

  // LGSynth 91 tests {{{

  tests(
    cir::synth_91::alu2   , // 40
    cir::synth_91::alu4   , // 42
    cir::synth_91::dalu   , // 35
    cir::synth_91::apex6  , // 15
    cir::synth_91::apex7  , // 14
    cir::synth_91::b1     , // 4
    cir::synth_91::c8     , // 9
    cir::synth_91::cc     , // 4
    cir::synth_91::cht    , // 6
    cir::synth_91::cm138a , // 4
    cir::synth_91::cm150a , // 13
    cir::synth_91::cm162a , // 9
    cir::synth_91::cm163a , // 8
    cir::synth_91::cm42a  , // 3
    cir::synth_91::cm82a  , // 5
    cir::synth_91::cm85a  , // 7
    cir::synth_91::cmb    , // 8
    cir::synth_91::comp   , // 18
    cir::synth_91::cordic , // 13
    cir::synth_91::cu     , // 9
    cir::synth_91::count  , // 20
    cir::synth_91::decod  , // 3
    cir::synth_91::my_adder // 49
  );

  // }}}

} // TEST_CASE: celaeno::graph::views::depth }}}


} // namespace celaeno::graph::views::depth::test }}}
