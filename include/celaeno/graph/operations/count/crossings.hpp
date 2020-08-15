// vim: set expandtab fdm=marker ts=2 sw=2 tw=100 et :
//
// @author      : Ruan E. Formigoni (ruanformigoni@gmail.com)
// @file        : crossings
// @created     : segunda jun 22, 2020 18:46:09 -03
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

#include <concepts>
#include <utility>
#include <exception>
#include <iostream>
#include <celaeno/concepts.hpp>
#include <celaeno/aliases.hpp>

// namespace celaeno::graph::operations::count::crossings {{{

namespace celaeno::graph::operations::count::crossings
{

// Namespaces {{{
using namespace celaeno::concepts;
// }}}

// Algorithm {{{

// Impl {{{
template<Matrix M>
decltype(auto) impl(M&& m)
{
  // Count the number of crossings
  i64 crossings{};

  // Check if matrix is empty
  if( m.empty() ) { return crossings; }

  // Number of rows
  auto p{m.size()};

  // Number of columns
  auto q{m.at(0).size()};

  try
  {
    for (siz j{0}; j < p-1; j++)
    {
      for (siz k{j+1}; k < p; k++)
      {
        for (siz a{0}; a < q-1; a++)
        {
          for (siz b{a+1}; b < q; b++)
          {
            crossings += m.at(j).at(b) * m.at(k).at(a);
          } // for: b
        } // for: a
      } // for: k
    } // for: j
  } // try
  catch (std::exception const& e)
  {
    std::cerr << "Degenerate incidence matrix" << std::endl;
  } // catch
  return crossings;
} // function: impl }}}

// Variadic parameters {{{
template<typename... MS>
i64 run(MS&&... ms)
{
  return (impl(std::forward<MS>(ms)) + ...);
} // function: run }}}

// }}}

} // namespace celaeno::graph::operations::count::crossings }}}
