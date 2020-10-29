// vim: set expandtab fdm=marker ts=2 sw=2 tw=80 et :
//
// @author      : Ruan E. Formigoni (ruanformigoni@gmail.com)
// @file        : functional
// @created     : quarta set 23, 2020 17:45:55 -03
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

// includes {{{
#include <utility>
#include <vector>
#include <celaeno/concepts.hpp>
// }}}

// namespace celaeno::functional {{{
namespace celaeno::functional
{
  // using namespaces {{{
  using namespace celaeno::concepts;
  using namespace celaeno::functional;
  // }}}

  // fn: zip {{{
  auto zip = []<Iterable C1, Iterable C2>(C1 const& c1, C2 const& c2)
    requires
       requires(C1){ typename std::decay_t<C1>::value_type; }
    && requires(C2){ typename std::decay_t<C2>::value_type; }
  {

    using T1 = typename C1::value_type;
    using T2 = typename C2::value_type;

    std::vector<std::pair<T1,T2>> zipped;

    for (auto it1{c1.begin()}, it2{c2.begin()}; it1 != c1.end() && it2 != c2.end(); ++it1, ++it2)
    {
      zipped.emplace_back(*it1,*it2);
    } // for

    return zipped;
  }; // }}}

  // fn: values {{{
  template<Map M>
  decltype(auto) values(M&& m)
  {
    using T = typename std::decay_t<M>::mapped_type;

    std::vector<T> values;

    for (auto it{m.cbegin()}; it != m.cend(); ++it)
    {
      values.emplace_back(it->second);
    } // for: it != m.cend()

    return values;
  } // function: values }}}

} // namespace celaeno::functional }}}
