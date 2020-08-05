//
// @author      : Ruan E. Formigoni (ruanformigoni@gmail.com)
// @file        : concepts
// @created     : sábado jul 25, 2020 21:08:03 -03
//

#pragma once

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
    {m.at(int32_t{})};

    {m.at(int32_t{}).at(int32_t{})};

    {Iterable<decltype(m.at(int32_t{}))>};

    {Arithmetic<decltype( m.at(int32_t{}).at(int32_t{}) )>};
  };

template<typename MS>
concept Matrices =
  Iterable<MS>
&&
  requires(MS ms)
  {
    {ms.at(int32_t{})};

    {ms.at(int32_t{}).at(int32_t{})};

    {ms.at(int32_t{}).at(int32_t{}).at(int32_t{})};

    {Iterable<decltype(ms.at(int32_t{}))>};

    {Arithmetic<decltype( ms.at(int32_t{}).at(int32_t{}) )>};

    {Arithmetic<decltype( ms.at(int32_t{}).at(int32_t{}).at(int32_t{}) )>};
  };


} // namespace celaeno::concepts
