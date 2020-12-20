//
// @author      : Ruan E. Formigoni (ruanformigoni@gmail.com)
// @file        : graph
// @created     : sunday dec 20, 2020 04:02:05 -03
//
// BSD 2-Clause License

// Copyright (c) 2020, Ruan Evangelista Formigoni
// All rights reserved.

// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:

// * Redistributions of source code must retain the above copyright notice, this
//   list of conditions and the following disclaimer.

// * Redistributions in binary form must reproduce the above copyright notice,
//   this list of conditions and the following disclaimer in the documentation
//   and/or other materials provided with the distribution.

// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
// FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
// DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
// SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
// CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
// OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.


#pragma once

#include <vector>
#include <map>
#include <memory>
#include <functional>
#include <type_traits>
#include <concepts>
#include <utility>
#include <celaeno/concepts.hpp>
#include <range/v3/all.hpp>
#include <fplus/fplus.hpp>

// {{{ celaeno::graph
namespace celaeno::graph
{

// Using namespace {{{
using namespace celaeno::concepts;
// }}}

// Namespaces {{{
namespace rg = ranges;
namespace fp = fplus;
namespace fw = fplus::fwd;
// }}}

// Aliases {{{
template<typename T>
using Vertices = std::multimap<T,T>;
template<typename T>
using Storage = std::unique_ptr<Vertices<T>>;
// }}}

// Graph {{{

template<Arithmetic T>
class Graph
{
  // Private Members {{{
  private:
    Storage<T> g;
  // }}}

  // Public Members {{{
  public:
    using value_type = T;
    using reference = T&;
    using const_reference = T const&;
  // }}}

  // Constructors {{{
  public:
    Graph() noexcept;
    Graph(Graph const& src) = delete;
    Graph(Graph&& src) = delete;
    Graph(std::initializer_list<std::pair<T,T>> t) noexcept;
  // }}}

  // Public Methods {{{
    // Element Access {{{
    template<typename U> requires ConvertibleTo<T,U>
    std::vector<T> successors(U u) const;
    template<typename U> requires ConvertibleTo<T,U>
    std::vector<T> predecessors(U u) const;
    template<typename U> requires ConvertibleTo<T,U>
    std::vector<T> neighbors(U u) const;
    template<typename U> requires ConvertibleTo<T,U>
    bool adjacent(U u1, U u2) const;
    Vertices<T>& data() noexcept;
    Vertices<T> const& data() const noexcept;
    // }}}

    // Capacity {{{
    std::size_t vertices_count() const;
    std::size_t edges_count() const;
    // }}}

    // Modifiers {{{
    template<typename... U> requires IsPairsOf<T,U...>
    void emplace(U&&... u);
    template<typename U = std::pair<T,T>> requires IsPairOf<T,U>
    void erase(U&& u);
    // }}}

    // Operators {{{
    Graph<T> operator=(Graph<T> const& rhs);
    Graph<T> operator=(Graph<T>&& rhs);
    // }}}

  // }}}
};

// }}}

// Constructors {{{
template<Arithmetic T>
Graph<T>::Graph() noexcept
  : g( std::make_unique<Vertices<T>>() )
{
}

template<Arithmetic T>
Graph<T>::Graph(std::initializer_list<std::pair<T,T>> t) noexcept
  : Graph()
{
  rg::for_each(t, [&](auto&& v){this->emplace(v);});
}
/// }}}

// Public Methods {{{

// Element Access {{{
template<Arithmetic T>
template<typename U> requires ConvertibleTo<T,U>
std::vector<T> Graph<T>::successors(U u) const
{
  return fp::get_map_values(fp::map_keep_if([&u](auto k){ return k == u; },*g));
}

template<Arithmetic T>
template<typename U> requires ConvertibleTo<T,U>
std::vector<T> Graph<T>::predecessors(U u) const
{
  return fp::get_map_keys(fp::map_keep_values(std::vector<T>{static_cast<T>(u)},*g));
}

template<Arithmetic T>
template<typename U> requires ConvertibleTo<T,U>
std::vector<T> Graph<T>::neighbors(U u) const
{
  return fp::append(this->successors(u),this->predecessors(u));
}

template<Arithmetic T>
template<typename U> requires ConvertibleTo<T,U>
bool Graph<T>::adjacent(U u1, U u2) const
{
  auto rng{g->equal_range(u1)};

  for (auto it{rng.first}; it != rng.second; ++it)
  {
    if( it->second ==  u2 ){ return true; }
  } // for

  return false;
}

template<Arithmetic T>
Vertices<T>& Graph<T>::data() noexcept
{
  return *(this->g);
}

template<Arithmetic T>
Vertices<T> const& Graph<T>::data() const noexcept
{
  return *(this->g);
}
// }}}

// Capacity {{{

template<Arithmetic T>
std::size_t Graph<T>::vertices_count() const
{
  return
    fw::apply(fp::get_map_keys(*g)
      , fw::append(fp::get_map_values(*g))
      , fw::sort()
      , fw::unique()
      , fw::size_of_cont()
    );
}

template<Arithmetic T>
std::size_t Graph<T>::edges_count() const
{
  return g->size();
}

// }}}

// Modifiers {{{

template<Arithmetic T>
template<typename... U> requires IsPairsOf<T,U...>
void Graph<T>::emplace(U&&... u)
{
  (this->g->emplace(std::forward<U>(u)),...);
}

template<Arithmetic T>
template<typename U> requires IsPairOf<T,U>
void Graph<T>::erase(U&& u)
{
  auto rng{this->g->equal_range(u.first)};

  for (auto it{rng.first}; it != rng.second; ++it)
  {
    if( u.second == it->second )
    {
      this->g->erase(it);
      break;
    }
  } // for: it != rng.second
} // function: erase

// }}}

// Operators {{{

template<Arithmetic T>
Graph<T> Graph<T>::operator=(Graph<T> const& rhs)
{
  this->g = std::make_unique<Vertices<T>>(*rhs.g);
  return *this;
} // function: operator=

template<Arithmetic T>
Graph<T> Graph<T>::operator=(Graph<T>&& rhs)
{
  this->g = std::exchange(rhs.g, std::make_unique<Vertices<T>>());
  return *this;
} // function: operator=

// }}}

// }}}

} // namespace celaeno::graph }}}
