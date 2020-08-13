// vim: set expandtab fdm=marker ts=2 sw=2 tw=100 et :
//
// @author      : Ruan E. Formigoni (ruanformigoni@gmail.com)
// @file        : proximity
// @created     : sexta jul 31, 2020 02:33:31 -03
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
#include <spdlog/spdlog.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <iostream>
#include <fplus/fplus.hpp>
#include <range/v3/all.hpp>
#include <taygete/graph/graph.hpp>
#include <taygete/graph/reader/verilog.hpp>
#include <maia/circuits/synth-91.hpp>
#include <celaeno/aliases.hpp>
#include <celaeno/graph/views/proximity.hpp>
#include <maia/circuits/synth-91.hpp>

// namespace celaeno::graph::view::proximity::test {{{

namespace celaeno::graph::view::proximity::test
{
// Namespaces {{{
namespace graph = taygete::graph;
namespace reader = taygete::graph::reader::verilog;
namespace circuit = maia::circuits;
namespace proximity = celaeno::graph::views::proximity;
namespace cir = maia::circuits;
namespace fp = fplus;
namespace fw = fplus::fwd;
namespace rv = ranges::views;
// }}}

// Concepts {{{
template<typename T>
concept String = requires(T t){ std::string{t}; };
// }}}

TEST_CASE("celaeno::graph::view::proximity"
  * doctest::description("Graph proximity view test")
  * doctest::timeout(100.0f)
)
{

  // Logger {{{
  auto logger {spdlog::basic_logger_mt("graph::views::proximity",
      "logs/celaeno/graph/views/proximity.csv", true)};
  spdlog::set_default_logger(logger);
  spdlog::set_pattern("%v");
  spdlog::info("date,time,vertices,edges,runtime");
  spdlog::set_pattern("%d/%m/%Y,%T,%v");

  // }}}

  // test_lambda {{{
  auto test = [&]<String S>(S&& str)
  {
    //  Read graph {{{
    graph::Graph<i64> g;
    auto emplace = [&g](auto&& e) -> void { g.emplace(e); };
    reader::Reader{str,emplace};
    // }}}

    // Execute proximity view algorithm {{{
    auto pred = [&g](auto&& v){ return g.predecessors(v); };
    auto succ = [&g](auto&& v){ return g.successors(v); };
    auto start {std::chrono::system_clock::now()};
    auto [dv,vd] {proximity::run(0, pred, succ)};
    auto end {std::chrono::system_clock::now()};
    std::chrono::duration<f64> dur {end-start};
    std::stringstream ss; ss << dur.count();
    // }}}

    // Perform tests {{{
    for (auto&& [v,d] : vd)
    {
      // If has successors get the minimum level between all of them
      auto successors{succ(v)};
      if( ! successors.empty() )
      {
        i64 minimum {fw::apply(successors
          , fw::transform([&](auto&& s){ return vd.at(s); })
          , fw::sort()
          , fw::minimum()
        )};
        // Check distance of v and topologically nearest successor is one
        CHECK( minimum-d == 1 );
      } // if: ! successors.empty()
    } // for [v,d] : vd
    // }}}

    // Log results {{{
    spdlog::info("{},{},{}", g.vertices_count(), g.edges_count(), ss.str());
    // }}}
  }; // lamb: test }}}

  // Forwarding test folding lambda {{{
  auto tests = [&]<String... S>(S&&... strs) { (test(std::forward<S>(strs)), ...); };
  // }}}

  // LGSynth 91 tests {{{
  tests(
    cir::synth_91::alu2,
    cir::synth_91::alu4,
    cir::synth_91::dalu,
    cir::synth_91::apex6,
    cir::synth_91::apex7,
    cir::synth_91::b1,
    cir::synth_91::c8,
    cir::synth_91::cc,
    cir::synth_91::cht,
    cir::synth_91::cm138a,
    cir::synth_91::cm150a,
    cir::synth_91::cm151a,
    cir::synth_91::cm162a,
    cir::synth_91::cm163a,
    cir::synth_91::cm42a,
    cir::synth_91::cm82a,
    cir::synth_91::cm85a,
    cir::synth_91::cmb,
    cir::synth_91::comp,
    cir::synth_91::cordic,
    cir::synth_91::cu,
    cir::synth_91::count,
    cir::synth_91::decod,
    cir::synth_91::my_adder
  );
  // }}}

} // TEST_CASE: "celaeno::graph::view::proximity"

} // namespace celaeno::graph::view::proximity::test

// }}}
