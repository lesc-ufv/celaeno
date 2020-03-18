// vim: set ts=2 sw=2 tw=0 et :
//
// @author      : Ruan E. Formigoni (ruanformigoni@gmail.com)
// @file        : hwm
// @created     : Saturday Aug 17, 2019 17:32:57 -03
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
#include <celaeno/graph/bfs.hpp>
#include <celaeno/graph/hvm.hpp>
#include <taygete/graph.hpp>

namespace celaeno::graph::hvm::test
{

TEST_CASE("Heavy-vertex Matching", "[hvm]")
{
  using namespace taygete::graph;

  SECTION("Verify if for each vertex there is only one matching")
  {
    Graph<int32_t> chebyshev
    {
      {0,2},{0,3},{1,3},
      {2,4},{2,5},{3,5},
      {4,7},{4,8},{5,9},{6,9},
      {7,10},{8,11},{9,11},
      {10,12},{10,13},{11,13},
      {12,15},{13,16},{14,16},
      {15,17},{16,17},
      {17,18}
    };

    auto res = celaeno::graph::hvm::hvm(0,
      [&chebyshev]( auto const& node ) -> auto
      {
        return chebyshev.get_adjacent(node);
      }
    );
  }
}

} // namespace celaeno::graph::hwm::test
