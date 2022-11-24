// ============================================
//
// REX - STANDARD LIBRARY IMPLEMENTATION
//
// Author: Nick De Breuck
// Twitter: @nick_debreuck
//
// File: basic_istream.h
// Copyright (c) Nick De Breuck 2022
//
// ============================================

//-----------------------------------------------------------------------------
// https://en.cppreference.com/w/cpp/io/basic_istream
//
// Provides support for high level input operations on character streams.
// The supported operations include formatted input (eg. integer values) and
// unformatted input (eg. raw characters and character arrays).
// This functionality is implemented in terms of the interface provided by the underlying basic_streambuf class,
// accessed through the basic_ios base class.
//-----------------------------------------------------------------------------

#pragma once

#include "rex_std/bonus/string/string_utils.h"
#include "rex_std/internal/algorithm/max.h"
#include "rex_std/internal/ios/basic_ios.h"
#include "rex_std/internal/iterator/istreambuf_iterator.h"
#include "rex_std/internal/type_traits/conjunction.h"
#include "rex_std/internal/type_traits/enable_if.h"
#include "rex_std/internal/type_traits/is_convertible.h"
#include "rex_std/internal/type_traits/is_same.h"
#include "rex_std/internal/type_traits/void.h"
#include "rex_std/internal/utility/declval.h"
#include "rex_std/internal/utility/forward.h"
#include "rex_std/internal/utility/move.h"
#include "rex_std/internal/utility/size.h"

