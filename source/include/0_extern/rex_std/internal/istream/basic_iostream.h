// ============================================
//
// REX - STANDARD LIBRARY IMPLEMENTATION
//
// Author: Nick De Breuck
// Twitter: @nick_debreuck
//
// File: basic_iostream.h
// Copyright (c) Nick De Breuck 2022
//
// ============================================

//-----------------------------------------------------------------------------
// https://en.cppreference.com/w/cpp/io/basic_iostream
//
// Provides support for high level input/output operations on streams.
// The supported operations include sequential reading or writing and formatting.
// the functionality is implemented over the interface provided by the basic_streambuf class,
// accessed through the basic_ios class.
//-----------------------------------------------------------------------------

#pragma once

#include "rex_std/internal/istream/basic_istream.h"
#include "rex_std/internal/ostream/basic_ostream.h"

namespace rsl
{
  inline namespace v1
  {

    template <typename CharT, typename Traits>
    class basic_iostream : public basic_ostream<CharT, Traits>, public basic_istream<CharT, Traits>
    {
    private:
      using my_istream = basic_istream<CharT, Traits>;
      using my_ostream = basic_ostream<CharT, Traits>;
      using my_ios     = basic_ios<CharT, Traits>;

    public:
      using char_type   = CharT;
      using traits_type = Traits;
      using int_type    = typename Traits::int_type;
      using pos_type    = typename Traits::pos_type;
      using off_type    = typename Traits::off_type;

      static_assert(rsl::is_same_v<typename Traits::char_type, CharT>, "char type of Traits has to be the same type as the stream's underlying type");

      // initializes with streambuf sb.
      explicit basic_iostream(basic_streambuf<CharT, Traits>* sb)
          : my_istream(sb)
          , my_ostream()
      {
      }

      // you can't copy a basic_iostream object
      basic_iostream(const basic_iostream&) = delete;

      // destructs the input/output stream
      ~basic_iostream() override = default;

      // you can't copy assign to a basic_iostream object
      basic_iostream& operator=(const basic_iostream&) = delete;

    protected:
      // move constructs the basic_istream, the basic_ostream is skipped.
      basic_iostream(basic_iostream&& other)
          : my_istream(other.rdbuf())
          , my_ostream()
      {
        my_ios::init();
        my_ios::move(rsl::move(right));
      }

      // move assigns another stream object.
      basic_iostream& operator=(basic_iostream&& other)
      {
        swap(other);
      }

      // exchanges the state with another input/output stream object.
      void swap(basic_iostream& other)
      {
        REX_ASSERT_X(this != addressof(other), "Can't swap with yourself");
        my_ios::swap(right);
      }
    };

  } // namespace v1
} // namespace rsl