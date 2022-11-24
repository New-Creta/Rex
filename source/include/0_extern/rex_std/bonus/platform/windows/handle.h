// ============================================
//
// REX - STANDARD LIBRARY IMPLEMENTATION
//
// Author: Nick De Breuck
// Twitter: @nick_debreuck
//
// File: handle.h
// Copyright (c) Nick De Breuck 2022
//
// ============================================

//-----------------------------------------------------------------------------
// This is a wrapper around Window's HANDLE structure.
// It enables RAII for HANDLE objects as well as some helper functions
//-----------------------------------------------------------------------------

#pragma once

#include "rex_std/bonus/defines.h"

namespace rsl
{
  inline namespace v1
  {
    namespace win
    {
      using handle_t = void*;

      class handle
      {
      public:
        handle();
        explicit handle(handle_t handle);
        handle(const handle&) = delete;
        handle(handle&& other);
        ~handle();

        handle& operator=(const handle&) = delete;
        handle& operator=(handle&& other);

        bool is_valid() const;
        explicit operator bool() const;

        static handle_t invalid_value();

      private:
        handle_t m_handle;
      };
    } // namespace win
  }   // namespace v1
} // namespace rsl
