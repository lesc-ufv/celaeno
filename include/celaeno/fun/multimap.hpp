// vim: set expandtab fdm=marker ts=2 sw=2 tw=80 et :
//
// @author      : Ruan E. Formigoni (ruanformigoni@gmail.com)
// @file        : multimap
// @created     : Monday Aug 16, 2021 13:10:35 UTC
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

#include <map>
#include <optional>
#include <celaeno/aliases.hpp>
#include <celaeno/concepts.hpp>
#include <celaeno/err/err.hpp>

// namespace celaeno::fun::multimap {{{
namespace celaeno::fun::multimap
{

// using namespaces {{{
using namespace celaeno::aliases;
using namespace celaeno::concepts;
using namespace celaeno::err;
// }}}

// fn: find {{{
template<template<typename...> typename M, typename K, typename V>
std::optional<typename M<K,V>::iterator> find(M<K,V> const& m, K k, V v)
  requires IsSpecializationOf<M,std::multimap,std::remove_reference<K>,std::remove_reference<V>>
{
  if ( ! m.contains(k) ) { return std::nullopt; }
  auto it = std::ranges::find_if(m, [&](auto&& e){ return e.first == k && e.second == v; });
  if ( it == m.end() ) { return std::nullopt; }
  return it;
} // function: find }}}

// fn: erase {{{
template<template<typename...> typename M, typename K, typename V>
void erase(M<K,V>& m, K k, V v)
  requires IsSpecializationOf<M,std::multimap,std::remove_reference<K>,std::remove_reference<V>>
{
  if ( auto opt_it = find(m,k,v); opt_it) { m.erase(*opt_it); }
  // else { err::err()("Could not erase pair ({},{})", k, v); }
} // function: erase }}}

// fn: insert {{{
template<template<typename...> typename M, typename K, typename V>
void emplace_if_not_exists(M<K,V>& m, K k, V v)
  requires IsSpecializationOf<M,std::multimap,std::remove_reference<K>,std::remove_reference<V>>
{
  if ( auto opt_it = find(m,k,v); ! opt_it) { m.emplace(k,v); }
} // function: insert }}}

} // namespace celaeno::fun::multimap }}}
