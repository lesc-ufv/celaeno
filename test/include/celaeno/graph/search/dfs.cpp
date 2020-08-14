// vim: set expandtab fdm=marker ts=2 sw=2 tw=100 et :
//
// @author      : Ruan E. Formigoni (ruanformigoni@gmail.com)
// @file        : dfs
// @created     : Wednesday Apr 08, 2020 08:07:08 -03
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
#include <celaeno/aliases.hpp>
#include <celaeno/graph/search/dfs.hpp>
#include <taygete/graph/graph.hpp>
#include <taygete/graph/reader/verilog.hpp>
#include <maia/circuits/iscas.hpp>
#include <maia/circuits/synth-91.hpp>
#include <fplus/fplus.hpp>

// namespace: celaeno::graph::search::dfs::test {{{

namespace celaeno::graph::search::dfs::test
{

// Aliases {{{
namespace reader = taygete::graph::reader::verilog;
namespace graph = taygete::graph;
namespace dfs = celaeno::graph::search::dfs;
namespace cir = maia::circuits;
namespace fw = fplus::fwd;
// }}}

// Concepts {{{
template<typename T>
concept String = requires(T t){ std::string{t}; };
// }}}

// test_case: celaeno::graph::search::dfs {{{

TEST_CASE("celaeno::graph::search::dfs"
  * doctest::description("Depth-First Search test")
  * doctest::timeout(10.0f)
)
{
  // Logger {{{
  auto logger {spdlog::basic_logger_mt("graph::search::dfs"
      , "logs/celaeno/graph/search/dfs.csv", true)};
  spdlog::set_default_logger(logger);
  spdlog::set_pattern("%v");
  spdlog::info("date,time,vertices,edges,runtime");
  spdlog::set_pattern("%d/%m/%Y,%T,%v");
  // }}}

  // test lambda {{{
  auto test = [&]<String S>(S&& str)
  {
    //  Read graph {{{
    graph::Graph<i64> g;
    auto emplace = [&g](auto&& e) -> void { g.emplace(e); };
    reader::Reader{str,emplace};
    // }}}

    // Test if vertices_count > 0 {{{
    REQUIRE(g.vertices_count() > 0);
    // }}}

    // Test dfs {{{
    auto adj = [&g](auto&& v){ return g.neighbors(v); };
    auto start {std::chrono::system_clock::now()};
    auto dfs {dfs::run(0,adj)};
    auto end {std::chrono::system_clock::now()};
    std::chrono::duration<f64> dur {end-start};
    std::stringstream ss; ss << dur.count();
    // }}}

    // Test vertices count with bfs size {{{
    REQUIRE(g.vertices_count() == dfs.size());
    // }}}

    // Test dfs vector size  {{{
    REQUIRE(fw::apply(dfs,fw::unique()).size() == dfs.size());
    // }}}

    // Log results {{{
    spdlog::info("{},{},{}", g.vertices_count(), g.edges_count(), ss.str());
    // }}}
  }; // lamb: test
  // }}}

  // Forwarding test folding lambda {{{
  auto tests = [&]<String... S>(S&&... strs) { (test(std::forward<S>(strs)), ...); };

  // }}}

  // LGSynth 91 tests {{{
  tests(cir::synth_91::alu2,
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

} // TEST_CASE: celaeno::graph::search::dfs }}}


} // namespace celaeno::graph::search::dfs::test

// }}}
