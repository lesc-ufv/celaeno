//
// @author      : Ruan E. Formigoni (ruanformigoni@gmail.com)
// @file        : barycenter
// @created     : quinta jun 11, 2020 10:32:29 -03
//

# pragma once
#include <fplus/fplus.hpp>
#include <range/v3/all.hpp>
#include <concepts>
#include <type_traits>

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
  // Concepts
  //
  // M must has .at with integral param type
  // M must be iterable
  // M must return a iterable
  //    - That supports multiplication
  //    - That supports division
  //    - That supports .at param
  // T must be of an integral type
  template<typename T>
  concept Iterable =
    requires(T t)
    {
      {t.begin()};
      {t.end()};
      {t.cbegin()};
      {t.cend()};
    };

  template<typename T>
  concept Integral = std::is_integral_v<T>;

  template<typename T>
  concept Arithmetic =
    requires(T t)
    {
      { t+t } -> std::same_as<T>;
      { t-t } -> std::same_as<T>;
      { t*t } -> std::same_as<T>;
      { t/t } -> std::same_as<T>;
    };

  template<typename M>
  concept Matrix =
    Iterable<M>
  &&
    requires(M m)
    {
      {m.at(int32_t{})};

      {m.at(int32_t{}).at(int32_t{})};

      {Iterable<decltype(m.at(int32_t{}))>};

      {Arithmetic<decltype( m.at(int32_t{}).at(int32_t{}) )>};
    };

  //
  // Algorithms
  //
  template<Matrix M, Integral T>
  auto row(M&& m,  T i_row)
  {
    auto index_mult = [i=1](auto&& e) mutable { return e*i++; };

    auto a {fw::apply(
      m.at(i_row)
      , fw::transform(index_mult)
      , fw::sum()
    )};

    auto b { fp::sum(m.at(i_row)) };

    return (b != 0)? static_cast<double>(a)/b : 0;
  } // function: row

  template<Matrix M, Integral T>
  auto col(M&& m, T i_col)
  {
    auto index_mult = [i=1](auto&& e) mutable { return e*i++; };

    auto m_col =
      rv::all(m)
      | rv::transform([&i_col](auto&& row){ return row.at(i_col); })
      | rg::to<std::vector>();

    auto a {fw::apply(
      m_col
      , fw::transform(index_mult)
      , fw::sum()
    )};

    auto b { fp::sum(m_col) };

    return (b != 0)? static_cast<double>(a)/b : 0;
  } // function: col

} // namespace celaeno::graph::barycenter
