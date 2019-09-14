// vim: set ts=2 sw=2 tw=0 et :
//
// @company     : Universidade Federal de Viçosa - Florestal
// @author      : Ruan E. Formigoni (ruanformigoni@gmail.com)
// @file        : coarsening
// @created     : Friday Sep 13, 2019 18:16:57 -03
// @license     : MIT
// @description : C++ Algorithms Collection
//

#pragma once

#include <range/v3/all.hpp>
#include <range/v3/all.hpp>
#include <functional> // Ref wrapper
#include <utility>
#include <typeinfo>

namespace celaeno::graph::coarsening
{

template<typename T1, typename T2>
auto coarsening(T2&& map)
{
  using namespace ranges;

  auto i{0};
  auto coarsen = [](auto&& e){ return std::make_pair(e.first, e.second); };
  auto enumerate = [&i](auto&& e){ return std::make_pair(i++,e); };

  auto rng =
      map
    | view::transform(coarsen)
    | view::transform(enumerate);


  return std::map(begin(rng), end(rng));
}

} // namespace celaeno::graph::coarsening
