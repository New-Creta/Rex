// ============================================
//
// REX - STANDARD LIBRARY IMPLEMENTATION
//
// Author: Nick De Breuck
// Twitter: @nick_debreuck
//
// File: console_buf.h
// Copyright (c) Nick De Breuck 2022
//
// ============================================

//-----------------------------------------------------------------------------
// This is a streambuf derived object used for streams that output to the console.
//-----------------------------------------------------------------------------

#pragma once

#include "rex_std/bonus/platform/windows/handle.h"
#include "rex_std/internal/memory/memcpy.h"
#include "rex_std/internal/streambuf/basic_streambuf.h"

namespace rsl
{
  inline namespace v1
  {

    namespace internal
    {
      class get_area
      {
      public:
        get_area()
            : m_begin(nullptr)
            , m_current(nullptr)
            , m_end(nullptr)
            , m_last(nullptr)
        {
        }

        void allocate()
        {
          m_begin   = new char8[s_stream_size];
          m_current = m_begin;
          m_end     = m_current;
          m_last    = m_begin + s_stream_size;
        }

        void deallocate()
        {
          delete[] m_begin;
        }

        bool is_allocated() const
        {
          return m_begin != nullptr;
        }

        char8*& begin()
        {
          return m_begin;
        }
        char8*& current()
        {
          return m_current;
        }
        char8*& end()
        {
          return m_end;
        }
        char8*& last()
        {
          return m_last;
        }

        void inc_current(count_t amount = 1)
        {
          m_current += amount;
        }
        void inc_end(count_t amount = 1)
        {
          m_end += amount;
        }
        void reset_current()
        {
          m_current = m_begin;
        }
        void reset()
        {
          reset_current();
          m_end = m_current;
        }

        streamsize num_available() const
        {
          return static_cast<streamsize>(m_end - m_current);
        }
        streamsize num_available_to_load() const
        {
          return static_cast<streamsize>(m_last - m_end);
        }

      private:
        char8* m_begin;   // beginning of buffer
        char8* m_current; // next char in the buffer to read
        char8* m_end;     // end of chars loaded into the buffer
        char8* m_last;    // 1 past the end of the buffer

        static constexpr streamsize s_stream_size = 256;
      };

      class console_buf_impl
      {
      public:
        explicit console_buf_impl(win::handle_t handle);
        console_buf_impl(const console_buf_impl&) = delete;
        console_buf_impl(console_buf_impl&&)      = delete;
        ~console_buf_impl();

        console_buf_impl& operator=(const console_buf_impl&) = delete;
        console_buf_impl& operator=(console_buf_impl&&)      = delete;

        streamsize xsgetn(char8* s, size_t elemSize, streamsize count);
        streamsize xsputn(const char8* s, size_t elemSize, streamsize count);

        template <typename CharT, typename Traits>
        typename Traits::int_type overflow(CharT ch)
        {
          return (xsputn(&ch, sizeof(CharT), 1) / sizeof(CharT)) == 0 ? Traits::eof() : ch;
        }
        template <typename CharT, typename Traits>
        typename Traits::int_type overflown(const CharT* s, count_t count)
        {
          return (xsputn(s, sizeof(CharT), count) / sizeof(CharT)) != count ? Traits::eof() : CharT();
        }

        template <typename CharT>
        struct underflow_result
        {
          get_area* get_area;
          CharT ch;
        };
        template <typename CharT, typename Traits>
        underflow_result<CharT> underflow() // we reached the end of the get area, get from stream, store first char in stream
        {
          if(!m_get_area.is_allocated())
          {
            m_get_area.allocate();
          }

          if(m_get_area.num_available_to_load() == 0)
          {
            m_get_area.reset();
          }

          const streamsize count_read = xsgetn(m_get_area.current(), sizeof(CharT), 1) / sizeof(CharT);
          m_get_area.inc_end();

          underflow_result<CharT> res {};
          res.get_area = &m_get_area;
          res.ch       = count_read == 0 ? Traits::to_char_type(Traits::eof()) : *res.get_area->current();

          return res;
        }
        template <typename CharT, typename Traits>
        underflow_result<CharT> uflow() // we reached the end of the get area, get from stream, inc pointer
        {
          if(!m_get_area.is_allocated())
          {
            m_get_area.allocate();
          }

          if(m_get_area.num_available_to_load() == 0)
          {
            m_get_area.reset();
          }

          const streamsize count_read = xsgetn(m_get_area.current(), sizeof(CharT), 1) / sizeof(CharT);

          m_get_area.inc_current();
          m_get_area.inc_end();

          underflow_result<CharT> res {};

          res.get_area = &m_get_area;
          res.ch       = count_read == 0 ? Traits::to_char_type(Traits::eof()) : *res.get_area->current();

          return res;
        }
        template <typename CharT>
        struct uflown_result
        {
          get_area* get_area;
          streamsize num_read;
        };
        template <typename CharT, typename Traits>
        uflown_result<CharT> uflown(CharT* s, streamsize count) // we reached the end of the get area, get from stream, inc pointer
        {
          // we're not checking if the get area is allocated because we don't have to.
          // if there's no get area, we load the directly from the character sequence
          // directly into the destination.
          // This means if an invalid read happened, there's no way to put it back

          const streamsize num_available = m_get_area.num_available();
          if(count > num_available)
          {
            // first load the characters in the get area
            rsl::memcpy(s, m_get_area.current(), sizeof(CharT) * num_available);
            m_get_area.reset();
          }

          const streamsize count_read = xsgetn(s, sizeof(CharT), count) / sizeof(CharT);

          uflown_result<CharT> res {};
          res.get_area = &m_get_area;
          res.num_read = count_read;

          return res;
        }

        get_area area() const
        {
          return m_get_area;
        }

      private:
        get_area m_get_area;
        win::handle_t m_handle;
      };
    } // namespace internal

    template <typename CharT, typename Traits = char_traits<CharT>>
    class console_buf : public basic_streambuf<CharT, Traits>
    {
    private:
      using base = basic_streambuf<char8, char_traits<char8>>;

    public:
      using char_type   = typename base::char_type;
      using traits_type = typename base::traits_type;
      using int_type    = typename base::int_type;
      using pos_type    = typename base::pos_type;
      using off_type    = typename base::off_type;

      explicit console_buf(win::handle_t handle)
          : m_impl(handle)
      {
      }

    protected:
      streamsize xsgetn(char_type* s, streamsize count) final
      {
        streamsize num_chars_read              = 0;
        const streamsize to_read_from_get_area = (rsl::min)(count, num_available_in_get_area());
        // first get all the elements from the get area.
        // these are copied there before from the character sequence.
        // We can't get them back from the get area that's managed by the operating system.
        if(to_read_from_get_area > 0)
        {
          Traits::copy(s, base::gptr(), to_read_from_get_area);
          base::inc_gptr(to_read_from_get_area);
          num_chars_read += to_read_from_get_area;
          s += to_read_from_get_area;
          count -= to_read_from_get_area;
        }

        // if we still need to get characters, get them directly from the operating system.
        if(count > 0)
        {
          const streamsize num_bytes_read = m_impl.xsgetn(s, sizeof(CharT), count);
          num_chars_read += num_bytes_read / sizeof(CharT);
        }

        return num_chars_read;
      }
      streamsize xsputn(const char_type* s, streamsize count) final
      {
        // we don't need to create temporary buffers, just pass them directly to the OS.
        return m_impl.xsputn(s, sizeof(char_type), count);
      }

      int_type overflow(int_type ch) final
      {
        // pass it directly to the OS.
        return m_impl.overflow<char_type, traits_type>(traits_type::to_char_type(ch));
      }
      streamsize overflown(const char_type* s, streamsize count) final
      {
        return m_impl.overflown<char_type, traits_type>(s, count);
      }
      int_type underflow() final
      {
        if(base::gptr() && base::gptr() < base::egptr()) // can we still get characters from the get area?
        {
          return traits_type::to_int_type(*base::gptr());
        }
        else // get it from the character sequence
        {
          internal::console_buf_impl::underflow_result<CharT> res = m_impl.underflow<char_type, traits_type>();

          // it's possible we allocated a intermediate buffer, so we have to set the pointers
          base::setg(&res.get_area->begin(), &res.get_area->current(), &res.get_area->end());
          return res.ch;
        }
      }
      int_type uflow() final
      {
        if(base::gptr() && base::gptr() < base::egptr()) // can we still get characters from the get area?
        {
          return traits_type::to_int_type(*base::gptr());
        }
        else // get it from the character sequence
        {
          internal::console_buf_impl::underflow_result<CharT> res = m_impl.uflow<char_type, traits_type>();

          // it's possible we allocated a intermediate buffer, so we have to set the pointers
          base::setg(&res.get_area->begin(), &res.get_area->current(), &res.get_area->end());
          return res.ch;
        }
      }
      streamsize uflown(char_type* s, streamsize count) final
      {
        if(base::gptr() && base::gptr() < base::egptr()) // can we still get characters from the get area?
        {
          return traits_type::to_int_type(*base::gptr());
        }
        else // get it from the character sequence
        {
          internal::console_buf_impl::uflown_result<char_type> res = m_impl.uflown<char_type, traits_type>(s, count);

          // it's possible we allocated a intermediate buffer, so we have to set the pointers
          base::setg(&res.get_area->begin(), &res.get_area->current(), &res.get_area->end());
          return res.num_read;
        }
      }

      streamsize available_in_get_area() const final
      {
        return m_impl.area().num_available();
      }

    private:
      streamsize num_available_in_get_area() const
      {
        return m_impl.area().num_available();
      }

    private:
      internal::console_buf_impl m_impl;
    };

  } // namespace v1
} // namespace rsl