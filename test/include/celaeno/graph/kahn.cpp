// vim: set expandtab fdm=marker ts=2 sw=2 tw=100 et :
//
// @author      : Ruan E. Formigoni (ruanformigoni@gmail.com)
// @file        : kahn
// @created     : Wednesday Apr 08, 2020 13:22:58 -03
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
#include <fplus/fplus.hpp>
#include <celaeno/graph/kahn.hpp>
#include <celaeno/aliases.hpp>
#include <taygete/graph/graph.hpp>
#include <taygete/graph/reader.hpp>
#include <maia/circuits/iscas.hpp>
#include <maia/circuits/synth-91.hpp>


// namespace celaeno::graph::kahn::test {{{

namespace celaeno::graph::kahn::test
{

// namespaces {{{

namespace cir = maia::circuits;
namespace graph = taygete::graph;
namespace fw = fplus::fwd;

// }}}

// Concepts {{{

template<typename T>
concept String = requires(T t){ std::string{t}; };

// }}}

// Test Wrapper {{{

template<String T>
void TEST(T&& str)
{
  graph::Graph<int64_t> g;
  auto emplace = [&g](auto&& pair){ g.emplace(pair); };
  graph::reader::Reader{std::string(str),emplace};

  // Helpers
  auto pred = [&g](auto&& v){ return g.predecessors(v); };
  auto succ = [&g](auto&& v){ return g.successors(v); };

  // Execution
  auto result {celaeno::graph::kahn::run(0,pred,succ)};

  // Check if graph was populated
  REQUIRE(g.vertices_count() > 0);

  // Check if no nodes are missing
  auto adj = [&g](auto&& v){ return g.neighbors(v); };
  auto bfs {bfs::run(0,adj)};
  REQUIRE(g.vertices_count() == result.size());

  // Check if no nodes are duplicates
  REQUIRE(fw::apply(result,fw::unique()).size() == result.size());

} // function: TEST }}}

// Test Case celaeno::graph::kahn {{{

TEST_CASE("celaeno::graph::kahn"
  * doctest::description("Kahn's algorithm test")
  * doctest::timeout(10.0f)
)
{
  // Logger
  auto logger {spdlog::basic_logger_mt("graph::kahn", "logs/graph-kahn.txt")};
  spdlog::set_default_logger(logger);
  auto start {std::chrono::system_clock::now()};

  // LGSynth 91
  TEST(cir::synth_91::alu2);
  TEST(cir::synth_91::alu4);
  TEST(cir::synth_91::dalu);
  TEST(cir::synth_91::apex6);
  TEST(cir::synth_91::apex7);
  TEST(cir::synth_91::b1);
  TEST(cir::synth_91::c8);
  TEST(cir::synth_91::cc);
  TEST(cir::synth_91::cht);
  TEST(cir::synth_91::cm138a);
  TEST(cir::synth_91::cm150a);
  TEST(cir::synth_91::cm151a);
  TEST(cir::synth_91::cm162a);
  TEST(cir::synth_91::cm163a);
  TEST(cir::synth_91::cm42a);
  TEST(cir::synth_91::cm82a);
  TEST(cir::synth_91::cm85a);
  TEST(cir::synth_91::cmb);
  TEST(cir::synth_91::comp);
  TEST(cir::synth_91::cordic);
  TEST(cir::synth_91::cu);
  TEST(cir::synth_91::count);
  TEST(cir::synth_91::decod);
  TEST(cir::synth_91::my_adder);

  // Log duration
  auto end {std::chrono::system_clock::now()};
  std::chrono::duration<f64> dur {end-start};
  std::stringstream ss; ss << dur.count();
  spdlog::info("Duration for khan.cpp: {}", ss.str());

} // TEST_CASE: celaeno::graph::kahn }}}

} // namespace celaeno::graph::kahn::test }}}
