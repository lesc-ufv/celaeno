//
// @author      : Ruan E. Formigoni (ruanformigoni@gmail.com)
// @file        : reduce-cardinality
// @created     : Thursday Mar 26, 2020 16:05:14 -03
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


#define CATCH_CONFIG_MAIN

#include <catch2/catch.hpp>
#include <celaeno/graph/red-card.hpp>
#include <taygete/graph.hpp>
#include <taygete/graph-reader.hpp>
#include <asterope/graph/to-dot.hpp>
#include <asterope/graph/to-png.hpp>
#include <range/v3/all.hpp>
#include <chrono>
#include "circuits.hpp"

TEST_CASE("reduce-cardinality","[reduce-cardinality.hpp]")
{
  namespace rg = ranges;
  namespace rv = ranges::views;
  namespace ra = ranges::actions;
  using float64_t = double;
  //
  // Read graph
  //
  taygete::graph::Graph<int64_t> g;
  std::stringstream ss; ss << c3540;
  auto callback = [&g]<typename T>(T&& a, T&& b) -> void
  {
    g.emplace(std::make_pair(std::forward<T>(a),std::forward<T>(b)));
  };
  taygete::graph::reader::Reader reader(ss, callback);


  //
  // Provide operations
  //
  auto pred = [&g]<typename T>(T&& v) -> auto
    { return g.get_predecessors(v); };
  auto succ = [&g]<typename T>(T&& v) -> auto
    { return g.get_successors(v); };
  auto link = [&g]<typename T>(T&& pair) -> void
    { g.emplace(pair); };
  auto unlink = [&g]<typename T>(T&& pair) -> void
    { g.erase_edge(pair); };

  // Write the balance graph to .png file
  // asterope::graph::to_png::to_png(
  //   asterope::graph::to_dot::to_dot(
  //     g.data(),
  //     [](auto&& os, auto&& k, auto& v)
  //       {
  //         os << k << " -> " << v << std::endl;
  //       }
  //   ),
  //   "green-card"
  // );
  //
  // Reduce card
  //
  auto start {std::chrono::system_clock::now()};
  celaeno::graph::red_card::red_card(0,pred,succ,link,unlink);
  auto end {std::chrono::system_clock::now()};
  std::chrono::duration<float64_t> ellapsed_seconds {end-start};
  std::cout << "Execution time: " << ellapsed_seconds.count() << std::endl;

  // Write the balance graph to .png file
  // asterope::graph::to_png::to_png(
  //   asterope::graph::to_dot::to_dot(
  //     g.data(),
  //     [](auto&& os, auto&& k, auto& v)
  //       {
  //         os << k << " -> " << v << std::endl;
  //       }
  //   ),
  //   "red-card"
  // );

  // std::cout << res << std::endl;

} // TEST_CASE: red-card
