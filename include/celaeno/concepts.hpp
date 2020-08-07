//
// @author      : Ruan E. Formigoni (ruanformigoni@gmail.com)
// @file        : concepts
// @created     : sábado jul 25, 2020 21:08:03 -03
//

#pragma once

#include <concepts>
#include <celaeno/aliases.hpp>

namespace celaeno::concepts
{

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
    {m.at(i32{})};

    {m.at(i32{}).at(i32{})};

    {Iterable<decltype(m.at(i32{}))>};

    {Arithmetic<decltype( m.at(i32{}).at(i32{}) )>};
  };

template<typename MS>
concept Matrices =
  Iterable<MS>
&&
  requires(MS ms)
  {
    {ms.at(i32{})};

    {ms.at(i32{}).at(i32{})};

    {ms.at(i32{}).at(i32{}).at(i32{})};

    {Iterable<decltype(ms.at(i32{}))>};

    {Arithmetic<decltype( ms.at(i32{}).at(i32{}) )>};

    {Arithmetic<decltype( ms.at(i32{}).at(i32{}).at(i32{}) )>};
  };


} // namespace celaeno::concepts
