// ============================================
//
// REX - STANDARD LIBRARY IMPLEMENTATION
//
// Author: Nick De Breuck
// Twitter: @nick_debreuck
//
// File: console_buf.cpp
// Copyright (c) Nick De Breuck 2022
//
// ============================================

#include "rex_std/bonus/iostream/console_buf.h"

#include "rex_std/internal/assert/assert_fwd.h"
#include "rex_std/internal/utility/size.h"

#include <Windows.h>

namespace rsl
{
  inline namespace v1
  {

    namespace internal
    {
      console_buf_impl::console_buf_impl(win::handle_t handle)
          : m_get_area()
          , m_handle(handle)
      {
      }

      console_buf_impl::~console_buf_impl()
      {
        m_get_area.deallocate();
      }

      streamsize console_buf_impl::xsgetn(char8* s, size_t elemSize, streamsize count)
      {
        DWORD num_read = 0;
        ReadFile(m_handle, s, static_cast<DWORD>(count * elemSize), &num_read, nullptr);
        return static_cast<streamsize>(num_read);
      }
      streamsize console_buf_impl::xsputn(const char8* s, size_t elemSize, streamsize count)
      {
        DWORD num_written = 0;
        WriteFile(m_handle, s, static_cast<DWORD>(count * elemSize), &num_written, nullptr);
        return static_cast<streamsize>(num_written);
      }

    } // namespace internal
  }   // namespace v1
} // namespace rsl