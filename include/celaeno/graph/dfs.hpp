//
// @author      : Ruan E. Formigoni (ruanformigoni@gmail.com)
// @file        : dfs
// @created     : Wednesday Mar 18, 2020 14:52:57 -03
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

#include <stack>
#include <type_traits> // std::remove_reference
#include <functional>  // std::reference_wrapper
#include <range/v3/all.hpp>

namespace celaeno::graph::dfs
{

template<typename T, typename F1, typename F2>
void dfs(T&& root, F1&& get_neighbors, F2&& callback)
{
  using base_type = std::remove_reference_t<T>;

  // Variables
  std::stack<base_type> s;
  std::unordered_map<base_type,bool> t;

  // Algorithm
  s.push(root);

  while ( ! s.empty() )
  {
    auto&& c {s.top()}; s.pop();
    if ( ! t.contains(c) )
    {
      t.emplace(c,true);
      auto&& neighbors {get_neighbors(c)};
      if( callback(c) ) break;
      ranges::for_each(neighbors, [&s](auto&& v){ s.push(v); });
    } // if
  } // while
} // dfs


} // namespace celaeno::graph::dfs
