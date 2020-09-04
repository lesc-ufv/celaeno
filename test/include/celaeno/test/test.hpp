// vim: set expandtab fdm=marker ts=2 sw=2 tw=80 et :
//
// @author      : Ruan E. Formigoni (ruanformigoni@gmail.com)
// @file        : test-template
// @created     : quinta set 03, 2020 19:54:21 -03
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

#include <chrono>
#include <sstream>
#include <celaeno/aliases.hpp>
#include <celaeno/concepts.hpp>

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest/doctest.h>

#include <spdlog/spdlog.h>
#include <spdlog/sinks/basic_file_sink.h>

// namespace celaeno::test {{{
namespace celaeno::test
{

// using namespaces {{{
using namespace celaeno::concepts;
// }}}

// runtime {{{
template<typename F>
decltype(auto) runtime(F&& f_test)
{
  auto start {std::chrono::system_clock::now()};
  auto result{f_test()};
  auto end {std::chrono::system_clock::now()};
  std::chrono::duration<f64> dur {end-start};
  std::stringstream ss; ss << dur.count();
  return std::make_pair(result,ss.str());
} // function: runtime }}}

// runtime void_t {{{
template<typename F>
decltype(auto) runtime_vt(F&& f_test)
{
  auto start {std::chrono::system_clock::now()};
  f_test();
  auto end {std::chrono::system_clock::now()};
  std::chrono::duration<f64> dur {end-start};
  std::stringstream ss; ss << dur.count();
  return ss.str();
} // function: runtime_vt }}}

// logger {{{

struct LoggerParams
{
  std::string name;
  std::string path;
  std::string info;
  std::string pattern;
}; // struct: LoggerParams

void logger_new(LoggerParams params)
{
  auto logger {spdlog::basic_logger_mt(params.name, params.path, true)};
  spdlog::set_default_logger(logger);
  spdlog::info(params.info);
  spdlog::set_pattern(params.pattern);
} // function: logger

template<String S, typename... Args>
void logger_write(S&& s, Args&&... args)
{
  spdlog::info(s, std::forward<Args>(args)...);
}
// }}}

// fn: checks {{{
template<typename... Args>
decltype(auto) check(Args... args)
{
  auto test = [](auto&& condition) -> void { CHECK(condition); };
  (test(args),...);
} // function: check }}}

// fn: requires {{{
template<typename... Args>
decltype(auto) require(Args... args)
{
  auto test = [](auto&& condition) -> void { REQUIRE(condition); };
  (test(args),...);
} // function: require }}}

} // }}}
