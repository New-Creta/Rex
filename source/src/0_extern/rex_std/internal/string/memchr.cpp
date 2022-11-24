// ============================================
//
// REX - STANDARD LIBRARY IMPLEMENTATION
//
// Author: Nick De Breuck
// Twitter: @nick_debreuck
//
// File: memchr.cpp
// Copyright (c) Nick De Breuck 2022
//
// ============================================

#include "rex_std/internal/string/memchr.h"

namespace rsl
{
  inline namespace v1
  {

    void* memchr(const void* ptr, char8 ch, card32 length)
    {
      const char8* str = reinterpret_cast<const char8*>(ptr); // NOLINT(cppcoreguidelines-pro-type-reinterpret-cast)

      for(card32 n = 0; n < length; n++)
      {
        if(str[n] == ch)
        {
          return reinterpret_cast<void*>(const_cast<char8*>(str + n)); // NOLINT
        }
      }

      return nullptr;
    }

  } // namespace v1
} // namespace rsl