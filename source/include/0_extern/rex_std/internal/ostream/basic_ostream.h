// ============================================
//
// REX - STANDARD LIBRARY IMPLEMENTATION
//
// Author: Nick De Breuck
// Twitter: @nick_debreuck
//
// File: basic_ostream.h
// Copyright (c) Nick De Breuck 2022
//
// ============================================

//-----------------------------------------------------------------------------
// https://en.cppreference.com/w/cpp/io/basic_ostream
//
// Provides support for high level output operations on character streams.
// The supported operations include formatted output (eg. integer values) and
// unformatted output (eg. raw characters and character arrays).
// This functionality is implemented in terms of the interface provided by the underlying basic_streambuf class,
// accessed through the basic_ios base class.
//-----------------------------------------------------------------------------

#pragma once

#include "rex_std/bonus/string/stack_string.h"
#include "rex_std/bonus/string/string_utils.h"
#include "rex_std/internal/ios/basic_ios.h"
#include "rex_std/internal/stddef/nullptr.h"
#include "rex_std/internal/type_traits/conjunction.h"
#include "rex_std/internal/type_traits/is_convertible.h"
#include "rex_std/internal/type_traits/is_same.h"
#include "rex_std/internal/type_traits/void.h"
#include "rex_std/internal/utility/declval.h"

