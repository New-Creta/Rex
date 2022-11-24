// ============================================
//
// REX - STANDARD LIBRARY IMPLEMENTATION
//
// Author: Nick De Breuck
// Twitter: @nick_debreuck
//
// File: fpos.h
// Copyright (c) Nick De Breuck 2022
//
// ============================================

//-----------------------------------------------------------------------------
// https://en.cppreference.com/w/cpp/io/fpos
//
// specialization of the class template identify absolute positions in a stream or in a file.
// each object of type fpos holds the byte position in the stream and the current shift state,
// a value of type State.
//-----------------------------------------------------------------------------

#pragma once

#include "rex_std/bonus/types.h"
#include "rex_std/internal/ios/io_types.h"
#include "rex_std/internal/type_traits/enable_if.h"
#include "rex_std/internal/type_traits/is_integral.h"
#include "rex_std/internal/wchar/mbstate.h"

namespace rsl
{
  inline namespace v1
  {

    using fpos_t = int64;

    template <typename State>
    class fpos
    {
    public:
      /* implicit */ fpos(streamoff off = 0) // NOLINT(google-explicit-constructor)
          : m_offset(off)
          , m_fpos(0)
          , m_state()
      {
      }

      fpos(State state, fpos_t filePos)
          : m_offset(filePos)
          , m_fpos(0)
          , m_state(state)
      {
      }

      State state() const
      {
        return m_state;
      }

      void state(State state)
      {
        m_state = state;
      }

      operator streamoff() const // NOLINT(google-explicit-constructor)
      {
        return m_offset + m_fpos;
      }

      streamoff operator-(const fpos& right) const
      {
        return static_cast<streamoff>(*this) - static_cast<streamoff>(right);
      }

      fpos& operator+=(streamoff off)
      {
        m_offset += off;
        return *this;
      }

      fpos& operator-=(streamoff off)
      {
        m_offset -= off;
        return *this;
      }

      fpos operator+(streamoff off) const
      {
        fpos tmp = *this;
        tmp += off;
        return tmp;
      }

      fpos operator-(streamoff off) const
      {
        fpos tmp = *this;
        tmp -= off;
        return tmp;
      }

      bool operator==(const fpos& right) const
      {
        return static_cast<streamoff>(*this) == static_cast<streamoff>(right);
      }

      template <typename Int, enable_if_t<is_integral_v<Int>, int> = 0>
      friend bool operator==(const fpos& left, const Int right)
      {
        return static_cast<streamoff>(left) == right;
      }

      template <typename Int, enable_if_t<is_integral_v<Int>, int> = 0>
      friend bool operator==(const Int left, const fpos& right)
      {
        return left == static_cast<streamoff>(right);
      }

      bool operator!=(const fpos& right) const
      {
        return static_cast<streamoff>(*this) != static_cast<streamoff>(right);
      }

      template <typename Int, enable_if_t<is_integral_v<Int>, int> = 0>
      friend bool operator!=(const fpos& left, const Int right)
      {
        return static_cast<streamoff>(left) != right;
      }

      template <typename Int, enable_if_t<is_integral_v<Int>, int> = 0>
      friend bool operator!=(const Int left, const fpos& right)
      {
        return left != static_cast<streamoff>(right);
      }

    private:
      streamoff m_offset;
      fpos_t m_fpos;
      State m_state;
    };

    using streampos    = fpos<mbstate_t>;
    using wstreampos   = fpos<mbstate_t>;
    using u16streampos = fpos<mbstate_t>;
    using u32streampos = fpos<mbstate_t>;

  } // namespace v1
} // namespace rsl