namespace rsl
{
  inline namespace v1
  {

    template <typename CharT, typename Traits>
    class basic_istream : virtual public basic_ios<CharT, Traits>
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

      // a sentry object is used to prepare the basic_istream for input
      class sentry
      {
      public:
        using traits_type = Traits;

        /// RSL Comment: Different from ISO C++ Standard at time of writing (10/Sep/2022)
        // In the C++ standard, this sentry takes in a bool flag to check if we should skip whitespace
        // this flag is omitted in Rex Standard Library
        // prepares the stream for formatted input
        explicit sentry(basic_istream<CharT, Traits>& inStream)
            : m_istream(&inStream)
            , m_is_ok(false)
        {
          if(inStream->good() == false)
          {
            m_istream.setstate(io::iostate::failbit);
            return;
          }

          basic_ostream<CharT, Traits>* tied_str = m_istream.tie();
          if(tied_str)
          {
            tied_str->flush();
          }

          m_is_ok = m_istream.good();
        }

        sentry(const sentry&) = delete;
        sentry(sentry&&)      = delete;
        // finalizes the stream object after formatted input
        ~sentry() = default;

        // the copy assignment is deleted
        sentry& operator=(const sentry&) = delete;

        // the move assignment is deleted
        sentry& operator=(sentry&&) = delete;

        // checks whether the preparation of the input stream was successful.
        explicit operator bool() const
        {
          return m_is_ok;
        }

      private:
        basic_istream<CharT, Traits>* m_istream;
        bool m_is_ok;
      };

      // constructs the basic_ostream object, assigning initial values to the base class
      // by calling basic_ios::init(sb)
      explicit basic_istream(basic_streambuf<CharT, Traits>* sb)
          : base(sb)
          , m_gcount(0)
      {
      }

      // you can't copy an input stream
      basic_istream(const basic_istream& other) = delete;

      // destroys the basic_istream object
      ~basic_istream() override = default;

      // you can't assign to an input stream
      basic_istream& operator=(const basic_istream& other) = delete;

      // reads an integer from the stream
      basic_istream& operator>>(int16& value)
      {
        const sentry sentry(*this);

        if(sentry)
        {
          optional<int16> opt_value = rsl::internal::str_to_signed<int16>(istreambuf_iterator(*this), 10);
          if(!opt_value.has_value())
          {
            base::setstate(io::iostate::failbit);
            value = 0;
          }
          else
          {
            value = opt_value.value();
          }
        }

        return *this;
      }
      // reads an integer from the stream
      basic_istream& operator>>(uint16& value)
      {
        const sentry sentry(*this);

        if(sentry)
        {
          optional<uint16> opt_value = rsl::internal::str_to_unsigned<uint16>(istreambuf_iterator(*this), 10);
          if(!opt_value.has_value())
          {
            base::setstate(io::iostate::failbit);
            value = 0;
          }
          else
          {
            value = opt_value.value();
          }
        }

        return *this;
      }

      // reads an integer from the stream
      basic_istream& operator>>(int32& value)
      {
        const sentry sentry(*this);

        if(sentry)
        {
          optional<int32> opt_value = rsl::internal::str_to_signed<int32>(istreambuf_iterator(*this), 10);
          if(!opt_value.has_value())
          {
            base::setstate(io::iostate::failbit);
            value = 0;
          }
          else
          {
            value = opt_value.value();
          }
        }

        return *this;
      }
      // reads an integer from the stream
      basic_istream& operator>>(uint32& value)
      {
        const sentry sentry(*this);

        if(sentry)
        {
          optional<uint32> opt_value = rsl::internal::str_to_unsigned<uint32>(istreambuf_iterator(*this), 10);
          if(!opt_value.has_value())
          {
            base::setstate(io::iostate::failbit);
            value = 0;
          }
          else
          {
            value = opt_value.value();
          }
        }

        return *this;
      }

      // reads an integer from the stream
      basic_istream& operator>>(int64& value)
      {
        const sentry sentry(*this);

        if(sentry)
        {
          optional<int64> opt_value = rsl::internal::str_to_signed<int64>(istreambuf_iterator(*this), 10);
          if(!opt_value.has_value())
          {
            base::setstate(io::iostate::failbit);
            value = 0;
          }
          else
          {
            value = opt_value.value();
          }
        }

        return *this;
      }
      // reads an integer from the stream
      basic_istream& operator>>(uint64& value)
      {
        const sentry sentry(*this);

        if(sentry)
        {
          optional<uint64> opt_value = rsl::internal::str_to_unsigned<uint64>(istreambuf_iterator(*this), 10);
          if(!opt_value.has_value())
          {
            base::setstate(io::iostate::failbit);
            value = 0;
          }
          else
          {
            value = opt_value.value();
          }
        }

        return *this;
      }

      // reads an integer from the stream
      basic_istream& operator>>(long& value)
      {
        const sentry sentry(*this);

        if(sentry)
        {
          optional<long> opt_value = rsl::internal::str_to_signed<long>(istreambuf_iterator(*this), 10);
          if(!opt_value.has_value())
          {
            base::setstate(io::iostate::failbit);
            value = 0;
          }
          else
          {
            value = opt_value.value();
          }
        }

        return *this;
      }
      // reads an integer from the stream
      basic_istream& operator>>(ulong& value)
      {
        const sentry sentry(*this);

        if(sentry)
        {
          optional<ulong> opt_value = rsl::internal::str_to_unsigned<ulong>(istreambuf_iterator(*this), 10);
          if(!opt_value.has_value())
          {
            base::setstate(io::iostate::failbit);
            value = 0;
          }
          else
          {
            value = opt_value.value();
          }
        }

        return *this;
      }

      // reads an float from the stream
      basic_istream& operator>>(float32& value)
      {
        const sentry sentry(*this);

        if(sentry)
        {
          optional<float32> opt_value = rsl::internal::str_to_floating_point<float32>(istreambuf_iterator(*this), 10);
          if(!opt_value.has_value())
          {
            base::setstate(io::iostate::failbit);
            value = 0;
          }
          else
          {
            value = opt_value.value();
          }
        }

        return *this;
      }
      // reads an float from the stream
      basic_istream& operator>>(float64& value)
      {
        const sentry sentry(*this);

        if(sentry)
        {
          // scan through istream, making sure we get new characters if we reach the end of the get area
          optional<float64> opt_value = rsl::internal::str_to_floating_point<float64>(istreambuf_iterator(*this), 10);
          if(!opt_value.has_value())
          {
            base::setstate(io::iostate::failbit);
            value = 0;
          }
          else
          {
            value = opt_value.value();
          }
        }

        return *this;
      }
      // reads an float from the stream
      basic_istream& operator>>(lfloat64& value)
      {
        const sentry sentry(*this);

        if(sentry)
        {
          optional<lfloat64> opt_value = rsl::internal::str_to_floating_point<lfloat64>(istreambuf_iterator(*this), 10);
          if(!opt_value.has_value())
          {
            base::setstate(io::iostate::failbit);
            value = 0;
          }
          else
          {
            value = opt_value.value();
          }
        }

        return *this;
      }

      // reads an bool from the stream
      basic_istream& operator>>(bool& value)
      {
        const sentry sentry(*this);

        if(sentry)
        {
          optional<bool> opt_value = rsl::internal::str_to_bool(istreambuf_iterator(*this));
          if(!opt_value.has_value())
          {
            base::setstate(io::iostate::failbit);
            value = 0;
          }
          else
          {
            value = opt_value.value();
          }
        }

        return *this;
      }

      // reads an  from the stream
      basic_istream& operator>>(void*& value)
      {
        const sentry sentry(*this);

        if(sentry)
        {
          optional<void*> opt_value = rsl::internal::str_to_pointer(istreambuf_iterator(*this));
          if(!opt_value.has_value())
          {
            base::setstate(io::iostate::failbit);
            value = 0;
          }
          else
          {
            value = opt_value.value();
          }
        }

        return *this;
      }

      // calls func(this). This function is implemented to enable I/O manipulators
      basic_istream& operator>>(ios_base& (*func)(ios_base&))
      {
        func(this);
      }
      // calls func(this). This function is implemented to enable I/O manipulators
      basic_istream& operator>>(basic_ios<CharT, Traits>& (*func)(basic_ios<CharT, Traits>&))
      {
        func(this);
      }
      // calls func(this). This function is implemented to enable I/O manipulators
      basic_istream& operator>>(basic_istream& (*func)(basic_istream&))
      {
        func(this);
      }

      // reads characters from the input sequence and inserts them into sb.
      basic_istream& operator>>(basic_streambuf<CharT, Traits>* sb)
      {
        count_t num_extracted = 0;
        const sentry sentry(*this);
        io::iostate state = io::iostate::goodbit;

        if(sentry)
        {
          for(int_type meta = base::rdbuf()->sgetc();; meta = base::rdbuf()->snextc())
          {
            if(traits_type::eq_int_type(traits_type::eof(), meta))
            {
              state |= io::iostate::eofbit;
              break;
            }

            if(traits_type::eq_int_type(traits_type::eof(), sb->sputc(traits_type::to_char_type(meta))))
            {
              break;
            }

            inc_gcount();
          }
        }

        if(num_extracted == 0)
        {
          state |= io::iostate::failbit;
        }

        base::setstate(state);
        return *this;
      }

      // reads one character and returns it if available
      int_type get()
      {
        int_type meta     = 0;
        io::iostate state = io::iostate::goodbit;

        const sentry sentry(*this);

        if(!sentry)
        {
          meta = traits_type::eof();
        }
        else
        {
          meta = base::rdbuf()->sgetc();

          if(traits_type::eq_int_type(traits_type::eof(), meta))
          {
            state |= io::iostate::eofbit | io::iostate::failbit;
          }
          else
          {
            base::rdbuf()->sbumpc();
            inc_gcount();
          }
        }

        base::setstate(state);
        return meta;
      }

      // reads one character and stores it to ch if available
      basic_istream& get(char_type& ch)
      {
        int_type meta = get();
        if(!traits_type::eq_int_type(traits_type::eof(), meta))
        {
          ch = traits_type::to_char_type(meta);
        }

        return *this;
      }
      // reads at most count - 1 characters, and stores them into character string
      // until '\n' is found.
      basic_istream& get(char_type* s, streamsize count)
      {
        return get(s, count, base::widen('\n'));
      }
      // reads at most count - 1 characters, and stores them into character string
      // until delim is found.
      basic_istream& get(char_type* s, streamsize count, char_type delim)
      {
        io::iostate state = io::iostate::goodbit;
        clear_gcount();
        const sentry sentry(*this);

        if(sentry && count > 0)
        {
          int_type meta = base::rdbuf()->sgetc();

          for(; 0 < --count; meta = base::rdbuf()->snextc())
          {
            if(traits_type::eq_int_type(traits_type::eof(), meta))
            {
              state |= io::iostate::eofbit;
              break;
            }
            else if(traits_type::to_char_type(meta) == delim)
            {
              break;
            }
            else
            {
              *s++ = traits_type::to_char_type(meta);
              inc_gcount();
            }
          }
        }

        base::setstate(m_gcount == 0 ? state | io::iostate::failbit : state);
        *s = char_type();
        return *this;
      }
      // reads available characters and inserts them to the given basic_streambuf
      // until '\n' is found.
      basic_istream& get(basic_streambuf<CharT, Traits>& strbuf)
      {
        return get(strbuf, base::widen('\n'));
      }
      // reads available characters and inserts them to the given basic_streambuf
      // until delim is found.
      basic_istream& get(basic_streambuf<CharT, Traits>& strbuf, char_type delim)
      {
        io::iostate state = io::iostate::goodbit;
        clear_gcount();
        const sentry sentry(*this);

        if(sentry)
        {
          int_type meta = base::rdbuf()->sgetc();

          for(;; meta = base::rdbuf()->snextc())
          {
            if(traits_type::eq_int_type(traits_type::eof(), meta))
            {
              state |= io::iostate::eofbit;
            }
            else
            {
              char_type ch = traits_type::to_char_type(meta);
              if(ch == delim || traits_type::eq_int_type(traits_type::eof(), strbuf.sputc(ch)))
              {
                break;
              }

              inc_gcount();
            }
          }
        }

        if(m_gcount == 0)
        {
          state |= io::iostate::failbit;
        }

        base::setstate(state);
        return *this;
      }

      // reads the next characters from the input stream without extracting it.
      int_type peek()
      {
        io::iostate state = io::iostate::goodbit;
        clear_gcount();
        int_type meta = 0;
        const sentry sentry(*this);

        if(!sentry)
        {
          meta = traits_type::eof();
        }
        else
        {
          if(traits_type::eq_int_type(traits_type::eof(), meta = base::rdbuf()->sgetc()))
          {
            state |= io::iostate::eofbit;
          }
        }

        base::setstate(state);
        return meta;
      }

      // makes the most recently extracted character available again.
      basic_istream& unget()
      {
        clear_gcount();
        io::iostate state     = io::iostate::goodbit;
        io::iostate old_state = base::rdstate();
        base::clear(old_state & ~io::iostate::eofbit);
        const sentry sentry(*this);

        if(sentry)
        {
          if(traits_type::eq_int_type(traits_type::eof(), base::rdbuf()->sungetc()))
          {
            state |= io::iostate::badbit | old_state;
          }
        }

        base::setstate(state);
        return *this;
      }

      // puts the character back to the input stream so the next extracted character will be ch
      basic_istream& putback(char_type ch)
      {
        clear_gcount();
        io::iostate state     = io::iostate::goodbit;
        io::iostate old_state = base::rdstate();
        base::clear(old_state & ~io::iostate::eofbit);
        const sentry sentry(*this);

        if(sentry)
        {
          if(traits_type::eq_int_type(traits_type::eof(), base::rdbuf()->sputbackc(ch)))
          {
            state |= io::iostate::badbit | old_state;
          }
        }

        base::setstate(state);
        return *this;
      }

      // extracts characters from stream until end of line
      basic_istream& getline(char_type* s, streamsize count)
      {
        return getline(s, count, base::widen('\n'));
      }

      // extracts characters from stream until delim is found
      basic_istream& getline(char_type* s, streamsize count, char_type delim)
      {
        io::iostate state = io::iostate::goodbit;
        clear_gcount();
        const sentry sentry(*this);

        if(sentry && count > 0)
        {
          int_type meta_delim = traits_type::to_int_type(delim);

          int_type meta = base::rdbuf()->sgetc();

          for(;; meta = base::rdbuf()->snextc())
          {
            if(traits_type::eq_int_type(traits_type::eof(), meta))
            {
              state |= io::iostate::eofbit;
              break;
            }
            else if(traits_type::eq_int_type(meta, meta_delim))
            {
              inc_gcount();
              base::rdbuf()->sbumpc();
              break;
            }
            else if(--count <= 0)
            {
              state |= io::iostate::failbit;
              break;
            }
            else
            {
              *s++ = traits_type::to_char_type(meta);
              inc_gcount();
            }
          }
        }

        *s = char_type();
        base::setstate(m_gcount == 0 ? state | io::iostate::failbit : state);
        return *this;
      }

      // extracts and discards characters from the input stream until and including delim
      basic_istream& ignore(streamsize count = 1, int_type delim = Traits::eof())
      {
        io::iostate state = io::iostate::goodbit;
        clear_gcount();
        const sentry ok(*this);

        if(ok && count > 0)
        {
          for(;;)
          {
            int_type meta;
            if(count != (numeric_limits<streamsize>::max)() && --count < 0)
            {
              break;
            }
            else if(traits_type::eq_int_type(Traits::eof(), meta = base::rdbuf()->sbumpc()))
            {
              state |= io::iostate::eofbit;
              break;
            }
            else
            {
              inc_gcount();
              if(meta == delim)
              {
                break;
              }
            }
          }
        }

        base::setstate(state);
        return *this;
      }

      // extracts characters from the stream
      basic_istream& read(char_type* s, streamsize count)
      {
        io::iostate state = io::iostate::goodbit;
        clear_gcount();
        const sentry sentry(*this);

        if(sentry && count > 0)
        {
          const streamsize num = base::rdbuf()->sgetn(s, count);
          m_gcount             = num;

          if(num != count)
          {
            state |= io::iostate::eofbit | io::iostate::failbit;
          }
        }

        base::setstate(state);
        return *this;
      }

      // extracts up to count immediately available characters from the input stream
      // the behavior is specific to which streambuf is used
      streamsize readsome(char_type* s, streamsize count)
      {
        io::iostate state = io::iostate::goodbit;
        clear_gcount();
        const sentry sentry(*this);
        streamsize num;

        if(!sentry)
        {
          state != io::iostate::failbit;
        }
        else if((num = base::rdbuf()->in_avail()) < 0)
        {
          state = io::iostate::eofbit;
        }
        else if(count > 0 && num > 0)
        {
          read(s, num < count ? num : count);
        }

        base::setstate(state);
        return gcount();
      }

      // returns the number of characters extracted by the last unformatted input operation
      // gcount() is changed by the following functions
      // - move constructor
      // - swap
      // - get
      // - getline
      // - ignore
      // - read
      // - readsome
      // - operator>>(basic_streambuf*)
      // the following functions set gcount() to zero
      // - constructor
      // - putback
      // - unget
      // - peek
      streamsize gcount() const
      {
        return m_gcount;
      }

      // returns input position indicator of the current associated streambuf object
      pos_type tellg()
      {
        const sentry sentry(*this);

        if(!base::fail())
        {
          return base::rdbuf()->pubseekoff(0, io::iostate::cur, io::openmode::in);
        }
        else
        {
          return pos_type(-1);
        }
      }

      // sets input position indicator of the current associated streambuf object
      basic_istream& seekg(off_type off, io::seekdir dir)
      {
        io::iostate state     = io::iostate::goodbit;
        io::iostate old_state = base::rdstate();
        base::clear(old_state & ~io::iostate::eofbit);
        const sentry sentry(*this);

        if(!base::fail() && static_cast<off_type>(base::rdbuf()->pubseekoff(off, dir, io::openmode::in)) == -1)
        {
          base::setstate(state | io::iostate::failbit);
        }

        return *this;
      }

      // synchronizes the input buffer with the associated data source
      int32 sync()
      {
        const sentry sentry(*this);

        const basic_streambuf<CharT, Traits>* rdbuf = base::rdbuf();
        if(!rdbuf)
        {
          return -1;
        }

        bool sync_failed = true;
        sync_failed      = rdbuf->pubsync() == -1;
        if(sync_failed)
        {
          base::setstate(io::iostate::badbit);
          return -1;
        }

        return 0;
      }

    protected:
      // the move constructor copies the value of gcount() from other,
      // sets the gcount() value of other to zero and uses basic_ios<CharT, Traits>::move(other)
      // to move all basic_ios members, except for the rdbuf(), from other into this.
      basic_istream(basic_istream&& other);

      // exchanges the gcount() values and all data members of the base class,
      // except for the rdbuf(), with other.
      basic_istream& operator=(basic_istream&& other)
      {
        swap(other);
        return *this;
      }

      // swaps all data members except for rdbuf(), and swaps the values of the gcount() counters.
      void swap(basic_istream& other)
      {
        REX_ASSERT_X(this != addressof(other), "Can't swap to yourself");
        base::swap(other);
      }

    private:
      void clear_gcount()
      {
        m_gcount = 0;
      }

      void inc_gcount()
      {
        ++m_gcount;
      }

    private:
      count_t m_gcount;
    };

