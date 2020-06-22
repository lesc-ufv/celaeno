//
// @author      : Ruan E. Formigoni (ruanformigoni@gmail.com)
// @file        : imatrix-crossings
// @created     : segunda jun 22, 2020 18:46:09 -03
//

#pragma once

namespace celaeno::graph::imatrix_crossings
{
//
// Concepts
//
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



//
// Algorithm
//
template<Matrix M>
int64_t imatrix_crossings(M&& m)
{
  if( m.size() == 0)
  {
    std::cerr << "Empty incidence matrix" << std::endl;
  }

  // Assumption that level i has its vertices in the
  // rows and level i+1 has its vertices in the columns

  // Number of vertices in the ith layer
  // is equal to the number of rows
  auto vi1{m.size()};

  // Number of vertices in the i+1th layer
  // is equal to the number of columns, take a
  // column and get its size, they should be
  // equal
  auto vi2{m.at(0).size()};

  int64_t c{};

  try
  {
    for (size_t j = 0; j < vi1-1; j++)
    {
      for (size_t k = j+1; k < vi1; k++)
      {
        for (size_t a = 0; a < vi2-1; a++)
        {
          for (size_t b = a+1; b < vi2; b++)
          {
            c += m.at(j).at(b) * m.at(k).at(a);
          } // for: b
        } // for: a
      } // for: k
    } // for: j

  } // try
  catch (std::exception const& e)
  {
    std::cerr << "Degenerate incidence matrix" << std::endl;
  } // catch

  return c;

} // function: imatrix_crossings

} // namespace celaeno::graph::imatrix_crossings