namespace rsl
{
  inline namespace v1
  {

    template <typename CharT, typename Traits = char_traits<CharT>>
    class basic_ostream : virtual public basic_ios<CharT, Traits>
    {
    private:
      using base = basic_ios<CharT, Traits>;

    public:
      using char_type   = CharT;
      using traits_type = Traits;
      using int_type    = typename Traits::int_type;
      using pos_type    = typename Traits::pos_type;
      using off_type    = typename Traits::off_type;

      static_assert(rsl::is_same_v<typename Traits::char_type, CharT>, "char type of Traits has to be the same type as the stream's underlying type");

      // a sentry object is used to prepare the basic_ostream for output
      class sentry
      {
      public:
        using traits_type = Traits;

        // prepares the stream for formatted output
        explicit sentry(basic_ostream<CharT, Traits>& outStream) // NOLINT(misc-no-recursion)
            : m_ostream(&outStream)
            , m_is_ok(false)
        {
          if(outStream.good() == false)
          {
            return;
          }

          basic_ostream<CharT, Traits>* tied_str = m_ostream->tie();
          if(tied_str)
          {
            REX_ASSERT_X(tied_str->tie() != addressof(m_ostream), "circular tied ostreams, this is not allowed!");
            tied_str->flush();
          }

          m_is_ok = outStream.good();
        }

        sentry(const sentry&) = delete;
        sentry(sentry&&)      = delete;

        // finalizes the stream object after formatted output
        ~sentry()
        {
          /// [09/Sep/2022] RSL Comment: Because ios_base doesn't have any flags, this always flushes.
          if(m_ostream->good())
          {
            // flush the stream
            if(m_ostream->rdbuf()->pubsync() == -1)
            {
              m_ostream->setstate(io::iostate::badbit);
            }
          }
        }

        // the copy assignment is deleted
        const sentry& operator=(const sentry&) = delete;
        const sentry& operator=(sentry&&)      = delete;

        // checks whether the preparation of the output stream was successful.
        explicit operator bool() const
        {
          return m_is_ok;
        }

      private:
        basic_ostream<CharT, Traits>* m_ostream;
        bool m_is_ok;
      };

      // constructs the basic_ostream object, assigning initial values to the base class
      // by calling basic_ios::init(sb)
      explicit basic_ostream(basic_streambuf<CharT, Traits>* sb)
          : base(sb)
      {
      }

      // destroys the basic_ostream object.
      ~basic_ostream() override = default;

      // you can't copy an output stream
      basic_ostream(const basic_ostream& other) = delete;

      // you can't copy assign an output stream
      basic_ostream& operator=(const basic_ostream&) = delete;

      // writes an integer value to the stream
      basic_ostream& operator<<(int16 value)
      {
        const sentry sentry(*this);

        if(sentry)
        {
          tiny_stack_string str = internal::to_string<tiny_stack_string>(value);
          base::rdbuf()->sputn(str.data(), str.length());
        }

        return *this;
      }
      // writes an integer value to the stream
      basic_ostream& operator<<(uint16 value)
      {
        const sentry sentry(*this);

        if(sentry)
        {
          tiny_stack_string str = internal::to_string<tiny_stack_string>(value);
          base::rdbuf()->sputn(str.data(), str.length());
        }

        return *this;
      }
      // writes an integer value to the stream
      basic_ostream& operator<<(int32 value)
      {
        const sentry sentry(*this);

        if(sentry)
        {
          tiny_stack_string str = internal::to_string<tiny_stack_string>(value);
          base::rdbuf()->sputn(str.data(), str.length());
        }

        return *this;
      }
      // writes an integer value to the stream
      basic_ostream& operator<<(uint32 value)
      {
        const sentry sentry(*this);

        if(sentry)
        {
          tiny_stack_string str = internal::to_string<tiny_stack_string>(value);
          base::rdbuf()->sputn(str.data(), str.length());
        }

        return *this;
      }
      // writes an integer value to the stream
      basic_ostream& operator<<(int64 value)
      {
        const sentry sentry(*this);

        if(sentry)
        {
          tiny_stack_string str = internal::to_string<tiny_stack_string>(value);
          base::rdbuf()->sputn(str.data(), str.length());
        }

        return *this;
      }
      // writes an integer value to the stream
      basic_ostream& operator<<(uint64 value)
      {
        const sentry sentry(*this);

        if(sentry)
        {
          tiny_stack_string str = internal::to_string<tiny_stack_string>(value);
          base::rdbuf()->sputn(str.data(), str.length());
        }

        return *this;
      }
      // writes an integer value to the stream
      basic_ostream& operator<<(long value)
      {
        const sentry sentry(*this);

        if(sentry)
        {
          tiny_stack_string str = internal::to_string<tiny_stack_string>(value);
          base::rdbuf()->sputn(str.data(), str.length());
        }

        return *this;
      }
      // writes an integer value to the stream
      basic_ostream& operator<<(ulong value)
      {
        const sentry sentry(*this);

        if(sentry)
        {
          tiny_stack_string str = internal::to_string<tiny_stack_string>(value);
          base::rdbuf()->sputn(str.data(), str.length());
        }

        return *this;
      }

      // writes a float value to the stream
      basic_ostream& operator<<(float32 value)
      {
        const sentry sentry(*this);

        if(sentry)
        {
          tiny_stack_string str = internal::to_string<tiny_stack_string>(value);
          base::rdbuf()->sputn(str.data(), str.length());
        }

        return *this;
      }
      // writes a float value to the stream
      basic_ostream& operator<<(float64 value)
      {
        const sentry sentry(*this);

        if(sentry)
        {
          tiny_stack_string str = internal::to_string<tiny_stack_string>(value);
          base::rdbuf()->sputn(str.data(), str.length());
        }

        return *this;
      }
      // writes a float value to the stream
      basic_ostream& operator<<(lfloat64 value)
      {
        const sentry sentry(*this);

        if(sentry)
        {
          tiny_stack_string str = internal::to_string<tiny_stack_string>(value);
          base::rdbuf()->sputn(str.data(), str.length());
        }

        return *this;
      }

      // writes a bool to the stream
      basic_ostream& operator<<(bool value)
      {
        const sentry sentry(*this);

        if(sentry)
        {
          tiny_stack_string str = internal::to_string<tiny_stack_string>(value);
          base::rdbuf()->sputn(str.data(), str.length());
        }

        return *this;
      }

      // writes a pointer to the stream
      basic_ostream& operator<<(const void* value)
      {
        const sentry sentry(*this);

        if(sentry)
        {
          tiny_stack_string str = internal::to_string<tiny_stack_string>(value);
          base::rdbuf()->sputn(str.data(), str.length());
        }

        return *this;
      }

      // writes "nullptr" to the stream
      basic_ostream& operator<<(nullptr_t)
      {
        return *this << "nullptr";
      }

      // insert characters from the input sequence and inserts them into this.
      basic_ostream& operator<<(basic_streambuf<CharT, Traits>* sb)
      {
        const sentry sentry(*this);

        if(sentry)
        {
          char_type ch = sb->sgetc();
          while(ch != traits_type::eof())
          {
            base::rdbuf()->sputc(ch);
          }
        }

        return *this;
      }

      // calls func(this). This function is implemented to enable I/O manipulators
      basic_ostream& operator<<(ios_base& (*func)(ios_base&))
      {
        return func(*this);
      }
      // calls func(this). This function is implemented to enable I/O manipulators
      basic_ostream& operator<<(basic_ios<CharT, Traits>& (*func)(basic_ios<CharT, Traits>&))
      {
        return func(*this);
      }
      // calls func(this). This function is implemented to enable I/O manipulators
      basic_ostream& operator<<(basic_ostream<CharT, Traits>& (*func)(basic_ios<CharT, Traits>&))
      {
        return func(*this);
      }

      // inserts a character
      basic_ostream& put(char_type ch)
      {
        const sentry sentry(*this);

        if(sentry)
        {
          base::rdbuf()->sputc(ch);
        }

        return *this;
      }

      // inserts blocks of characters
      basic_ostream& write(const char_type* s, streamsize count)
      {
        const sentry sentry(*this);

        if(sentry)
        {
          base::rdbuf()->sputn(s, count);
        }

        return *this;
      }

      // returns the output position indicator
      pos_type tellp()
      {
        const sentry sentry(*this);

        if(sentry)
        {
          return base::rdbuf()->pubseekoff(0, io::seekdir::cur, io::openmode::out);
        }
        else
        {
          return pos_type(-1);
        }
      }
      // sets the output position indicator
      basic_ostream& seekp(pos_type pos)
      {
        const sentry sentry(*this);

        if(sentry)
        {
          base::rdbuf()->pubseekpos(pos, io::openmode::out);
        }
        else
        {
          base::setstate(io::iostate::failbit);
        }

        return *this;
      }
      // sets the output position indicator
      basic_ostream& seekp(off_type off, io::seekdir dir)
      {
        const sentry sentry(*this);

        if(sentry)
        {
          base::rdbuf()->pubseekpos(off, dir, io::openmode::out);
        }
        else
        {
          base::setstate(io::iostate::failbit);
        }

        return *this;
      }

      // synchronizes with the underlying storage device
      basic_ostream& flush() // NOLINT(misc-no-recursion)
      {
        const sentry sentry(*this);

        if(sentry)
        {
          basic_streambuf<CharT, Traits>* streambuf = base::rdbuf();
          if(streambuf)
          {
            const int32 result = streambuf->pubsync();

            if(result == -1)
            {
              base::setstate(io::iostate::badbit);
            }
          }
        }

        return *this;
      }
      // swaps stream objects, except for the associated buffer
      void swap(basic_ostream& other)
      {
        base::swap(other);
      }

    protected:
      // Uses basic_ios<CharT, Traits>::move(other) to move all basic_ios members,
      // except for the rdbuf(), from other to this
      basic_ostream(basic_ostream&& other)
          : base()
      {
        base::move(rsl::move(other));
      }

      // exchanges all data members of the base class, except for rdbuf(), with other.
      basic_ostream& operator=(basic_ostream&& other)
      {
        swap(other);
        return *this;
      }
    };

    /// RSL Comment: Different from ISO C++ Standard at time of writing (07/Sep/2022)
    // to avoid the complexity of the stl, you can only output to a basic_ostream
    // you can only output the character type that is the same as the basic_ostream's char_type

    /// RSL Comment: Different from ISO C++ Standard at time of writing (08/Sep/2022)
    // this function is the same as basic_ostream<CharT, Traits>::put.
    // Rex Standard Library doesn't care about locale nor formatting for output
    // which is what makes this function different from put in the C++ standard.
    // writes a character to the stream
    template <typename CharT, typename Traits>
    basic_ostream<CharT, Traits>& operator<<(basic_ostream<CharT, Traits>& os, CharT ch)
    {
      return os.put(ch);
    }

    /// RSL Comment: Different from ISO C++ Standard at time of writing (08/Sep/2022)
    // this function is the same as basic_ostream<CharT, Traits>::put.
    // Rex Standard Library doesn't care about locale nor formatting for output
    // which is what makes this function different from put in the C++ standard.
    // writes a character to the stream
    template <typename CharT, typename Traits>
    basic_ostream<CharT, Traits>& operator<<(basic_ostream<CharT, Traits>& os, int8 ch)
    {
      typename basic_ostream<CharT, Traits>::char_type wide_ch = os.widen(ch);
      os.put(wide_ch);
      return os;
    }

    /// RSL Comment: Different from ISO C++ Standard at time of writing (08/Sep/2022)
    // this function is the same as basic_ostream<CharT, Traits>::put.
    // Rex Standard Library doesn't care about locale nor formatting for output
    // which is what makes this function different from put in the C++ standard.
    // writes a character to the stream
    template <typename CharT, typename Traits>
    basic_ostream<CharT, Traits>& operator<<(basic_ostream<CharT, Traits>& os, uint8 ch)
    {
      typename basic_ostream<CharT, Traits>::char_type wide_ch = os.widen(ch);
      os.put(wide_ch);
      return os;
    }

    /// RSL Comment: Different from ISO C++ Standard at time of writing (07/Sep/2022)
    // Rex Standard Library only supports a string literal overload of operator<<.
    // outputting a CharT pointer is not supported, for performance reasons.
    // writes a string literal to the stream
    template <typename CharT, typename Traits, count_t N>
    basic_ostream<CharT, Traits>& operator<<(basic_ostream<CharT, Traits>& os, const CharT (&s)[N]) // NOLINT(modernize-avoid-c-arrays)
    {
      os.write(s, N);
      return os;
    }

    namespace internal
    {
      template <typename Ostream, typename T, typename = void>
      struct can_stream_out : false_type
      {
      };

      template <typename Ostream, typename T>
      struct can_stream_out<Ostream, T, void_t<decltype(rsl::declval<Ostream&>() << rsl::declval<const T&>())>> : true_type
      {
      };
    } // namespace internal

    // calls the appropriate insertion operator, given an rvalue reference to an output stream object
    template <typename Ostream, typename T, enable_if_t<conjunction_v<is_convertible<Ostream*, ios_base*>, internal::can_stream_out<Ostream, T>>, bool> = true>
    Ostream&& operator<<(Ostream&& os, const T& val)
    {
      os << val;
      return rsl::move(os);
    }

  } // namespace v1
} // namespace rsl
