// ============================================
//
// REX - STANDARD LIBRARY IMPLEMENTATION
//
// Author: Nick De Breuck
// Twitter: @nick_debreuck
//
// File: handle.cpp
// Copyright (c) Nick De Breuck 2022
//
// ============================================

#include "rex_std/bonus/platform/windows/handle.h"

#include <Windows.h>

namespace rsl
{
  inline namespace v1
  {
    namespace win
    {
      static_assert(sizeof(HANDLE) == sizeof(handle_t), "Rex internal handle should be the same as Windows' HANDLE");

      handle::handle()
          : m_handle(invalid_value())
      {
      }
      handle::handle(handle_t handle)
          : m_handle(handle)
      {
      }
      handle::handle(handle&& other)
          : m_handle(other.m_handle)
      {
        other.m_handle = INVALID_HANDLE_VALUE;
      }

      handle::~handle()
      {
        CloseHandle(m_handle);
      }

      handle& handle::operator=(handle&& other)
      {
        m_handle       = other.m_handle;
        other.m_handle = nullptr;
        return *this;
      }

      bool handle::is_valid() const
      {
        return m_handle != invalid_value();
      }
      handle::operator bool() const
      {
        return is_valid();
      }

      handle_t handle::invalid_value()
      {
        return INVALID_HANDLE_VALUE;
      }
    } // namespace win
  }   // namespace v1
} // namespace rsl