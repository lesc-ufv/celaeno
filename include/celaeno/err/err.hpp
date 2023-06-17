// vim: set expandtab fdm=marker ts=2 sw=2 tw=80 et :
//
// @author      : Ruan E. Formigoni (ruanformigoni@gmail.com)
// @file        : err
// @created     : Thursday Aug 19, 2021 12:35:25 UTC
//

#pragma once

#include <set>
#include <filesystem>

#include <spdlog/spdlog.h>
#include <spdlog/sinks/basic_file_sink.h>

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
    char const* m_str_file=__builtin_FILE();
    u32 m_str_line=__builtin_LINE();
    char const* m_str_fun=__builtin_FUNCTION();
  // }}}

  public:
  // Constructors {{{
    Location(
        char const* str_file = __builtin_FILE()
      , u32 str_line = __builtin_LINE()
      , char const* str_fun = __builtin_FUNCTION()
    )
      : m_str_file(str_file)
      , m_str_line(str_line)
      , m_str_fun(str_fun)
    {}
  // }}}

  // public member functions {{{
  auto get() const
  {
    std::string basename = std::filesystem::path(m_str_file).filename();
    return fmt::format("{}:{} `{}`: ", basename, m_str_line, m_str_fun);
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
    if( conds.contains(false) || conds.empty() )
    {
      auto loc_curr = loc.get() + m;
#ifdef DEBUG
      spdlog::error(loc_curr, std::forward<Args>(args)...);
#endif // DEBUG
      exit(1);
    } // if
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

// class: Fold {{{
class Fold
{
  private:
    std::shared_ptr<spdlog::logger> m_logger;

  public:
    Fold(std::shared_ptr<spdlog::logger> logger, std::string const& suffix)
      : m_logger(logger)
    {
#ifdef DEBUG
      m_logger->info("{{{" + suffix);
#endif
    } // Fold

    ~Fold()
    {
#ifdef DEBUG
      m_logger->info("}}}");
#endif
    } // ~Fold
}; // class: Fold }}}

// class: Logger {{{
class Logger
{
  private:
    static char const * const m_log_filename;
    std::shared_ptr<spdlog::logger> m_logger;

  public:
    Logger(std::shared_ptr<spdlog::logger> logger, Location const& loc = {})
      : m_logger(logger)
    {
#ifdef DEBUG
      m_logger->info("{{{" + loc.get()); // }}}
      m_logger->flush();
#endif
    }

    Logger(Location const& loc = {})
      : m_logger(spdlog::basic_logger_st("celaeno", m_log_filename, true))
    {
#ifdef DEBUG
      m_logger->info("{{{" + loc.get()); // }}}
      m_logger->flush();
#endif
    }

    ~Logger()
    {
      // {{{
#ifdef DEBUG
      m_logger->info("}}}");
      m_logger->flush();
#endif
    }

    std::shared_ptr<spdlog::logger> sink()
    {
      return m_logger;
    }

    decltype(auto) info([[maybe_unused]] Location const& loc = {})
    {
      return [&]<String M, Printable... Args>(M&& m, Args&&... args)
      {
#ifdef DEBUG
        auto loc_curr = loc.get() + m;
        m_logger->info(loc_curr, std::forward<Args>(args)...);
        m_logger->flush();
#endif
      }; // anonymous lambda
    } // fn: info

    decltype(auto) fold(Location const& loc = {})
    {
      return Fold{m_logger,loc.get()};
    } // fold

}; // class: Logger }}}

// class: Logger {{{

char const * const Logger::m_log_filename = "celaeno.log";

// class: Logger }}}

} // namespace celaeno::err }}}
