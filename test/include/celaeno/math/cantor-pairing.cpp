/**
 * @author      : Ruan E. Formigoni (ruanformigoni@gmail.com)
 * @file        : cantor-pairing
 * @created     : Monday Jan 13, 2020 23:18:15 -03
 */

#define CATCH_CONFIG_MAIN

#include <catch2/catch.hpp>
#include <utility>
#include <celaeno/math/cantor-pairing.hpp>
#include <random>

namespace celaeno::math::cantor_pairing::test
{

TEST_CASE("Cantor Pairing", "[cantor]")
{
  using namespace celaeno::math::cantor_pairing;

  // Random number generator
  auto gen { std::mt19937(std::random_device{}())  };
  auto dis { std::uniform_int_distribution<int>(0,100) };
  auto rand = [&gen,&dis](){ return dis(gen); };

  SECTION("Retrieve a pair after generating an id")
  {
    for (int i = 0; i < 10; ++i)
    {
      auto [k1,k2] = std::make_pair(rand(),rand());
      auto id {cantor_pairing(std::make_pair(k1,k2))};
      auto restore {cantor_pairing(id)};
      REQUIRE(k1 == restore.first);
      REQUIRE(k2 == restore.second);
    }
  }

  SECTION("Retrieve an id after generating a pair")
  {

    for (int i = 0; i < 10; ++i)
    {
      auto id {rand()};
      auto [k1,k2] = cantor_pairing(id);
      auto _id {cantor_pairing(std::make_pair(k1,k2))};
      REQUIRE(id == _id);
    }
  }
}
} // namespace celaeno::math::cantor_pairing::test
