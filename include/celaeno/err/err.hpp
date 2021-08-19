// vim: set expandtab fdm=marker ts=2 sw=2 tw=80 et :
//
// @author      : Ruan E. Formigoni (ruanformigoni@gmail.com)
// @file        : err2
// @created     : Thursday Aug 19, 2021 12:35:25 UTC
//

#include <spdlog/spdlog.h>

#include <celaeno/aliases.hpp>
#include <celaeno/concepts.hpp>

#define DEBUG (! NDEBUG)

namespace celaeno::err
{

// using namespace {{{
using namespace celaeno::concepts;
using namespace celaeno::aliases;
// }}}

// Error Handlers {{{
template<typename... C>
[[nodiscard]] auto err(C&&... conds)
{
  return
  [passed=(conds && ...)]<String S, Printable... Args>(S&& msg, Args&&... args)
  {
#ifdef DEBUG
    if( ! passed )
    {
      spdlog::error(std::forward<S>(msg), std::forward<Args>(args)...);
      exit(1);
    } // if
  };
#endif // DEBUG
} // function: check_if }}}

} // namespace celaeno::err
