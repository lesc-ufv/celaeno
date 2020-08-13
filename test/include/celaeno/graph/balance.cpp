// vim: set expandtab fdm=marker ts=2 sw=2 tw=100 et :
//
// @author      : Ruan E. Formigoni (ruanformigoni@gmail.com)
// @file        : balance
// @created     : Wednesday Apr 01, 2020 21:24:23 -03
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
#include <cstdlib>
#include <concepts>
#include <chrono>
#include <range/v3/all.hpp>
#include <celaeno/graph/balance.hpp>
#include <celaeno/graph/views/depth.hpp>
#include <celaeno/aliases.hpp>
#include <taygete/graph/graph.hpp>
#include <taygete/graph/reader/verilog.hpp>
#include <maia/circuits/synth-91.hpp>

// namespace celaeno::graph::balance::test {{{

namespace celaeno::graph::balance::test
{

// namespaces {{{

namespace graph = taygete::graph;
namespace reader = taygete::graph::reader::verilog;
namespace cir = maia::circuits;
namespace balance = celaeno::graph::balance;
namespace depth = celaeno::graph::views::depth;
namespace rg = ranges;
namespace rv = ranges::views;
namespace ra = ranges::actions;

// }}}

// Concepts {{{

template<typename T>
concept String = requires(T t){ std::string{t}; };

// }}}

// Test Cases {{{

TEST_CASE("celaeno::graph::balance"
  * doctest::description("Balance test")
  * doctest::timeout(1000.0f)
)
{
  // Logger {{{
  auto logger {spdlog::basic_logger_mt("graph::balance", "logs/celaeno/graph/balance.csv", true)};
  spdlog::set_default_logger(logger);
  spdlog::set_pattern("%v");
  spdlog::info("date,time,vertices,edges,runtime");
  spdlog::set_pattern("%d/%m/%Y,%T,%v");
  // }}}

  // test lambda {{{
  auto test = [&]<String S>(S&& str)
  {
    // Read graph {{{
    graph::Graph<int64_t> g;
    auto emplace = [&g](auto&& pair){ g.emplace(pair); };
    reader::Reader{str,emplace};
    // }}}

    // Helpers {{{
    auto pred = [&g](auto&& v){ return g.predecessors(v); };
    auto succ = [&g](auto&& v){ return g.successors(v); };
    auto link = [&g](auto&& pair){ g.emplace(pair); };
    auto unlink = [&g](auto&& pair){ g.erase(pair); };
    // }}}

    // Test Balance {{{
    auto start {std::chrono::system_clock::now()};
    balance::run(0,pred,succ,link,unlink);
    auto end {std::chrono::system_clock::now()};
    std::chrono::duration<f64> dur {end-start};
    std::stringstream ss; ss << dur.count();
    // }}}

    // More tests {{{
    // * Given a depth-view, each vertex must have a distance of one
    // * to its successor or predecessor
    auto dview {depth::run(0,pred,succ)};
    auto const& level_vert {dview.first};
    auto const& vert_level {dview.second};

    // Get the levels
    auto levels { level_vert | rv::keys | rv::unique };

      // Get the vertices on level l
    for(auto const& l : levels)
    {
      auto rng{level_vert.equal_range(l)};
      // For each vertex on level l
      for(auto it{rng.first}; it!=rng.second; ++it)
      {
        // Current vertex
        auto const& curr {it->second};
        // The adjacent vertices
        auto adj {g.neighbors(curr)};
        // Verify if distance is one to each
        auto is_dist_one = [&vert_level,&curr](auto&& a) -> void
          { REQUIRE(std::abs(vert_level.at(a) - vert_level.at(curr)) == 1); };
        // Execute tests
        rg::for_each(adj, is_dist_one);
      } // for it{rng.first}; it!=rng.second
    } // for auto const& l : levels
    // }}}

    // Log results {{{
    spdlog::info("{},{},{}", g.vertices_count(), g.edges_count(), ss.str());
    // }}}

  }; // lamb: test }}}

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

} // TEST_CASE: celaeno::graph::balance }}}

} // namespace celaeno::graph::balance::test }}}
