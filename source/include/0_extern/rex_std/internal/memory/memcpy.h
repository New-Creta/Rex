// ============================================
//
// REX - STANDARD LIBRARY IMPLEMENTATION
//
// Author: Nick De Breuck
// Twitter: @nick_debreuck
//
// File: memcpy.h
// Copyright (c) Nick De Breuck 2022
//
// ============================================

#pragma once

#include "rex_std/bonus/types.h"
#include "rex_std/internal/memory/byte.h"

namespace rsl
{
  inline namespace v1
  {

    // TODO: are there more performant ways of copying bytes over that is defined behavior?
    // Maybe cast the memory to int32 or int64, so we can copy over more bytes in 1 go
    constexpr void* memcpy(void* dst, const void* src, count_t len)
    {
      byte* dst_byte       = static_cast<byte*>(dst);
      const byte* src_byte = static_cast<const byte*>(src);

      for(count_t i = 0; i < len; ++i)
      {
        *dst_byte = *src_byte;
        ++dst_byte;
        ++src_byte;
      }
      return dst;
    }

  } // namespace v1
} // namespace rsl
