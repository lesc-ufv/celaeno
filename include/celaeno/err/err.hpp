// vim: set expandtab fdm=marker ts=2 sw=2 tw=80 et :
//
// @author      : Ruan E. Formigoni (ruanformigoni@gmail.com)
// @file        : err2
// @created     : Thursday Aug 19, 2021 12:35:25 UTC
//

#pragma once

#include <set>

#include <spdlog/spdlog.h>

#include <celaeno/aliases.hpp>
#include <celaeno/concepts.hpp>

#define DEBUG (!NDEBUG)

// namespace celaeno::err {{{
namespace celaeno::err
{

// using namespace {{{
using namespace celaeno::concepts;
using namespace celaeno::aliases;
// }}}

// class: Location {{{
class Location
{
  private:
  // Private members {{{
    char const* str_file=__builtin_FILE();
    u32 str_line=__builtin_LINE();
    char const* str_fun=__builtin_FUNCTION();
  // }}}

  public:
  // Constructors {{{
    Location(
        char const* str_file = __builtin_FILE()
      , u32 str_line = __builtin_LINE()
      , char const* str_fun = __builtin_FUNCTION()
    )
      : str_file(str_file)
      , str_line(str_line)
      , str_fun(str_fun)
    {}
  // }}}

  // public member functions {{{
  auto get() const
  {
    return fmt::format("{}:{} `{}`: ",str_file,str_line,str_fun);
  }
  // }}}

}; // class: Location }}}

// fn: err {{{

//
// Given a brace enclosed list of boolean values, aborts the program if any
// argument of C is false, with a message passed with the () operator.
//
// @param conds: Comma-separated boolean values
//
// @e.g: err({! c.empty()}) ("Error on iteration {}, empty container", i)
//
[[nodiscard]] decltype(auto) err(
    [[maybe_unused]] std::set<bool> const& conds = {}
  , [[maybe_unused]] Location const& loc = {})
{
  return [&]<String M, Printable... Args>(
      [[maybe_unused]] M&& m
    , [[maybe_unused]] Args&&... args )
  {
#ifdef DEBUG
    if( conds.contains(false) || conds.empty() )
    {
      spdlog::error(loc.get() + std::forward<M>(m), std::forward<Args>(args)...);
      exit(1);
    } // if
#endif // DEBUG
  }; // anonymous lambda
} // fn: err }}}

// fn: info {{{
[[nodiscard]] decltype(auto) info([[maybe_unused]] Location const& loc = {})
{
  return [&]<String M, Printable... Args>(
      [[maybe_unused]] M&& m
    , [[maybe_unused]] Args&&... args )
  {
#ifdef DEBUG
      spdlog::info(loc.get() + std::forward<M>(m), std::forward<Args>(args)...);
#endif // DEBUG
  }; // anonymous lambda
} // fn: info }}}

} // namespace celaeno::err }}}
