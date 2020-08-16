// vim: set expandtab fdm=marker ts=2 sw=2 tw=100 et :
//
// @author      : Ruan E. Formigoni (ruanformigoni@gmail.com)
// @file        : concepts
// @created     : sábado jul 25, 2020 21:08:03 -03
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
#include <type_traits>
#include <iterator>
#include <celaeno/aliases.hpp>

// namespace celaeno::concepts {{{
namespace celaeno::concepts
{

template<typename T>
concept Integral = std::integral<T>;

template<typename T>
concept SignedIntegral = std::signed_integral<T>;

template<typename T>
concept Iterable = requires{ std::input_iterator<T> && std::incrementable<T>; };

template<typename T>
concept Arithmetic =
  requires(T t)
  {
    { t+t } -> std::same_as<T>;
    { t-t } -> std::same_as<T>;
    { t*t } -> std::same_as<T>;
    { t/t } -> std::same_as<T>;
  };

template<typename V>
concept Vector =
  Iterable<V>
&&
  requires(V v)
  {
    {v.at(int32_t{})};

    {Arithmetic<decltype( v.at(int32_t{}) )>};
  };

template<typename M>
concept Matrix =
  Iterable<M>
&&
  requires(M m)
  {
    {m.at(i32{})};

    {m.at(i32{}).at(i32{})};

    {Iterable<decltype(m.at(i32{}))>};

    {Arithmetic<decltype( m.at(i32{}).at(i32{}) )>};
  };

template<typename MS>
concept Matrices =
  Iterable<MS>
&&
  requires(MS ms)
  {
    {ms.at(i32{})};

    {ms.at(i32{}).at(i32{})};

    {ms.at(i32{}).at(i32{}).at(i32{})};

    {Iterable<decltype(ms.at(i32{}))>};

    {Arithmetic<decltype( ms.at(i32{}).at(i32{}) )>};

    {Arithmetic<decltype( ms.at(i32{}).at(i32{}).at(i32{}) )>};
  };


} // namespace celaeno::concepts }}}
