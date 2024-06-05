//
// @author      : Ruan E. Formigoni (ruanformigoni@gmail.com)
// @file        : literals
//

#pragma once

#include <iostream>
#include <format>
#include <sstream>

namespace
{

// concepts {{{
template<typename T>
concept StringConvertible = std::is_convertible_v<std::decay_t<T>, std::string>;

template<typename T>
concept StringConstructible = std::constructible_from<std::string, std::decay_t<T>>;

template<typename T>
concept Numeric = std::integral<std::remove_cvref_t<T>> or std::floating_point<std::remove_cvref_t<T>>;

template<typename T>
concept StreamInsertable = requires(T t, std::ostream& os)
{
  { os << t } -> std::same_as<std::ostream&>;
};

template<typename T>
concept IterableConst = requires(T t)
{
  { t.cbegin() } -> std::input_iterator;
  { t.cend() } -> std::input_iterator;
};

template<typename T>
concept StringRepresentable = StringConvertible<T> or StringConstructible<T> or Numeric<T> or StreamInsertable<T>;
// }}}

// to_string() {{{
template<typename T>
inline std::string to_string(T&& t)
{
  if constexpr ( StringConvertible<T> )
  {
    return t;
  } // if
  else if constexpr ( StringConstructible<T> )
  {
    return std::string{t};
  } // else if
  else if constexpr ( Numeric<T> )
  {
    return std::to_string(t);
  } // else if
  else if constexpr ( StreamInsertable<T> )
  {
    std::stringstream ss;
    ss << t;
    return ss.str();
  } // else if
  else if constexpr ( IterableConst<T> )
  {
    std::stringstream ss;
    ss << '[';
    std::for_each(t.cbegin(), t.cend(), [&](auto&& e){ ss << std::format("'{}',", e); });
    ss << ']';
    return ss.str();
  } // else if

  throw std::runtime_error(std::string{"Cannot convert valid string, type: "} + typeid(T).name());
} // to_string() }}}

// struct format_args {{{
template<typename... Args>
struct format_args
{
  // Create a tuple where each type is the result of ns_string::to_string(Args...)
  std::tuple<decltype(to_string(std::declval<Args>()))...> m_tuple_args;

  // Initializes the tuple elements to be string representations of the arguments
  format_args(Args&&... args)
    : m_tuple_args(to_string(std::forward<Args>(args))...)
  {} // format_args

  // Get underlying arguments
  auto operator*()
  {
    return std::apply([](auto&&... e) { return std::make_format_args(e...); }, m_tuple_args);
  } // operator*
}; // struct format_args }}}

} // namespace

// Print to stdout
inline auto operator""_print(const char* c_str, std::size_t)
{
  return [=]<typename... Args>(Args&&... args)
  {
    std::cout << std::vformat(c_str, *format_args<Args...>(std::forward<Args>(args)...));
  };
}

// Format strings with user-defined literals
inline decltype(auto) operator ""_fmt(const char* str, size_t)
{
  return [str]<typename... Args>(Args&&... args)
  {
    return std::vformat(str, *format_args<Args...>(std::forward<Args>(args)...)) ;
  };
}

// Throw with message
inline decltype(auto) operator ""_throw(const char* str, size_t)
{
  return [str]<typename... Args>(Args&&... args)
  {
    throw std::runtime_error(std::vformat(str, *format_args<Args...>(std::forward<Args>(args)...)));
  };
}

/* vim: set expandtab fdm=marker ts=2 sw=2 tw=100 et :*/
