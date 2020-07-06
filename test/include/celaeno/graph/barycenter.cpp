// vim: set expandtab fdm=marker ts=2 sw=2 tw=100 et :

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
#include <fplus/fplus.hpp>

namespace celaeno::graph::barycenter::test
{

  //
  // Aliases
  //
  namespace barycenter = celaeno::graph::barycenter;
  namespace fp = fplus;
  namespace fw = fplus::fwd;

  //
  // Helpers
  //
  template<typename M>
  auto swap_rows_and_cols(M&& m)
  {
    // Get a column by index
    auto col = [&](auto i){return fp::transform([&i](auto&& v){return v.at(i);},m);};

    // Get the number of columns
    size_t sz{};
    try{ sz = m.at(0).size(); }
    catch(std::exception const& e) { std::cerr << "Empty matrix" << std::endl; }

    // Return new container with columns as rows
    return fw::apply(fp::numbers(size_t{},sz),fw::transform([&](auto i){return col(i);}));

  } // function: swap_rows_and_cols

  template<typename V>
  auto compare(V&& v)
  {
    std::stringstream ss;
    ss << std::setprecision(2);
    ss << barycenter::run(v);
    auto exec{std::stod(ss.str())};

    return exec;
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

        // auto col = [&](auto i){return fp::transform([&i](auto&& v){return v.at(i);},m);};


    // Rows
    fw::apply(m0
      , fw::zip(r0)
      , fw::transform([](auto&& e){
          CHECK( compare(e.second) == e.first );
          return EXIT_SUCCESS;
      })
    );

    fw::apply(m1
      , fw::zip(r1)
      , fw::transform([](auto&& e){
          CHECK( compare(e.second) == e.first );
          return EXIT_SUCCESS;
      })
    );

    // Cols
    fw::apply(swap_rows_and_cols(m0)
      , fw::zip(c0)
      , fw::transform([](auto&& e){
          CHECK( compare(e.second) == e.first );
          return EXIT_SUCCESS;
      })
    );

    fw::apply(swap_rows_and_cols(m1)
      , fw::zip(c1)
      , fw::transform([](auto&& e){
          CHECK( compare(e.second) == e.first );
          return EXIT_SUCCESS;
      })
    );

    // compare(m0.at(0),r0.at(0));
    // compare(m0,c0,fn_col);
    // compare(m1.at(0),r1.at(0));
    // compare(m1,c1,fn_col);

  } // TEST_CASE: "celaeno::graph::barycenter"

} // namespace celaeno::graph::barycenter::test
