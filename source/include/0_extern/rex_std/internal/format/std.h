// Formatting library for C++ - formatters for standard library types
//
// Copyright (c) 2012 - present, Victor Zverovich
// All rights reserved.
//
// For the license information refer to format.h.

#ifndef FMT_STD_H_
#define FMT_STD_H_

#include "ostream.h"

#include <thread>
#include <type_traits>
#include <utility>

#if FMT_HAS_INCLUDE(<version>)
  #include <version>
#endif
// Checking FMT_CPLUSPLUS for warning suppression in MSVC.
#if FMT_CPLUSPLUS >= 201703L
  #if FMT_HAS_INCLUDE(<filesystem>)
    #include <filesystem>
  #endif
  #if FMT_HAS_INCLUDE(<variant>)
    #include <variant>
  #endif
#endif

#ifdef __cpp_lib_filesystem
FMT_BEGIN_NAMESPACE

namespace detail
{

  template <typename Char>
  void write_escaped_path(basic_memory_buffer<Char>& quoted, const rsl::filesystem::path& p)
  {
    write_escaped_string<Char>(rsl::back_inserter(quoted), p.string<Char>());
  }
  #ifdef _WIN32
  template <>
  inline void write_escaped_path<char>(basic_memory_buffer<char>& quoted, const rsl::filesystem::path& p)
  {
    auto s = p.u8string();
    write_escaped_string<char>(rsl::back_inserter(quoted), string_view(reinterpret_cast<const char*>(s.c_str()), s.size()));
  }
  #endif
  template <>
  inline void write_escaped_path<rsl::filesystem::path::value_type>(basic_memory_buffer<rsl::filesystem::path::value_type>& quoted, const rsl::filesystem::path& p)
  {
    write_escaped_string<rsl::filesystem::path::value_type>(rsl::back_inserter(quoted), p.native());
  }

} // namespace detail

template <typename Char>
struct formatter<rsl::filesystem::path, Char> : formatter<basic_string_view<Char>>
{
  template <typename FormatContext>
  auto format(const rsl::filesystem::path& p, FormatContext& ctx) const -> typename FormatContext::iterator
  {
    basic_memory_buffer<Char> quoted;
    detail::write_escaped_path(quoted, p);
    return formatter<basic_string_view<Char>>::format(basic_string_view<Char>(quoted.data(), quoted.size()), ctx);
  }
};
FMT_END_NAMESPACE
#endif

FMT_BEGIN_NAMESPACE
template <typename Char>
struct formatter<rsl::thread::id, Char> : basic_ostream_formatter<Char>
{
};
FMT_END_NAMESPACE

#ifdef __cpp_lib_variant
FMT_BEGIN_NAMESPACE
template <typename Char>
struct formatter<rsl::monostate, Char>
{
  template <typename ParseContext>
  FMT_CONSTEXPR auto parse(ParseContext& ctx) -> decltype(ctx.begin())
  {
    return ctx.begin();
  }

  template <typename FormatContext>
  auto format(const rsl::monostate&, FormatContext& ctx) const -> decltype(ctx.out())
  {
    auto out = ctx.out();
    out      = detail::write<Char>(out, "monostate");
    return out;
  }
};

namespace detail
{

  template <typename T>
  using variant_index_sequence = rsl::make_index_sequence<rsl::variant_size<T>::value>;

  // variant_size and variant_alternative check.
  template <typename T, typename U = void>
  struct is_variant_like_ : rsl::false_type
  {
  };
  template <typename T>
  struct is_variant_like_<T, rsl::void_t<decltype(rsl::variant_size<T>::value)>> : rsl::true_type
  {
  };

  // formattable element check
  template <typename T, typename C>
  class is_variant_formattable_
  {
    template <rsl::size_t... I>
    static rsl::conjunction<is_formattable<rsl::variant_alternative_t<I, T>, C>...> check(rsl::index_sequence<I...>);

  public:
    static constexpr const bool value = decltype(check(variant_index_sequence<T> {}))::value;
  };

  template <typename Char, typename OutputIt, typename T>
  auto write_variant_alternative(OutputIt out, const T& v) -> OutputIt
  {
    if constexpr(is_string<T>::value)
      return write_escaped_string<Char>(out, detail::to_string_view(v));
    else if constexpr(rsl::is_same_v<T, Char>)
      return write_escaped_char(out, v);
    else
      return write<Char>(out, v);
  }

} // namespace detail

template <typename T>
struct is_variant_like
{
  static constexpr const bool value = detail::is_variant_like_<T>::value;
};

template <typename T, typename C>
struct is_variant_formattable
{
  static constexpr const bool value = detail::is_variant_formattable_<T, C>::value;
};

template <typename Variant, typename Char>
struct formatter<Variant, Char, rsl::enable_if_t<rsl::conjunction_v<is_variant_like<Variant>, is_variant_formattable<Variant, Char>>>>
{
  template <typename ParseContext>
  FMT_CONSTEXPR auto parse(ParseContext& ctx) -> decltype(ctx.begin())
  {
    return ctx.begin();
  }

  template <typename FormatContext>
  auto format(const Variant& value, FormatContext& ctx) const -> decltype(ctx.out())
  {
    auto out = ctx.out();

    out = detail::write<Char>(out, "variant(");
    rsl::visit([&](const auto& v) { out = detail::write_variant_alternative<Char>(out, v); }, value);
    *out++ = ')';
    return out;
  }
};
FMT_END_NAMESPACE
#endif

#endif // FMT_STD_H_
