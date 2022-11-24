// ============================================
//
// REX - STANDARD LIBRARY IMPLEMENTATION
//
// Author: Nick De Breuck
// Twitter: @nick_debreuck
//
// File: ostream.h
// Copyright (c) Nick De Breuck 2022
//
// ============================================

#pragma once

#include "rex_std/internal/ostream/basic_ostream.h"

namespace rsl
{
  inline namespace v1
  {

    using ostream  = basic_ostream<char8>;
    using wostream = basic_ostream<tchar>;

    // template <typename ... Args>
    // void print(ostream& os, fmt::format_string<Args...> fmt, Args&& ... args);

    // template <typename ... Args>
    // void println(ostream& os, fmt::format_string<Args...> fmt, Args&& ... args);

    // template <typename CharT, typename Traits>
    // basic_ostream<CharT, Traits>& endl(basic_ostream<CharT, Traits>& os);

    // template <typename CharT, typename Traits>
    // basic_ostream<CharT, Traits>& ends(basic_ostream<CharT, Traits>& os);

    // template <typename CharT, typename Traits>
    // basic_ostream<CharT, Traits>& flush(basic_ostream<CharT, Traits>& os);

    // template <typename CharT, typename Traits>
    // basic_ostream<CharT, Traits>& emit_on_flush(basic_ostream<CharT, Traits>& os);

    // template <typename CharT, typename Traits>
    // basic_ostream<CharT, Traits>& noemit_on_flush(basic_ostream<CharT, Traits>& os);

    // template <typename CharT, typename Traits>
    // basic_ostream<CharT, Traits>& flush_emit(basic_ostream<CharT, Traits>& os);

  } // namespace v1
} // namespace rsl

#include "rex_std/enable_std_checking.h"