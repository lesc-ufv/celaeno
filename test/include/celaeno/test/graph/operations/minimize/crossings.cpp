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
#include <celaeno/graph/operations/minimize/crossings.hpp>
#include "../../template.hpp"

// TODO Remove
#include <fmt/core.h>
#include <fmt/ranges.h>

// namespace celaeno::graph::operations::minimize::crossings::test {{{
namespace celaeno::graph::operations::minimize::crossings::test
{

// namespaces {{{
namespace cir = maia::circuits;
namespace graph = taygete::graph;
namespace reader = taygete::graph::reader::verilog;
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

  // test_lambda {{{
  auto test = [&]<String S>(S&& str)
  {
    //  Read graph {{{
    graph::Graph<i64> g;
    auto emplace = [&g](auto&& e) -> void { g.emplace(e); };
    reader::Reader{str,emplace};
    // }}}

    // Helpers {{{
    auto p = [&g](auto&& v){ return g.predecessors(v); };
    auto s = [&g](auto&& v){ return g.successors(v); };
    auto l = [&](auto&& e) -> void { g.emplace(e); };
    auto u = [&](auto&& e) -> void { g.erase(e); };
    // }}}

    // Perform test {{{

    // Start algorithm and log duration {{{
    celaeno::graph::test::runtime([&](){ return minimize::run(0,p,l,s,u); });
    // }}}


    // std::ranges::for_each(result,
    // [](auto&& vertices)
    // {
    //   fmt::print("-- {}\n", vertices);
    // });
    //
    //
    // // Number of layers of the graph
    // auto layers {fw::apply(pv,fw::get_map_keys(),fw::unique(),fw::size_of_cont())};
    //
    // // Calculate new number of crossings {{{
    // auto adjacent = [&](auto&& _u, auto&& _v){ return rg::contains(s(_u),_v); };
    // auto new_layers = [&result](i64 idx) { return result.at(idx); };
    // auto ms {incidence::impl::run(new_layers, adjacent, layers)};
    // i64 new_crossings{};
    // rg::for_each(ms,[&](auto&& m){ new_crossings += count_crossings::run(m); });
    // i64 new_crossings{count_crossings::run(ms)};
    // }}}


    // // TESTS {{{
    // CHECK(layers == result.size());
    // CHECK(new_crossings <= prev_crossings);
    // // }}}
    //
    // // Log results {{{
    // spdlog::info("{},{},{},{},{}",
    //   g.vertices_count(), g.edges_count(), prev_crossings, new_crossings, ss.str());
    // // }}}

  }; // lamb: test }}}

  // Forwarding test folding lambda {{{
  auto tests = [&]<String... S>(S&&... strs) { (test(std::forward<S>(strs)), ...); };
  // }}}

  // LGSynth 91 tests {{{
  tests(
    // cir::synth_91::alu2,
    // cir::synth_91::alu4,
    // cir::synth_91::dalu,
    // cir::synth_91::apex6,
    // cir::synth_91::apex7,
    cir::synth_91::b1 // ,
    // cir::synth_91::c8 ,
    // cir::synth_91::cc ,
    // cir::synth_91::cht ,
    // cir::synth_91::cm138a,
    // cir::synth_91::cm150a,
    // cir::synth_91::cm151a,
    // cir::synth_91::cm162a,
    // cir::synth_91::cm163a,
    // cir::synth_91::cm42a,
    // cir::synth_91::cm82a,
    // cir::synth_91::cm85a,
    // cir::synth_91::cmb,
    // cir::synth_91::comp,
    // cir::synth_91::cordic,
    // cir::synth_91::cu,
    // cir::synth_91::count,
    // cir::synth_91::decod,
    // cir::synth_91::my_adder
  ); // }}}

} // TEST_CASE: "celaeno::graph::operations::minimize::crossings"

} // namespace celaeno::graph::operations::minimize::crossings::test }}}
