// ============================================
//
// REX - STANDARD LIBRARY IMPLEMENTATION
//
// Author: Nick De Breuck
// Twitter: @nick_debreuck
//
// File: c_string.h
// Copyright (c) Nick De Breuck 2022
//
// ============================================

#pragma once

#include "rex_std/bonus/defines.h"

namespace rsl
{
  inline namespace v1
  {

    template <typename T>
    class c_string
    {
    public:
      c_string(const T* ptr) // NOLINT(google-explicit-constructor): this needs to be implicit
          : m_ptr(ptr)
      {
      }

      const T* ptr() const
      {
        return m_ptr;
      }

    private:
      const T* m_ptr;
    };

  } // namespace v1
} // namespace rsl
