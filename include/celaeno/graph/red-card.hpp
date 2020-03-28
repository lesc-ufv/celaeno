//
// @author      : Ruan E. Formigoni (ruanformigoni@gmail.com)
// @file        : reduce-cardinality
// @created     : Thursday Mar 26, 2020 16:04:44 -03
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
#include <vector>
#include <algorithm>
#include <map>
#include <range/v3/all.hpp>
#include <fplus/fplus.hpp>
#include <celaeno/graph/bfs.hpp>

namespace celaeno::graph::red_card
{

namespace rg = ranges;
namespace rv = ranges::views;
namespace ra = ranges::actions;
using namespace fplus;

template<typename T1, typename T2>
decltype(auto) operator|=(T1&& lhs, T2&& rhs)
{
  return lhs | rhs | rg::to<std::vector> ;
}

template<typename F, typename... Args>
auto apply_as_lvalue(F&& f, Args&&... args)
{
  return f(std::forward<Args const&>(args)...);
}

template<
  typename T1,
  typename F1,
  typename F2,
  typename F3,
  typename F4
>
auto red_card(
  T1&& root,
  F1&& pred,
  F2&& succ,
  F3&& link,
  F4&& unlink,
  int64_t start = -1
)
{

  auto adj = [&pred,&succ](auto&& v)
  {
    return apply_as_lvalue(rv::concat,pred(v),succ(v)) | rg::to<std::vector>;
  };

  auto bfs {celaeno::graph::bfs::bfs(0,adj,[](auto&& v){return false;})};

  //
  // Predecessors balancing
  //

  for (auto&& v : bfs)
  {
    std::cout << "Parent: " << v << " ";
    // Get the successors
    auto s{succ(v)};
    // Group the successors into pairs
    auto p {fplus::split_every(2,s)};
    // Remove the ones with < 2 vertices
    auto f {fplus::drop_if([](auto&& e){ return e.size() < 2; },p)};
    // Convert (list -> list) -> (list -> pairs)
    auto t {f | rv::transform([](auto&& v)
      {return std::make_pair(v.at(0),v.at(1));})};
    // Unlink parent from children
    rg::for_each(t, [&v,&unlink](auto&& pair)
      {
        unlink(std::make_pair(v,pair.first));
        unlink(std::make_pair(v,pair.second));
      });
    // Connect each pair to a pseudonode
    // and the pseudonode to parent
    rg::for_each(t, [&link,&v,&start](auto&& pair)
      {
        link(std::make_pair(start,pair.first));
        link(std::make_pair(start,pair.second));
        link(std::make_pair(v,start));
        --start;
      });
    rg::for_each(t,
      [](auto&& pair){std::cout << "Children: " << pair.first << " " << pair.second << " | "; }
    );
    std::cout << std::endl;
  } // for bfs

} // function: red_card

} // namespace celaeno::graph::red_card
