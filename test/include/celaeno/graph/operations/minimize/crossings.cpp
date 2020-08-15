// vim: set expandtab fdm=marker ts=2 sw=2 tw=100 et :
//
// @author      : Ruan E. Formigoni (ruanformigoni@gmail.com)
// @file        : minimize-crossings
// @created     : quarta jun 24, 2020 11:56:38 -03
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
#include <chrono>
#include <spdlog/spdlog.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <doctest/doctest.h>
#include <range/v3/all.hpp>
#include <fplus/fplus.hpp>
#include <taygete/graph/graph.hpp>
#include <taygete/graph/reader/verilog.hpp>
#include <maia/circuits/synth-91.hpp>
#include <celaeno/graph/operations/balance/paths.hpp>
#include <celaeno/graph/operations/minimize/crossings.hpp>
#include <celaeno/graph/operations/count/crossings.hpp>
#include <celaeno/graph/representations/incidence.hpp>
#include <celaeno/graph/views/proximity.hpp>

// namespace celaeno::graph::operations::minimize::crossings::test {{{
namespace celaeno::graph::operations::minimize::crossings::test
{

// namespaces {{{
namespace fw = fplus::fwd;
namespace rg = ranges;
namespace rv = ranges::views;
namespace cir = maia::circuits;
namespace graph = taygete::graph;
namespace reader = taygete::graph::reader::verilog;
namespace balance = celaeno::graph::operations::balance::paths;
namespace count_crossings = celaeno::graph::operations::count::crossings;
namespace proximity = celaeno::graph::views::proximity;
namespace incidence = celaeno::graph::representations::incidence;
namespace minimize = celaeno::graph::operations::minimize::crossings;
// }}}

// Concepts {{{
template<typename T>
concept String = requires(T t){ std::string{t}; };
// }}}

TEST_CASE("celaeno::graph::operations::minimize::crossings"
  * doctest::description("Graph crossing minimization test")
)
{

  // Logger {{{
  auto logger {spdlog::basic_logger_mt("graph::operations::minimize::crossings"
    , "logs/celaeno/graph/operations/minimize/crossings.csv", true)};
  spdlog::set_default_logger(logger);
  spdlog::set_pattern("%v");
  spdlog::info("date,time,vertices,edges,prev_crossings,new_crossings,runtime");
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

    // Create a proximity view {{{
    auto pred = [&g](auto&& v){ return g.predecessors(v); };
    auto succ = [&g](auto&& v){ return g.successors(v); };
    auto [dv,_] {proximity::run(0, pred, succ)};
    // }}}

    // node_t {{{
    using node_t = decltype(dv)::key_type;
    // }}}

    // Balace the graph {{{
    // Link and unlink graph edges
    auto link = [&](auto&& e) -> void { g.emplace(e); };
    auto unlink = [&](auto&& e) -> void
    {
      auto rng {g.data().equal_range(e.first)};
      for (auto it{rng.first}; it != rng.second; ++it)
      {
        if( it->second == e.second )
        {
          g.data().erase(it);
          break;
        }
      } // for: it != it.second
    };
    // Execute the balacing algorithm
    balance::run(0, pred, succ, link, unlink);
    // }}}

    // Pre-processing {{{
    auto layer = [&dv](node_t idx)
    {
      return fw::apply(dv
        , fw::drop_if([&idx](auto&& e){ return e.first != idx; })
        , fw::get_map_values()
      );
    };
    // Get the number of layers of the graph
    auto layers {fw::apply(dv,fw::get_map_keys(),fw::unique(),fw::size_of_cont())};
    // Verify edge uv exists.
    auto adjacent = [&g](node_t u, node_t v) {  return g.adjacent(u,v); };
    // }}}

    // Perform test {{{
    // Incidence matrices of the graph
    auto ms {incidence::run(layer, adjacent, layers)};
    // Calculate current crossings
    i64 prev_crossings{};
    rg::for_each(ms,[&](auto&& m){ prev_crossings += count_crossings::run(m); });
    // Start algorithm
    auto start {std::chrono::system_clock::now()};
    auto result{minimize::run(ms, layer, layers)};
    auto end {std::chrono::system_clock::now()};
    std::chrono::duration<f64> dur {end-start};
    std::stringstream ss; ss << dur.count();
    // Calculate new number of crossings {{{
    i64 new_crossings{};
    auto new_layers = [&result](node_t idx) { return result.at(idx); };
    ms = incidence::run(new_layers, adjacent, layers);
    rg::for_each(ms,[&](auto&& m){ new_crossings += count_crossings::run(m); });
    // }}}

    // Log results {{{
    spdlog::info("{},{},{},{},{}",
        g.vertices_count(), g.edges_count(), prev_crossings, new_crossings, ss.str());
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
    cir::synth_91::b1 ,
    cir::synth_91::c8 ,
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
  ); // }}}

} // TEST_CASE: "celaeno::graph::operations::minimize::crossings"

} // namespace celaeno::graph::operations::minimize::crossings::test }}}
