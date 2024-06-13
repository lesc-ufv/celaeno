// vim: set expandtab fdm=marker ts=2 sw=2 tw=80 et :
//
// @author      : Ruan E. Formigoni (ruanformigoni@gmail.com)
// @file        : fun
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

#pragma once

#include <boost/preprocessor/repetition.hpp>
#include <boost/preprocessor/punctuation/comma_if.hpp>
#include <boost/preprocessor/cat.hpp>
#include <boost/preprocessor/variadic/size.hpp>

// Macros {{{
#define AUTO_FORWARD_DECL(z, n, prefix) BOOST_PP_COMMA_IF(n) [[maybe_unused]] auto&& BOOST_PP_CAT(prefix, BOOST_PP_INC(n))

#define LAMB_1(body) [&]([[maybe_unused]] auto&& _1) { body; }
#define LAMB_2(body, n) [&](BOOST_PP_REPEAT(n, AUTO_FORWARD_DECL, _)) { body; }
#define LAMB_3(body, n, prefix) [&](BOOST_PP_REPEAT(n, AUTO_FORWARD_DECL, prefix)) { body; }

#define GET_MACRO(_1,_2,_3,NAME,...) NAME
#define LV(...) GET_MACRO(__VA_ARGS__, LAMB_3, LAMB_2, LAMB_1)(__VA_ARGS__)

#define LAMBR_1(body) [&]([[maybe_unused]] auto&& _1) -> decltype(auto) { return body; }
#define LAMBR_2(body, n) [&](BOOST_PP_REPEAT(n, AUTO_FORWARD_DECL, _)) -> decltype(auto) { return body; }
#define LAMBR_3(body, n, prefix) [&](BOOST_PP_REPEAT(n, AUTO_FORWARD_DECL, prefix)) -> decltype(auto) { return body; }

#define LR(...) GET_MACRO(__VA_ARGS__, LAMBR_3, LAMBR_2, LAMBR_1)(__VA_ARGS__)
// }}}
