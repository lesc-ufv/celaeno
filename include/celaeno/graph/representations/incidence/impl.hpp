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
#include <celaeno/aliases.hpp>
#include <celaeno/concepts.hpp>

// namespace celaeno::graph::representations::incidence::impl {{{
namespace celaeno::graph::representations::incidence::impl
{

// Using namespaces {{{
using namespace celaeno::concepts;
using namespace celaeno::aliases;
// }}}

// Namespaces {{{
namespace rg = ranges;
// }}}

// Type aliases {{{
using Matrix = std::vector<std::vector<bool>>;
// }}}

// Algorithm {{{

// fn: single {{{
template<Range R, typename A>
auto single(R&& l1, R&& l2, A&& f_adjacent)
  requires CallableWith<A,i64,i64>
{
  // Create the incidence matrix
  Matrix m(l1.size(),std::vector<bool>(l2.size(),0));

  // If edge exists, 1 else 0
  rg::for_each(l1,
  [&,i=0](auto&& u) mutable
  {
    rg::for_each(l2,[&,j=0](auto&& v) mutable
    {
      if( f_adjacent(u,v) ){ m.at(i).at(j) = 1; }
      ++j;
    });
    ++i;
  });

  // A single incidence matrix
  return m;

} // function: single }}}

// fn: all {{{
template<Range R, typename A>
auto all(R&& layers, A&& f_adjacent)
  requires CallableWith<A,i64,i64>
{
  // Result is the representation of the grpah as an incidence matrix
  std::vector<Matrix> result;

  // Create all incidence matrices
  for (auto&& it{layers.cbegin()}; it != std::prev(layers.cend()); ++it)
  {
    result.emplace_back(single(it->second,std::next(it)->second,f_adjacent));
  } // for: it != layers.cend()

  // All incidence matrices
  return result;

} // function: all }}}

// }}}

} // namespace celaeno::graph::representations::incidence::impl }}}
