// ============================================
//
// REX - STANDARD LIBRARY IMPLEMENTATION
//
// Author: Nick De Breuck
// Twitter: @nick_debreuck
//
// File: basic_ios.h
// Copyright (c) Nick De Breuck 2022
//
// ============================================

//-----------------------------------------------------------------------------
// https://en.cppreference.com/w/cpp/io/basic_ios
//
// This class provides facilities for interfacing with objects
// that have basic_streambuf interface
//-----------------------------------------------------------------------------

#pragma once

#include "rex_std/internal/assert/assert_fwd.h"
#include "rex_std/internal/ios/ios_base.h"
#include "rex_std/internal/memory/addressof.h"
#include "rex_std/internal/streambuf/basic_streambuf.h"
#include "rex_std/internal/type_traits/is_same.h"
#include "rex_std/internal/utility/exchange.h"
#include "rex_std/internal/utility/swap.h"
#include "rex_std/iosfwd.h"

namespace rsl
{
  inline namespace v1
  {

    template <typename CharT, typename Traits>
    class basic_ios : public ios_base
    {
    public:
      using char_type   = CharT;
      using traits_type = Traits;
      using int_type    = typename Traits::int_type;
      using pos_type    = typename Traits::pos_type;
      using off_type    = typename Traits::off_type;

      static_assert(rsl::is_same_v<typename Traits::char_type, CharT>, "char type of Traits has to be the same type as the stream's underlying type");

      // initializes the internal state by calling init(sb).
      explicit basic_ios(basic_streambuf<CharT, Traits>* sb) // NOLINT(cppcoreguidelines-pro-type-member-init)
          : basic_ios()
      {
        init(sb);
      }

      // you're not allowed to copy a I/O stream.
      basic_ios(const basic_ios&) = delete;
      // you're not allowed to move a I/O stream.
      basic_ios(basic_ios&&) = delete;

      // destroys the object
      ~basic_ios() override = default;

      // you can't assign to a basic_ios
      basic_ios& operator=(const basic_ios&) = delete;
      // you can't assign to a basic_ios
      basic_ios& operator=(basic_ios&&) = delete;

      //
      // state functions
      //

      // returns true if the most recent I/O operation on the stream completed successfully.
      bool good() const
      {
        return rdstate() == io::iostate::goodbit;
      }
      // returns true if the associated stream has reached end-of-file.
      bool eof() const
      {
        return rdstate() & io::iostate::eofbit;
      }
      // returns true if an error has occurred on the associated stream.
      bool fail() const
      {
        return rdstate() & io::iostate::failbit;
      }
      // returns true if non-recoverable error has occurred on the associated stream
      bool bad() const
      {
        return rdstate() & io::iostate::badbit;
      }
      /// RSL Comment: Not in ISO C++ Standard at time of writing (09/Sep/2022)
      // returns true if the stream is not yet initialized
      bool noinit() const
      {
        return rdstate() & io::iostate::noinitbit;
      }

      // returns true if an error has occurred on the associated stream.
      // basically, returns true if the badbit or failbit is set.
      bool operator!() const
      {
        return bad() || fail();
      }

      // returns true if the stream has no errors and is ready for I/O operations.
      explicit operator bool() const
      {
        return good();
      }

      // returns the current stream error state
      io::iostate rdstate() const
      {
        return m_iostate;
      }

      // set the stream error flags state in addition to currently set flags.
      void setstate(io::iostate state)
      {
        io::iostate current_state = rdstate();
        clear(current_state |= state);
      }

      // overwrites the stream error state with the new state
      // if there's no streambuf, the state assigned is state | badbit.
      void clear(io::iostate state = io::iostate::goodbit)
      {
        m_iostate = state;

        if(rdbuf() == nullptr)
        {
          m_iostate |= io::iostate::badbit;
        }
      }

      /// RSL Comment: Different from ISO C++ Standard at time of writing (09/Sep/2022)
      // this has no effect in Rex Standard Library
      // 1) calls every callback registered by register_callback passing erase_event as parameter
      // 2) copies all member objects from other to this except for rdstate(), the exception mas and
      // the stream buffer.
      // 3) calls every callback registered by register_callback() passing copyfmt_event as parameter.
      // 4) copies the exception mask from other to *this as if by calling exceptions(other.exceptions)
      basic_ios& copyfmt(const basic_ios& other)
      {
        // Nothing to do here
      }

      // returns the current fill character
      CharT fill() const
      {
        return m_fill_ch;
      }
      // sets the fill character to ch, returns the previous value of the fill character
      CharT fill(CharT ch)
      {
        return rsl::exchange(m_fill_ch, ch);
      }

      /// RSL Comment: Different from ISO C++ Standard at time of writing (09/Sep/2022)
      // Rex Standard Library doesn't use exceptions, so this function doesn't do anything.
      // returns the exception mask
      io::iostate exceptions() const
      {
        return io::iostate::goodbit;
      }
      /// RSL Comment: Different from ISO C++ Standard at time of writing (09/Sep/2022)
      // Rex Standard Library doesn't use exceptions, so this function doesn't do anything.
      // sets the exception mask to except.
      void exceptions(io::iostate /*unused*/)
      {
        REX_ASSERT("You can't set the exception mask of a stream");
      }

      /// RSL Comment: Different from ISO C++ Standard at time of writing (09/Sep/2022)
      // Rex Standard Library doesn't use locales, so the function imbue is not defined

      // returns the associated stream buffer
      basic_streambuf<CharT, Traits>* rdbuf() const
      {
        return m_stream_buf;
      }
      // sets the associated stream buffer to sb
      basic_streambuf<CharT, Traits>* rdbuf(basic_streambuf<CharT, Traits>* sb)
      {
        clear();
        return rsl::exchange(m_stream_buf, sb);
      }

      // returns the current tied stream
      basic_ostream<CharT, Traits>* tie() const
      {
        return m_tied_stream;
      }
      // sets the current tied stream to stream, returns the previous one.
      basic_ostream<CharT, Traits>* tie(basic_ostream<CharT, Traits>* stream)
      {
        return rsl::exchange(m_tied_stream, stream);
      }

      /// RSL Comment: Different from ISO C++ Standard at time of writing (09/Sep/2022)
      // Rex Standard Library doesn't use locale, so this is just a static cast
      // converts a current locale-specific character c to its standard equivalent.
      // the result is converted from char_type to char8 if needed.
      char8 narrow(char_type c, char8 /*dfault*/) const
      {
        return static_cast<char8>(c);
      }

      /// RSL Comment: Different from ISO C++ Standard at time of writing (09/Sep/2022)
      // Rex Standard Library doesn't use locale, so this is just a static cast
      // converts a character c to its equivalent in the current locale.
      // the result is converted from char8 to character type used within the stream if needed.
      char_type widen(char8 c) const
      {
        return static_cast<char_type>(c);
      }

    protected:
      // not initialized after default ctor call.
      // init() must be called before the first use of the object or before the destructor.
      basic_ios()
          : m_stream_buf(nullptr)
          , m_iostate(io::iostate::noinitbit)
          , m_tied_stream(nullptr)
          , m_fill_ch()
      {
      }

      // sets the associates stream buffer to sb and initializes the internal state
      // post conditions:
      // rdbuf()          |  sb
      // tie()            |  nullptr
      // rdstate()        |  goodbit if sb is not a nullptr, otherwise badbit
      // exceptions()     |  goodbit
      // flags()          |  skipws | dec
      // width()          |  0
      // precision()      |  6
      // fill()           |  widen(' ')
      // getloc()         |  a copy of the value returned by rsl::locale()
      void init(basic_streambuf<CharT, Traits>* sb)
      {
        m_stream_buf  = sb;
        m_iostate     = io::iostate::goodbit;
        m_tied_stream = nullptr;
        m_fill_ch     = CharT(' '); // NOLINT(google-readability-casting)
      }

      // replaces the current state with that of other, except for the associated stream buffer
      void move(basic_ios& other)
      {
        REX_ASSERT_X(this != rsl::addressof(other));

        m_stream_buf  = nullptr;
        m_tied_stream = nullptr;
        swap(other);
      }
      // replaces the current state with that of other, except for the associated stream buffer
      void move(basic_ios&& other)
      {
        REX_ASSERT_X(this != rsl::addressof(other));

        m_stream_buf  = nullptr;
        m_tied_stream = nullptr;
        swap(other);
      }

      // exchanges the states of this and other, except for the stream buffer objects.
      void swap(basic_ios& other)
      {
        rsl::swap(m_tied_stream, other.m_tied_stream);
        rsl::swap(m_iostate, other.m_iostate);
        rsl::swap(m_fill_ch, other.m_fill_ch);
      }

      // sets the associated stream buffer to sb without clearing the error state
      void set_rdbuf(basic_streambuf<CharT, Traits>* sb)
      {
        m_stream_buf = sb;
      }

    private:
      basic_streambuf<CharT, Traits>* m_stream_buf;
      basic_ostream<CharT, Traits>* m_tied_stream;
      io::iostate m_iostate;
      CharT m_fill_ch;
    };

  } // namespace v1
} // namespace rsl