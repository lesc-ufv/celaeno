// vim: set expandtab fdm=marker ts=2 sw=2 tw=80 et :
//
// @author      : Ruan E. Formigoni (ruanformigoni@gmail.com)
// @file        : dimacs
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

#include <optional>
#include <map>
#include <sstream>
#include <type_traits>
#include <regex>
#include <fstream>

#include <fmt/ranges.h>
#include <spdlog/spdlog.h>
#include <range/v3/all.hpp>

#include <celaeno/aliases.hpp>
#include <celaeno/concepts.hpp>
#include <celaeno/log/log.hpp>
#include <celaeno/fun/fun.hpp>

#include <celaeno/graph/search/bfs.hpp>

// celaeno::graph::io::dimacs
namespace celaeno::graph::io::dimacs
{

namespace
{

// Namespaces
namespace log = celaeno::log;
namespace ns_search = celaeno::graph::search;
namespace ns_log = celaeno::log;
namespace rv = ranges::views;

// Using namespaces
using namespace celaeno::fun::fn;
using namespace celaeno::concepts;
using namespace celaeno::aliases;

} // namespace

// class Writer {{{
class Writer
{
  public:
    Writer( std::multimap<i64,i64> const& edges, auto&& f_pred , auto&& f_succ , String auto&& out);
}; // class: Writer }}}

// Writer::Writer {{{
Writer::Writer( std::multimap<i64,i64> const& edges, auto&& f_pred , auto&& f_succ , String auto&& out)
{
  [[maybe_unused]] ns_log::Timer timer("celaeno::graph::io::dimacs::Writer");

  // Read file
  std::ofstream ofile{out};

  // Check for erros
  log::err({ ofile.good() })("Error to open file {}", out);

  // Get all nodes through bfs
  auto bfs{ns_search::bfs::run(0,f_pred,f_succ)};

  // Normalize node indices to start from 1, and be sequential
  std::map<i64,i64> m_nodes_norm;
  fn(bfs)
    .zip(rv::ints(u64{1},bfs.size()+1) | rg::to<std::vector>)
    .ply([&](auto&& e){ m_nodes_norm[e.first] = e.second; });

  // Write header
  ofile << fmt::format("p edges {} {}\n", bfs.size(), edges.size());

  // Write edges
  for (auto edge : edges)
  {
    ofile << fmt::format("e {} {} 1\n", m_nodes_norm[edge.first], m_nodes_norm[edge.second]);
  } // for

  ofile.close();
} // }}}

} // namespace celaeno::graph::io::dimacs
