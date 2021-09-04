// vim: set expandtab fdm=marker ts=2 sw=2 tw=80 et :
//
// @author      : Ruan E. Formigoni (ruanformigoni@gmail.com)
// @file        : fun-1
// @created     : Monday Aug 16, 2021 13:10:35 UTC
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

#include <celaeno/aliases.hpp>
#include <celaeno/concepts.hpp>
#include <fmt/ranges.h>
#include <range/v3/all.hpp>

// namespace celaeno::fun {{{
namespace celaeno::fun
{

// using namespaces {{{
using namespace celaeno::aliases;
using namespace celaeno::concepts;
// }}}

// namespaces {{{
namespace rg = ranges;
namespace ra = ranges::actions;
namespace rv = ranges::views;
// }}}

// Class: Fun {{{
template<typename V>
struct Fun
{
  V view;

  Fun(V v) : view(v) {}

  // Terminators
  template<Range R, typename F = std::equal_to<> >
  decltype(auto) find_first_of(R&& r, F&& f = {})
  {
    return rg::find_first_of(view,r,f);
  } // function: first_of

  template<typename... T>
  [[nodiscard]] bool has(T&&... t)
  {
    return (rg::contains(view,t) || ...);
  }

  [[nodiscard]] auto sum()
  {
    return rg::accumulate(view,0);
  }

  template<typename T>
  [[nodiscard]] size_t count(T&& t)
  {
    return rg::count(view,t);
  }

  template<typename F>
  [[nodiscard]] size_t count_if(F&& f)
  {
    return rg::count_if(view,f);
  }

  template<typename F = std::less<>, typename P = ranges::identity>
  [[nodiscard]] auto max(F&& f = {}, P&& p = {})
  {
    return *rg::max_element(view,f,p);
  }

  template<typename F>
  [[nodiscard]] auto all(F&& f)
  {
    return rg::all_of(view,f);
  }

  template<typename F>
  [[nodiscard]] auto group(F&& f)
  {
    auto it{rg::partition(view,f)};
    return std::tuple(view.begin(),it,view.end());
  }

  [[nodiscard]] auto vec()
  {
    return view | rg::to<std::vector>;
  }

  // Print
  template<typename T>
  [[nodiscard]] auto print()
  {
    fmt::print("-- {}\n", this->vec());
    return *this;
  }

  // Modifying
  template<typename F = std::less<>, typename P = ranges::identity>
  [[nodiscard]] auto sort(F&& f = {}, P&& p = {})
  {
    rg::sort(view,f,p);
    return *this;
  }

  [[nodiscard]] auto rot(int n)
  {
    rg::rotate(view,std::next(view.begin(),n));

    return *this;
  }

  template<typename It>
  [[nodiscard]] auto rot(It it)
  {
    rg::rotate(view,it);

    return *this;
  }

  template<typename F>
  decltype(auto) ply(F&& f)
  {
    rg::for_each(view,f);
    return *this;
  } // function: ply

  // Views {{{

  // fn: make_view {{{
  template<typename Out = void, typename E>
  [[nodiscard]] auto make_view(E e)
  {
    using Type = decltype(e);

    if constexpr ( SameAs<Out,void> )
    {
      return Fun<Type>(e);
    } // if
    else
    {
      return e | rg::to<Out>;
    } // else
  } // fn: make_view }}}

  // fn: as {{{
  //
  // Transforms the elements of a type from another, given a transformation
  // unary predicate f
  //
  // @param f: Transformation unary predicate
  //
  // Synonyms: transform, apply
  //
  template<typename Out = void, typename F>
  [[nodiscard]] auto as(F&& f)
  {
    return make_view<Out>(view | rv::transform(f));
  } // fn: as }}}

  // fn: chain {{{
  //
  // Adds all elements of range r to the end of the current
  // e.g.: [1,2,3].chain([4,5]) == [1,2,3,4,5]
  //
  // @param r: A range to chain current one with
  //
  // Synonyms: Concat, Append
  //
  template<typename Out = void, Range R>
  [[nodiscard]] decltype(auto) chain(R&& r)
  {
    return make_view<Out>(rv::concat(view,r));
  } // fn: chain }}}

  // fn: cut {{{
  //
  // Patitions the sequence, given initial and final positions
  //
  // @param from: Start of the cut
  // @param to: End of the cut
  //
  // Synonyms: partition, slice
  //
  template<typename Out = void, typename... Args>
  [[nodiscard]] decltype(auto) cut(Args&&... args)
  {
    return make_view<Out>(view | rv::slice(std::forward<Args>(args)...));
  } // fn: cut }}}

  // fn: dif {{{
  //
  // Keeps all elements that are in current view and NOT in r
  // e.g.: [1,2,3].dif([0,1,2,3,4]) == [0,4]
  //
  // @param r: Input range to compare elements to
  //
  // Synonyms: difference
  //
  template<typename Out = void, Range R>
  [[nodiscard]] auto dif(R&& r)
  {
    if constexpr ( Map<R> or Set<R> )
    {
      return this->keep<Out>([&](auto&& u) { return ! r.contains(u); });
    } // if
    else
    {
      return this->keep<Out>([&](auto&& u) { return ! rg::contains(r,u); });
    } // else

  } // fn: dif }}}

