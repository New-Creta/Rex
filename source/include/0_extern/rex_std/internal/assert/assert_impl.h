// ============================================
//
// REX - STANDARD LIBRARY IMPLEMENTATION
//
// Author: Nick De Breuck
// Twitter: @nick_debreuck
//
// File: assert_impl.h
// Copyright (c) Nick De Breuck 2022
//
// ============================================

//-----------------------------------------------------------------------------
// Implementation of the assert function. This is implemented in a different file
// to avoid circular inclusion issues.
//-----------------------------------------------------------------------------

#pragma once

#include "rex_std/format.h"
#include "rex_std/internal/utility/forward.h"
#include "rex_std/string.h"

namespace rsl
{
  inline namespace v1
  {
    namespace internal
    {
      void log_assert(const rsl::string& msg);
    } // namespace internal

    template <typename... Args>
    bool rex_assert(bool cond, Args&&... args)
    {
#ifdef _MSC_VER
      if(cond)
      {
        rsl::string str = rsl::format(rsl::forward<Args>(args)...);
        internal::log_assert(str);
        __debugbreak();
        return true;
      }
#endif // _MSC_VER

      return false;
    }

  } // namespace v1
} // namespace rsl