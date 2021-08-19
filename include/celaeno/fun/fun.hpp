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

  // Helpers
  template<typename E>
  [[nodiscard]] auto make_view(E e)
  {
    using Type = decltype(e);
    return Fun<Type>(e);
  }

  // Terminators
  template<typename T>
  [[nodiscard]] bool has(T&& t)
  {
    return rg::contains(view,t);
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

  template<typename T>
  [[nodiscard]] auto into()
  {
    return view | rg::to<T>;
  }

  // Print
  template<typename T>
  [[nodiscard]] auto print()
  {
    fmt::print("-- {}\n", this->template into<T>());
    return *this;
  }

  // Modifying
  template<typename F = std::less<>, typename P = ranges::identity>
  [[nodiscard]] auto sort(F&& f = {}, P&& p = {})
  {
    rg::sort(view,f,p);
    return *this;
  }

  [[nodiscard]] auto rotate(int n)
  {
    rg::rotate(view,std::next(view.begin(),n));

    return *this;
  }

  template<typename It>
  [[nodiscard]] auto rotate(It it)
  {
    rg::rotate(view,it);

    return *this;
  }

  template<typename F>
  decltype(auto) every(F&& f)
  {
    rg::for_each(view,f);
    return *this;
  } // function: every

  //
  // Views
  //
  template<Range R>
  [[nodiscard]] decltype(auto) add(R&& r)
  {
    return make_view(rv::concat(view,r));
  } // function: add

  [[nodiscard]] decltype(auto) reverse()
  {
    return make_view(view | rv::reverse);
  } // function: reverse

  template<typename... Args>
  [[nodiscard]] decltype(auto) cut(Args&&... args)
  {
    return make_view(view | rv::slice(std::forward<Args>(args)...));
  } // function: cut

  template<Range R>
  [[nodiscard]] auto in(R&& r)
  {
    return this->keep([&](auto&& u) { return rg::contains(r,u); });
  }

  template<typename F>
  [[nodiscard]] auto as(F&& f)
  {
    return make_view(view | rv::transform(f));
  }

  [[nodiscard]] auto squash()
  {
    return make_view(ra::join(view));
  }

  [[nodiscard]] auto enumerate()
  {
    return make_view(view | rv::enumerate);
  }

  template<typename F>
  [[nodiscard]] auto take_while(F&& f)
  {
    return make_view(view | rv::take_while(f));
  }

  template<typename F>
  [[nodiscard]] auto drop_while(F&& f)
  {
    return make_view(view | rv::drop_while(f));
  }

  template<typename F>
  [[nodiscard]] auto keep(F&& f)
  {
    return make_view(view | rv::filter(f));
  }

  template<typename F>
  [[nodiscard]] auto drop(F&& f)
  {
    return this->keep([&](auto e){ return ! f(e); });
  }

  //
  // Keep only elements that are true for every element of r by f
  //
  template<Range R, typename F>
  [[nodiscard]] auto keep_if_all(R&& r, F&& f)
  {
    return this->keep([&](auto&& a)
    {
      return rg::all_of(r,[&](auto&& b){ return f(a,b); } );
    });
  }

  [[nodiscard]] auto unique()
  {
    return make_view(rv::unique(view));
  }
}; // class: fun }}}

template<typename... Args>
[[nodiscard]] decltype(auto) apply(Args... args)
{
  return [&]<typename... F>(F&&... f)
  { (std::forward<F>(f)(std::forward<Args>(args)),...); };
} // function: apply

template<Range R>
[[nodiscard]] decltype(auto) fn(R&& r)
{
  return Fun<decltype(rv::all(r))>{rv::all(r)};
} // function: fn

} // namespace celaeno::fun }}}
