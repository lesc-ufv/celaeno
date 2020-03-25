//
// @author      : Ruan E. Formigoni (ruanformigoni@gmail.com)
// @file        : balance
// @created     : Friday Mar 13, 2020 18:31:22 -03
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
#include <taygete/graph.hpp>
#include <taygete/graph-reader.hpp>
#include <asterope/graph/to-dot.hpp>
#include <asterope/graph/to-png.hpp>
#include <celaeno/graph/balance.hpp>
#include <celaeno/graph/views/depth.hpp>
#include "circuits.hpp"

namespace celaeno::graph::balance::test
{

template<typename T1, typename T2>
void run(T1&& circuit, T2&& alias)
{
  // Define output names
  std::string original{std::string{alias}};
  std::string balanced{std::string{alias} + "-balanced"};

  // Use taygete data structure
  taygete::graph::Graph<int64_t> g;

  // Read circuit into a stream
  std::stringstream ss; ss << circuit;

  // Callback for populating the graph
  auto callback = [&g]<typename T>(T&& a, T&& b) -> void
  {
    g.emplace(std::make_pair(std::forward<T>(a),std::forward<T>(b)));
  };

  // Read the graph into g
  taygete::graph::reader::Reader reader(ss, callback);

  // Write graph to .png file
  // asterope::graph::to_png::to_png(
  //   asterope::graph::to_dot::to_dot(g.data()),
  //   original
  // );

  // Create a depth view
  auto g_views
  {
    celaeno::graph::views::depth::depth(
      0,
      [&g](auto&& e){ return g.get_successors(std::forward<decltype(e)>(e)); },
      [&g](auto&& e){ return g.get_predecessors(std::forward<decltype(e)>(e)); }
    )
  };
  auto& d_view{g_views.first};

  auto pred = [&g]<typename T>(T&& e)
    { return g.get_predecessors(std::forward<T>(e)); };

  int64_t depth{};
  if( ! d_view.empty() )
  {
    depth = ( d_view.rbegin()->first ) + 1;
  }
  else
  {
    std::cerr << " * Empty map" << std::endl;
    exit(1);
  }

  // Balace the graph
  celaeno::graph::balance::balance(d_view, g, depth, pred);

  // Write the balance graph to .png file
  asterope::graph::to_png::to_png(
    asterope::graph::to_dot::to_dot(
      g.data(),
      [](auto&& os, auto&& k, auto& v)
        {
          os << k << " -> " << v << std::endl;
        }
    ),
    balanced
  );
}

TEST_CASE("balance.cpp")
{

// SECTION("C17")
// {
//   run(c17,"c17");
// } // SECTION
//
// SECTION("t")
// {
//   run(t,"t");
// } // SECTION
//
// SECTION("1-bit adder AIOG")
// {
//   run(adder_1_bit_aoig,"1-bit-adder-aoig");
// } // SECTION
//
// SECTION("b1-r2")
// {
//   run(b1_r2,"b1-r2");
// } // SECTION
//
// SECTION("clpl")
// {
//   run(clpl,"clpl");
// } // SECTION
//
// SECTION("newtag")
// {
//   run(newtag,"newtag");
// } // SECTION
//
SECTION("xor_5")
{
  run(xor5_r,"xor5_r");
} // SECTION
//
// SECTION("c432")
// {
//   run(c432,"c432");
// } // SECTION
//
// SECTION("c499")
// {
//   run(c499,"c499");
// } // SECTION: c499

// SECTION("c3540")
// {
//   run(c3540,"c3540");
// } // SECTION: "c3540"

} // TEST_CASE

} // namespace celaeno::graph::balance::test
