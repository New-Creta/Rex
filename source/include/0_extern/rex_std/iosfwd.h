// ============================================
//
// REX - STANDARD LIBRARY IMPLEMENTATION
//
// Author: Nick De Breuck
// Twitter: @nick_debreuck
//
// File: iosfwd.h
// Copyright (c) Nick De Breuck 2022
//
// ============================================

#pragma once

#include "rex_std/bonus/types.h"

namespace rsl
{
  inline namespace v1
  {

    // defined in rex_std/string.h
    template <typename CharT>
    class char_traits;

    template <>
    class char_traits<char8>;
    template <>
    class char_traits<tchar>;
#ifdef REX_ENABLE_WITH_CPP20
    template <>
    class char_traits<char8_t>;
#endif
    template <>
    class char_traits<char16_t>;
    template <>
    class char_traits<char32_t>;

    // defined in rex_std/memory.h
    class allocator; /// RSL Comment: Different from ISO C++ Standard at time of writing (04/Sep/2022)

    // defined in rex_std/ios.h
    template <typename CharT, typename Traits = char_traits<CharT>>
    class basic_ios;

    template <typename State>
    class fpos;

    // defined in rex_std/streambuf.h
    template <typename CharT, typename Traits>
    class basic_streambuf;

    // defined in rex_std/ostream.h
    template <typename CharT, typename Traits>
    class basic_ostream;

    // defined in rex_std/istream.h
    template <typename CharT, typename Traits = char_traits<CharT>>
    class basic_istream;

    template <typename CharT, typename Traits = char_traits<CharT>>
    class basic_iostream;

    // defined in rex_std/fstream.h
    template <typename CharT, typename Traits = char_traits<CharT>>
    class basic_filebuf;

    template <typename CharT, typename Traits = char_traits<CharT>>
    class basic_ifstream;

    template <typename CharT, typename Traits = char_traits<CharT>>
    class basic_ofstream;

    template <typename CharT, typename Traits = char_traits<CharT>>
    class basic_fstream;

    // defined in rex_std/sstream.h
    template <typename CharT, typename Traits = char_traits<CharT>, typename Allocator = allocator>
    class basic_stringbuf;

    template <typename CharT, typename Traits = char_traits<CharT>, typename Allocator = allocator>
    class basic_istringstream;

    template <typename CharT, typename Traits = char_traits<CharT>, typename Allocator = allocator>
    class basic_ostringstream;

    template <typename CharT, typename Traits = char_traits<CharT>, typename Allocator = allocator>
    class basic_stringstream;

#ifdef REX_ENABLE_WITH_CPP20
    template <typename CharT, typename Traits, typename Allocator>
    class basic_syncbuf;

    template <typename CharT, typename Traits, typename Allocator>
    class basic_osyncstream;
#endif // REX_ENABLE_WITH_CPP20

#ifdef REX_ENABLE_WITH_CPP23
    template <typename CharT, typename Traits>
    class basic_spanbuf;

    template <typename CharT, typename Traits>
    class basic_ispanstream;

    template <typename CharT, typename Traits>
    class basic_ospanstream;

    template <typename CharT, typename Traits>
    class basic_spanstream;
#endif // REX_ENABLE_WITH_CPP23

  } // namespace v1
} // namespace rsl
