// ============================================
//
// REX - STANDARD LIBRARY IMPLEMENTATION
//
// Author: Nick De Breuck
// Twitter: @nick_debreuck
//
// File: basic_filebuf.h
// Copyright (c) Nick De Breuck 2022
//
// ============================================

//-----------------------------------------------------------------------------
// https://en.cppreference.com/w/cpp/io/basic_filebuf
//
// a basic_streambuf that's associated with a character sequence
// of a file.
//-----------------------------------------------------------------------------

#pragma once

#include "rex_std/internal/streambuf/basic_streambuf.h"

namespace rsl
{
  inline namespace v1
  {

    namespace internal
    {
      using handle = void*;

      class filebuf_impl
      {
      };
    } // namespace internal

    template <typename CharT, typename Traits>
        class basic_filebuf : public basic_streambuf < CharT,
        Traits >>
    {
    private:
      using base = basic_streambuf<char8, char_traits<char8>>;

    public:
      using char_type   = typename base::char_type;
      using traits_type = typename base::traits_type;
      using int_type    = typename base::int_type;
      using pos_type    = typename base::pos_type;
      using off_type    = typename base::off_type;

      basic_filebuf(internal::handle handle);

    protected:
      streamsize xsgetn(char_type* s, streamsize count) final;
      streamsize xsputn(const char_type* s, streamsize count) final;

      int_type overflow(int_type ch) final;
      streamsize overflown(const char_type* s, streamsize count) final;
      int_type underflow() final;
      int_type uflow() final;
      streamsize uflown(char_type* s, streamsize count) final;

    private:
      filebuf_impl m_impl;
    };

  } // namespace v1
} // namespace rsl