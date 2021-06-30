// vim: set expandtab fdm=marker ts=2 sw=2 tw=80 et :
//
// @author      : Ruan E. Formigoni (ruanformigoni@gmail.com)
// @file        : 2ddwave
// @created     : Wednesday Jun 30, 2021 02:39:07 UTC
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

#include <vector>
#include <celaeno/aliases.hpp>

#include <spdlog/spdlog.h>

// namespace celaeno::tech::clock_scheme {{{
namespace celaeno::tech::clock_scheme
{

// using namespace {{{
using namespace celaeno::aliases;
// }}}

// Aliases {{{
using Phase = i32;
using Cutout = std::vector<std::vector<Phase>>;
// }}}

// 2DDWave: V. Vankamamidi, M. Ottavi and F. Lombardi, "Clocking and Cell {{{
// Placement for QCA," 2006 Sixth IEEE Conference on Nanotechnology, 2006,
// pp. 343-346, doi: 10.1109/NANO.2006.247647.
struct TwoddWave
{
  private:
    const static Cutout twoddwave;
  public:
    Phase operator()(i64 x, i64 y);
}; // struct: TwoddWave

const Cutout TwoddWave::twoddwave =
{{
  {{0, 1, 2}},
  {{1, 2, 0}},
  {{2, 0, 1}},
}};

Phase TwoddWave::operator()(i64 x, i64 y)
{
  return this->twoddwave.at(x % 3).at(y % 3);
}
// struct: TwoddWave }}}

} // namespace celaeno::tech::clock_scheme }}}
