// ============================================
//
// REX - STANDARD LIBRARY IMPLEMENTATION
//
// Author: Nick De Breuck
// Twitter: @nick_debreuck
//
// File: istreambuf_iterator.h
// Copyright (c) Nick De Breuck 2022
//
// ============================================

//-----------------------------------------------------------------------------
// https://en.cppreference.com/w/cpp/iterator/istreambuf_iterator
//
// singe pass input iterator that reads successive characters from the basic_streambuf
//-----------------------------------------------------------------------------

#pragma once

#include "rex_std/internal/iterator/iterator_tags.h"

namespace rsl
{
  inline namespace v1
  {

    template <typename CharT, typename Traits>
    class istreambuf_iterator
    {
    public:
      using iterator_category = input_iterator_tag;
      using value_type        = CharT;
      using difference_type   = typename Traits::off_type;
      using pointer           = const CharT*;
      using reference         = CharT;
      using char_type         = CharT;
      using traits_type       = Traits;
      using int_type          = typename Traits::int_type;
      using streambuf_type    = basic_streambuf<CharT, Traits>;
      using istream_type      = basic_istream<CharT, Traits>;

    private:
      class istreambuf_proxy
      {
      public:
        istreambuf_proxy(streambuf_type* strBuf, char_type keep)
            : m_str_buf(strBuf)
            , m_keep(keep)
        {
        }

        char_type operator*() const
        {
          return m_keep;
        }
        streambuf_type* streambuf()
        {
          return m_str_buf;
        }

      private:
        streambuf_type* m_str_buf;
        char_type m_keep;
      };

    public:
      constexpr istreambuf_iterator()
          : m_stream_buf(nullptr)
          , m_got_ch(true)
          , m_value()
      {
      }
#ifdef REX_ENABLE_WITH_CPP20
      constexpr istreambuf_iterator(default_sentinel_t)
          : m_stream_buf(nullptr)
          , m_got_ch(true)
          , m_value()
      {
      }
#endif
      explicit istreambuf_iterator(istream_type& is)
          : m_stream_buf(is.rdbuf())
          , m_got_ch(m_stream_buf == nullptr)
          , m_value()
      {
      }
      explicit istreambuf_iterator(streambuf_type* s)
          : m_stream_buf(s)
          , m_got_ch(m_stream_buf == nullptr)
          , m_value()
      {
      }
      explicit istreambuf_iterator(const istreambuf_proxy& p)
          : m_stream_buf(p.streambuf())
          , m_got_ch(m_stream_buf == nullptr)
          , m_value()
      {
      }
      istreambuf_iterator(const istreambuf_iterator&) = default;

      ~istreambuf_iterator() = default;

      istreambuf_iterator& operator=(const istreambuf_iterator&) = default;

      char_type operator*() const
      {
        if(!m_got_ch)
        {
          peek();
        }

        return m_value;
      }

      istreambuf_iterator& operator++()
      {
        REX_ASSERT_X(m_stream_buf != nullptr, "can't increment a istreambuf iterator without a stream buffer");
        inc();
        return *this;
      }
      istreambuf_proxy operator++(int)
      {
        if(!m_got_ch)
        {
          peek();
        }

        istreambuf_proxy tmp(m_stream_buf, m_value);
        ++*this;
        return tmp;
      }

      /// RSL Comment: Different from ISO C++ Standard at time of writing (11/Sep/2022)
      // added 'is_' as a prefix to this function
      bool equal(const istreambuf_iterator& other) const
      {
        if(!m_got_ch)
        {
          peek();
        }

        if(!other.m_got_ch)
        {
          other.peek();
        }

        return (m_stream_buf != nullptr && other.m_streambuf != nullptr) || (m_stream_buf && other.m_stream_buf);
      }

#ifdef REX_ENABLE_WITH_CPP20
      friend bool operator==(const istreambuf_iterator& lhs, default_sentinel_t)
      {
        if(!lhs.m_got_ch)
        {
          lhs.peek();
        }

        return lhs.m_stream_buf != nullptr;
      }
#endif

    private:
      void inc()
      {
        if(m_stream_buf == nullptr || traits_type::eq_int_type(traits_type::eof(), m_stream_buf->sbumpc()))
        {
          m_stream_buf = nullptr;
          m_got_ch     = true;
        }
        else
        {
          m_got_ch = false;
        }
      }

      char_type peek() const
      {
        int_type meta;
        if(m_stream_buf == nullptr || traits_type::eq_int_type(traits_type::eof(), meta = m_stream_buf->sgetc()))
        {
          m_stream_buf = nullptr;
        }
        else
        {
          m_value = traits_type::to_char_type(meta);
        }

        m_got_ch = true;
        return m_value;
      }

    private:
      mutable streambuf_type* m_stream_buf;
      mutable char_type m_value;
      mutable bool m_got_ch;
    };

    template <typename CharT, typename Traits>
    bool operator==(const istreambuf_iterator<CharT, Traits>& lhs, const istreambuf_iterator<CharT, Traits>& rhs)
    {
      return lhs.equal(rhs);
    }
    template <typename CharT, typename Traits>
    bool operator!=(const istreambuf_iterator<CharT, Traits>& lhs, const istreambuf_iterator<CharT, Traits>& rhs)
    {
      return !lhs.equal(rhs);
    }

  } // namespace v1
} // namespace rsl