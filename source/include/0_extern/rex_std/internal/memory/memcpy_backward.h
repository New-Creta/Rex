// ============================================
//
// REX - STANDARD LIBRARY IMPLEMENTATION
//
// Author: Nick De Breuck
// Twitter: @nick_debreuck
//
// File: memcpy_backward.h
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

    constexpr void* memcpy_backward(void* dst, const void* src, card32 len)
    {
      byte* dst_byte       = static_cast<byte*>(dst) + len;
      const byte* src_byte = static_cast<const byte*>(src) + len;

      for(card32 i = 0; i < len; ++i)
      {
        *dst_byte = *src_byte;
        --dst_byte;
        --src_byte;
      }
      return dst;
    }

  } // namespace v1
} // namespace rsl
