/**
 * @author      : Ruan E. Formigoni (ruanformigoni@gmail.com)
 * @file        : cantor-pairing
 * @created     : Monday Jan 13, 2020 22:47:39 -03
 */

#pragma once

#include <utility>
#include <type_traits>
#include <cmath>
#include <cstdint>

namespace celaeno::math::cantor_pairing
{

template<typename T>
auto cantor_pairing(T&& z)
{
  using _T = std::remove_reference_t<T>;
  using float32_t = float;
  using float64_t = double;

  if constexpr
    (
      std::is_same_v<_T, std::int32_t> ||
      std::is_same_v<_T, std::int64_t> ||
      std::is_same_v<_T, float32_t>    ||
      std::is_same_v<_T, float64_t>
    )
  {
    auto w { std::floor( ( -1 + std::sqrt( 8 * z + 1 ) ) / 2 ) };
    auto t { ( std::pow(w,2) + w ) / 2 };
    auto k2 { z - t };
    auto k1 { w - k2 };
    return std::make_pair(k1,k2);
  }
  else
  {
    auto const& k1 {z.first};
    auto const& k2 {z.second};
    return .5 * (k1 + k2) * (k1 + k2 + 1) + k2;
  }
}

} // namespace celaeno::math::cantor_pairing

