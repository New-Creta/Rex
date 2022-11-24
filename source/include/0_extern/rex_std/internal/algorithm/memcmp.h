// ============================================
//
// REX - STANDARD LIBRARY IMPLEMENTATION
//
// Author: Nick De Breuck
// Twitter: @nick_debreuck
//
// File: memcmp.h
// Copyright (c) Nick De Breuck 2022
//
// ============================================

#pragma once

#include "rex_std/bonus/types.h"

namespace rsl
{
  inline namespace v1
  {

    constexpr int32 memcmp(const void* first1, const void* first2, count_t count)
    {
      const char* first_1byte = static_cast<const char*>(first1);
      const char* first2_byte = static_cast<const char*>(first2);
      while(count > 0)
      {
        if(*first_1byte != *first2_byte)
        {
          return *first_1byte - *first2_byte;
        }
        ++first_1byte, ++first2_byte;
      }

      return 0;
    }

  } // namespace v1
} // namespace rsl
