//
// @author      : Ruan E. Formigoni (ruanformigoni@gmail.com)
// @file        : barycenter
// @created     : quinta jun 11, 2020 10:32:29 -03
//

# pragma once
#include <fplus/fplus.hpp>
#include <range/v3/all.hpp>

namespace celaeno::graph::barycenter
{
  //
  // Aliases
  //
  namespace fp = fplus;
  namespace fw = fplus::fwd;
  namespace rg = ranges;
  namespace rv = ranges::views;

  //
  // Algorithm
  //
  template<typename M, typename T>
  auto row(M&& m,  T i_row)
  {
    auto index_sum = [i=1](auto&& e) mutable { return e*i++; };

    auto a {fw::apply(
      m.at(i_row)
      , fw::transform(index_sum)
      , fw::sum()
    )};

    auto b { fp::sum(m.at(i_row)) };

    return static_cast<double>(a)/b;
  } // function: row

  template<typename M, typename T>
  auto col(M&& m, T i_col)
  {
    auto index_sum = [i=1](auto&& e) mutable { return e*i++; };

    auto m_col =
      rv::all(m)
      | rv::transform([&i_col](auto&& row){ return row.at(i_col); })
      | rg::to<std::vector>();

    auto a {fw::apply(
      m_col
      , fw::transform(index_sum)
      , fw::sum()
    )};

    auto b { fp::sum(m_col) };

    return static_cast<double>(a)/b;
  } // function: col

} // namespace celaeno::graph::barycenter