    /// RSL Comment: Different from ISO C++ Standard at time of writing (07/Sep/2022)
    // to avoid the complexity of the stl, you can only output to a basic_istream
    // you can only read the character type that is the same as the basic_istream's char_type

    template <typename CharT, typename Traits>
    basic_istream<CharT, Traits>& operator>>(basic_istream<CharT, Traits>& str, CharT& ch)
    {
      using my_istream = basic_istream<CharT, Traits>;

      typename my_istream::int_type meta;
      io::iostate state = io::iostate::goodbit;
      const typename my_istream::sentry sentry(str);

      if(sentry)
      {
        meta = str.rdbuf()->sbumpc();
        if(Traits::eq_int_type(Traits::eof(), meta))
        {
          state |= io::iostate::eofbit | io::iostate::failbit;
        }
        else
        {
          ch = Traits::to_char_type(meta);
        }
      }

      str.setstate(state);
      return str;
    }

    template <typename CharT, typename Traits, count_t N>
    basic_istream<CharT, Traits>& operator>>(basic_istream<CharT, Traits>& str, CharT (&s)[N]) // NOLINT(modernize-avoid-c-arrays)
    {
      using my_istream  = basic_istream<CharT, Traits>;
      io::iostate state = io::iostate::goodbit;
      count_t current   = 0;
      const typename my_istream::sentry sentry(str);

      if(sentry)
      {
        typename my_istream::int_type meta = str.rdbuf()->sgetc();
        CharT ch;
        for(; current < N - 1; meta = str.rdbuf()->snextc(), ++current)
        {
          if(Traits::eq_int_type(Traits::eof(), meta))
          {
            state |= io::iostate::eofbit;
            break;
          }
          else if(is_space(ch = Traits::to_char_type(meta)) || ch == CharT())
          {
            break;
          }
          else
          {
            s[current] = Traits::to_char_type(meta);
          }
        }
      }

      s[current] = CharT();
      if(current == 0)
      {
        state |= io::iostate::failbit;
      }

      str.setstate(state);
      return str;
    }

    namespace internal
    {
      template <typename IStream, typename T, typename = void>
      struct can_stream_in : false_type
      {
      };

      template <typename IStream, typename T>
      struct can_stream_in<IStream, T, void_t<decltype(rsl::declval<IStream&>() >> rsl::declval<T>())>> : true_type
      {
      };
    } // namespace internal

    template <typename IStream, typename T, enable_if_t<conjunction_v<is_convertible<IStream*, ios_base*>, internal::can_stream_in<IStream, T>>, bool> = true>
    IStream&& operator>>(IStream&& is, T&& val)
    {
      is >> rsl::forward<T>(val);
      return rsl::move(is);
    }

  } // namespace v1
} // namespace rsl