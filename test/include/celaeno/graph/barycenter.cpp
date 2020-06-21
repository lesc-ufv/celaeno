//
// @author      : Ruan E. Formigoni (ruanformigoni@gmail.com)
// @file        : barycenter
// @created     : sábado jun 20, 2020 18:21:24 -03
//

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest/doctest.h>
#include <celaeno/graph/barycenter.hpp>
#include <sstream>
#include <iomanip>

namespace celaeno::graph::barycenter::test
{

  //
  // Aliases
  //
  namespace barycenter = celaeno::graph::barycenter;

  //
  // Helpers
  //
  template<typename Arr, typename M, typename Fn>
  void compare(M&& m, Arr&& arr, Fn&& fn)
  {
    std::stringstream ss;
    ss << std::setprecision(2);

    for (size_t i{0}; i < arr.size(); ++i)
    {
      ss << fn(m,i);
      auto res{std::stod(ss.str())};

      CHECK( res == arr.at(i) );

      ss.str("");
      ss.clear();
    } // for arr
  } // function: compare

  //
  // Tests
  //
  TEST_CASE("celaeno::graph::barycenter")
  {
    std::array<std::array<int32_t,5>,4> m0
    {{
      {1,1,0,0,0},
      {1,0,0,1,1},
      {0,1,0,1,1},
      {1,0,1,0,1},
    }};
    std::array<double,4> r0 {1.5,3.3,3.7,3};
    std::array<double,5> c0 {2.3,2,4,2.5,3};

    std::array<std::array<int32_t,5>,4> m1
    {{
      {1,1,0,1,0},
      {0,1,1,0,0},
      {0,1,0,1,1},
      {0,0,1,1,1},
    }};
    std::array<double,4> r1 {2.3, 2.5, 3.7, 4};
    std::array<double,5> c1 {1, 2, 3, 2.7, 3.5};

    auto fn_row = []<typename M>(M&& m, auto i)
    {
      return barycenter::row(std::forward<M>(m), i);
    };

    auto fn_col = []<typename M>(M&& m, auto i)
    {
      return barycenter::col(std::forward<M>(m), i);
    };

    compare(m0,r0,fn_row);
    compare(m0,c0,fn_col);
    compare(m1,r1,fn_row);
    compare(m1,c1,fn_col);

  } // TEST_CASE: "celaeno::graph::barycenter"

} // namespace celaeno::graph::barycenter::test
