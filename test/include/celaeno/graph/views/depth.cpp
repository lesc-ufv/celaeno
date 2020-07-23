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
#include <celaeno/graph/views/depth.hpp>
#include <taygete/graph/graph.hpp>
#include <taygete/graph/reader.hpp>
#include <maia/circuits/iscas.hpp>
#include <maia/circuits/synth-91.hpp>


namespace celaeno::graph::views::depth::test
{

//
// Aliases
//
namespace cir = maia::circuits;
namespace gra = taygete::graph;
namespace dep = celaeno::graph::views::depth;
using float64_t = double;

//
// Concepts
//

template<typename T>
concept String = requires(T t){ std::string{t}; };

//
// Test Wrapper
//
template<String T>
void TEST(T&& str)
{
  gra::Graph<int64_t> g;
  auto emplace = [&g](auto&& pair){ g.emplace(pair); };
  gra::reader::Reader reader{str,emplace};

  //
  // Helpers
  //
  auto pred = [&g](auto&& v){ return g.get_predecessors(v); };
  auto succ = [&g](auto&& v){ return g.get_successors(v); };

  //
  // Execute
  //
  auto dview{dep::depth(0, pred, succ).second};

} // function: TEST

TEST_CASE("celaeno::graph::views::depth")
{
  TEST(cir::iscas::s27);
} // TEST_CASE: celaeno::graph::views::depth

} // namespace celaeno::graph::views::depth::test
