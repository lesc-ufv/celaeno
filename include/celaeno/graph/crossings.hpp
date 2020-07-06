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

namespace celaeno::graph::crossings
{
//
// Concepts
//
template<typename T>
concept Iterable =
  requires(T t)
  {
    {t.begin()};
    {t.end()};
    {t.cbegin()};
    {t.cend()};
  };

template<typename T>
concept Arithmetic =
  requires(T t)
  {
    { t+t } -> std::same_as<T>;
    { t-t } -> std::same_as<T>;
    { t*t } -> std::same_as<T>;
    { t/t } -> std::same_as<T>;
  };

template<typename M>
concept Matrix =
  Iterable<M>
&&
  requires(M m)
  {
    {m.at(int32_t{})};

    {m.at(int32_t{}).at(int32_t{})};

    {Iterable<decltype(m.at(int32_t{}))>};

    {Arithmetic<decltype( m.at(int32_t{}).at(int32_t{}) )>};
  };

//
// Algorithm
//
template<Matrix M>
int64_t run(M const& m)
{
  if( m.size() == 0)
  {
    std::cerr << "Empty incidence matrix" << std::endl;
  }

  // Assumption that level i has its vertices in the
  // rows and level i+1 has its vertices in the columns

  // Number of vertices in the ith layer
  // is equal to the number of rows
  auto p{m.size()};

  // Number of vertices in the i+1th layer
  // is equal to the number of columns, take a
  // column and get its size, they should be
  // equal
  auto q{m.at(0).size()};

  int64_t crossings{};

  try
  {
    for (size_t j = 1; j < p-1; j++)
    {
      for (size_t k = j+1; k < p; k++)
      {
        for (size_t a = 1; a < q-1; a++)
        {
          for (size_t b = a+1; b < q; b++)
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

} // function: run

} // namespace celaeno::graph::crossings
