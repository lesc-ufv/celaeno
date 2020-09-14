// vim: set expandtab fdm=marker ts=2 sw=2 tw=80 et :
//
// @author      : Ruan E. Formigoni (ruanformigoni@gmail.com)
// @file        : incidence
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
#include <range/v3/all.hpp> // TODO remove
#include <celaeno/aliases.hpp>
#include <celaeno/concepts.hpp>
#include <celaeno/graph/concepts.hpp>
#include <celaeno/graph/representations/incidence/impl.hpp>
#include <celaeno/graph/views/proximity.hpp>

// namespace celaeno::graph::representations::incidence {{{
namespace celaeno::graph::representations::incidence
{


// Macros {{{
#define assertm(exp, msg) assert(((void)msg, exp))
// }}}

// Namespaces {{{
namespace rg = ranges;
namespace proximity = celaeno::graph::views::proximity;
namespace incidence = celaeno::graph::representations::incidence::impl;
// }}}

// Using namespaces {{{
using namespace celaeno::concepts;
using namespace celaeno::graph::concepts;
// }}}

// Algorithm {{{
template<SignedIntegral T, typename N1, typename N2>
auto run(T root, N1&& f_pred, N2&& f_succ)
{
    // Proximity view: @level → vertex && @vertex → level
    auto [lvs,_] = proximity::run(root,f_pred,f_succ);

    // Number of levels must be > 1
    assertm(lvs.size() > 1, "Number of layers of the input graph is less 2");

    // Lambda to verify if an edge between u → v exists
    auto f_adjacent = [&](T u, T v){ return rg::contains(f_succ(u),v); };

    // Return the incidence matrix
    return incidence::all(
      std::forward<decltype(lvs)>(lvs),
      std::forward<decltype(f_adjacent)>(f_adjacent)
    );

} // function: run }}}

template<typename L1, typename L2, typename A>
auto run(L1&& l1, L2&& l2, A&& f_adjacent)
{
  return incidence::single(
    std::forward<L1>(l1),
    std::forward<L2>(l2),
    std::forward<A>(f_adjacent)
  );

} // function: run }}}

} // namespace celaeno::graph::representations::incidence }}}
