// vim: set expandtab fdm=marker ts=2 sw=2 tw=80 et :
//
// @author      : Ruan E. Formigoni (ruanformigoni@gmail.com)
// @file        : verilog
// @created     : domingo dez 20, 2020 14:00:25 -03
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

#include <string>
#include <celaeno/concepts.hpp>
#include <magic_enum/magic_enum.hpp>
#include <celaeno/string/string.hpp>


namespace ns_concept = celaeno::concepts;
namespace magic = magic_enum;

// namespace ns_enum
namespace ns_enum
{

// Concepts
template<typename T>
concept Enum = std::is_enum_v<std::remove_cvref_t<T>>;

// from_string() {{{
template<Enum U, ns_concept::StreamInsertable T>
inline decltype(auto) from_string(T&& t)
{
  auto opt = magic::enum_cast<U>(ns_string::to_string(t), magic::case_insensitive);

	if ( ! opt ) { throw std::runtime_error("Could not convert enum"); } // if

	return U{*opt};
} // }}}

// to_string() {{{
template<Enum T>
inline std::string to_string(T&& t)
{
	return std::string(magic::enum_name(std::forward<T>(t)));
} // to_string() }}}

// to_string_lower() {{{
template<Enum T>
inline std::string to_string_lower(T&& t)
{
	return ns_string::to_lower(ns_string::to_string(magic::enum_name(std::forward<T>(t))));
} // to_string_lower() }}}

// check_and() {{{
template<ns_concept::Enum T, ns_concept::Enum... Args>
constexpr bool check_and(T&& t, Args&&... flags)
{
  return static_cast<int>(t) & ( static_cast<int>(flags) & ... );
} // check_and() }}}

} // namespace ns_enum

/* vim: set expandtab fdm=marker ts=2 sw=2 tw=100 et :*/
