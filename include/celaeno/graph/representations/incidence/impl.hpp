// vim: set expandtab fdm=marker ts=2 sw=2 tw=80 et :
//
// @author      : Ruan E. Formigoni (ruanformigoni@gmail.com)
// @file        : incidence-matrix
// @created     : domingo jun 14, 2020 12:17:32 -03
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

#pragma once

#include <vector>
#include <concepts>
#include <iterator>
#include <range/v3/all.hpp>
#include <fplus/fplus.hpp>
#include <celaeno/aliases.hpp>
#include <celaeno/concepts.hpp>
#include <celaeno/graph/concepts.hpp>

// namespace celaeno::graph::representations::incidence::impl {{{
namespace celaeno::graph::representations::incidence::impl
{

// Namespaces {{{
namespace fp = fplus;
// }}}

// Using namespaces {{{
using namespace celaeno::concepts;
using namespace celaeno::graph::concepts;
// }}}

// Algorithm {{{
template<Layer L, Adjacent A>
auto run(L&& layer, A&& adjacent, u64 height)
{
  using Matrix = std::vector<std::vector<bool>>;

  // Result
  std::vector<Matrix> result;

  for (u64 i{0}; i < height-1; ++i)
  {
    // Get two layers
    auto [l1,l2] = std::make_pair(layer(i),layer(i+1));

    // Create the incidence matrix
    Matrix m( l1.size(), std::vector<bool>(l2.size(), 0) );

    // Enumerate layers
    auto [l1e,l2e] { std::make_pair(fp::enumerate(l1),fp::enumerate(l2)) };

    // For each node of layer 1
    // check if edge exists for each edge of layer 2
    // If edge exists, assign 1 to matrix
    // If edge does not exist, do nothing
    for (auto&& u : l1e)
    {
      for (auto&& v : l2e)
      {
        if( adjacent(u.second,v.second) )
        {
          m.at(u.first).at(v.first) = 1;
        }
      } // for l2e
    } // for l1e

    result.push_back(m);

  } // for: i

  return result;
} // function: run }}}

} // namespace celaeno::graph::representations::incidence::impl }}}
