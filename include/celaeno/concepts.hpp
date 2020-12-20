// vim: set expandtab fdm=marker ts=2 sw=2 tw=80 et :
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
#include <string>
#include <celaeno/aliases.hpp>

// namespace celaeno::concepts {{{
namespace celaeno::concepts
{

template<typename T, typename U = std::decay_t<T>>
concept Integral = std::integral<U>;

template<typename T, typename U = std::decay_t<T>>
concept SignedIntegral = std::signed_integral<U>;

template<typename T, typename U = std::decay_t<T>>
concept Float = std::floating_point<U>;

template<typename T, typename U = std::decay_t<T>>
concept String =
requires(U u) { { std::string{u} } -> std::same_as<std::string>; };

template<typename T>
concept Range = std::ranges::range<T>;

template<typename T, typename U>
concept SameAs = std::same_as<T,U>;

template<typename T, typename U>
concept ConvertibleTo = std::convertible_to<std::decay_t<T>,std::decay_t<U>>;

template<typename T, typename U>
concept IsPairOf =
requires(U u)
{
  {u.first} -> std::convertible_to<std::decay_t<T>>;
  {u.second} -> std::convertible_to<std::decay_t<T>>;
};

template<typename T, typename... U>
concept IsPairsOf =
requires(U... u)
{
  { ((u.first),...) } -> std::convertible_to<std::decay_t<T>>;
  { ((u.second),...) } -> std::convertible_to<std::decay_t<T>>;
};

template<typename F, typename... V>
concept CallableWith = requires(F f, V... v) { f(std::forward<V>(v)...); };

template<typename T, typename U = std::decay_t<T>>
concept Iterable = requires{ std::input_iterator<U> && std::incrementable<U>; };

template<typename T, typename U = std::decay_t<T>>
concept ForwardIterator = std::forward_iterator<U>;

template<typename T, typename U = std::decay_t<T>>
concept Arithmetic =
requires(U u)
{
  { u+u } -> std::same_as<U>;
  { u-u } -> std::same_as<U>;
  { u*u } -> std::same_as<U>;
  { u/u } -> std::same_as<U>;
};

template<typename T>
concept Map =
requires
{
  typename std::decay_t<T>::key_type;
  typename std::decay_t<T>::mapped_type;
};

template<typename V>
concept Vector =
Iterable<V> && requires(V&& v){ typename std::decay_t<V>::value_type; };

template<typename M>
concept Matrix =
Vector<M> && Iterable<typename std::decay_t<M>::value_type::value_type>;

} // namespace celaeno::concepts }}}
