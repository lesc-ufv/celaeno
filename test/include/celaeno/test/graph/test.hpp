// vim: set expandtab fdm=marker ts=2 sw=2 tw=80 et :
//
// @author      : Ruan E. Formigoni (ruanformigoni@gmail.com)
// @file        : test
// @created     : sexta set 04, 2020 02:30:56 -03
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

#include <utility>
#include <maia/circuits/synth-91.hpp>

namespace celaeno::graph::test
{

// namespaces {{{
namespace circ = maia::circuits;
// }}}

template<typename T>
void run(T&& test)
{
  // Forwarding test folding lambda {{{
  auto tests = [&]<typename... S>(S&&... strs) { (test(strs), ...); };
  // }}}

  // LGSynth 91 tests {{{
  tests(
    circ::synth_91::alu2,
    circ::synth_91::alu4,
    circ::synth_91::dalu,
    circ::synth_91::apex6,
    circ::synth_91::apex7,
    circ::synth_91::b1,
    circ::synth_91::c8,
    circ::synth_91::cc,
    circ::synth_91::cht,
    circ::synth_91::cm138a,
    circ::synth_91::cm150a,
    circ::synth_91::cm151a,
    circ::synth_91::cm162a,
    circ::synth_91::cm163a,
    circ::synth_91::cm42a,
    circ::synth_91::cm82a,
    circ::synth_91::cm85a,
    circ::synth_91::cmb,
    circ::synth_91::comp,
    circ::synth_91::cordic,
    circ::synth_91::cu,
    circ::synth_91::count,
    circ::synth_91::decod,
    circ::synth_91::my_adder
  );
} // function: run

  // }}}
} // namespace celaeno::graph::test