  // fn: drop {{{
  //
  // Drop all elements of current view that satisfy F
  //
  // @param f: Unary predicate
  //
  // Synonyms: filter
  //
  template<typename Out = void, typename F>
  [[nodiscard]] auto drop(F&& f)
  {
    return this->keep<Out>([&](auto&& e){ return ! f(e); });
  } // fn: drop }}}

  // fn: drop_while {{{
  //
  // Drop elements of current view while a unary predicate is true
  //
  // @param f: A unary predicate
  //
  template<typename Out = void, typename F>
  [[nodiscard]] auto drop_while(F&& f)
  {
    return make_view<Out>(view | rv::drop_while(f));
  } // fn: drop_while }}}

  // fn: first {{{
  //
  // Takes only the first element of current view
  //
  template<typename Out = void>
  decltype(auto) first()
  {
    return make_view(view | rv::take(1));
  } // fn: first }}}

  // fn: in {{{
  //
  // Keeps all elements that are in current view and r
  // e.g.: [1,2,3].dif([0,1,2,3,4]) == [1,2,3]
  //
  // @param r: Input range to compare elements to
  //
  // Synonyms: keep, intersection
  //
  template<typename Out = void, Range R>
  [[nodiscard]] auto in(R&& r)
  {
    if constexpr ( Map<R> or Set<R> )
    {
      return this->keep<Out>([&](auto&& u) { return r.contains(u); });
    } // if
    else
    {
      return this->keep<Out>([&](auto&& u) { return rg::contains(r,u); });
    } // else

  } // fn: in }}}

  // fn: in_all {{{
  //
  // Keep only elements that are true for every element of r by f
  //
  // @param r: A range of which to compare every element to
  // @param f: An unary predicate to apply on each element of r
  //
  template<typename Out = void, Range R, typename F>
  [[nodiscard]] auto in_all(R&& r, F&& f)
  {
    return this->keep<Out>([&](auto&& a)
    {
      return rg::all_of(r,[&](auto&& b){ return f(a,b); } );
    });
  } // fn: in_all }}}

  // fn: keep {{{
  //
  // Keep all elements of current view that satisfy F
  //
  // @param f: Unary predicate
  //
  // Synonyms: filter
  //
  template<typename Out = void, typename F>
  [[nodiscard]] auto keep(F&& f)
  {
    return make_view<Out>(view | rv::filter(f));
  } // fn: keep }}}

  // fn: key {{{
  //
  // Returns all keys of a map type sequence
  //
  template<typename Out = void>
  [[nodiscard]] decltype(auto) key()
  {
    return make_view<Out>(rv::keys(view));
  } // fn: key }}}

  // fn: rev {{{
  //
  // Reverses current sequence
  //
  template<typename Out = void>
  [[nodiscard]] decltype(auto) rev()
  {
    return make_view<Out>(view | rv::reverse);
  } // fn: rev }}}

  // fn: sliding {{{
  //
  // Places a sliding window of size n in current view
  //
  template<typename Out = void>
  [[nodiscard]] auto sliding(int n)
  {
    return make_view<Out>(view | rv::sliding(n));
  } // fn: sliding }}}

  // fn: squash {{{
  //
  // Merges a list of lists into a single list
  //
  // Synonyms: flatten, merge, join
  template<typename Out = void>
  [[nodiscard]] auto squash()
  {
    return make_view<Out>(ra::join(view));
  } // fn: squash }}}

  // fn: take_while {{{
  //
  // Keeps taking elements of current range while the unary predicate is true
  //
  // @param f: A unary predicate
  //
  template<typename Out = void, typename F>
  [[nodiscard]] auto take_while(F&& f)
  {
    return make_view<Out>(view | rv::take_while(f));
  } // fn: take_while }}}

  // fn: unique {{{
  //
  // Keeps only elements that are adjacently unique, i.e., previous and next
  // elements are not equal. To have exclusively unique elements, sort before
  // usage.
  //
  template<typename Out = void>
  [[nodiscard]] auto unique()
  {
    return make_view<Out>(rv::unique(view));
  } // fn: unique }}}

  // fn: val {{{
  //
  // Returns all values of a map type sequence
  //
  template<typename Out = void>
  [[nodiscard]] decltype(auto) val()
  {
    return make_view<Out>(rv::values(view));
  } // fn: val }}}

  // fn: enu {{{
  //
  // Enumerates a sequence
  //
  template<typename Out = void>
  [[nodiscard]] auto enu()
  {
    return make_view<Out>(view | rv::enumerate);
  } // fn: enu }}}

  // Views }}}

}; // class: fun }}}

template<typename... Args>
[[nodiscard]] decltype(auto) apply(Args... args)
{
  return [&]<typename... F>(F&&... f)
  { (std::forward<F>(f)(std::forward<Args>(args)),...); };
} // function: apply

// namespace: fn {{{
namespace fn
{

template<Range R>
[[nodiscard]] decltype(auto) fn(R&& r)
{
  return Fun<decltype(rv::all(r))>{rv::all(r)};
} // function: fn

} // namespace: fn }}}

} // namespace celaeno::fun }}}
