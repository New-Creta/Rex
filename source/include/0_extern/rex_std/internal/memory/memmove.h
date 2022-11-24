// ============================================
//
// REX - STANDARD LIBRARY IMPLEMENTATION
//
// Author: Nick De Breuck
// Twitter: @nick_debreuck
//
// File: memmove.h
// Copyright (c) Nick De Breuck 2022
//
// ============================================

#pragma once

#include "rex_std/bonus/types.h"
#include "rex_std/internal/memory/memcpy.h"
#include "rex_std/internal/memory/memcpy_backward.h"

namespace rsl
{
  inline namespace v1
  {

    constexpr void* memmove(void* dst, const void* src, card32 len)
    {
      // If the buffers don't overlap, it doesn't matter what direction
      // we copy in. If they do overlap, we can't copy from front to back
      // If they overlap and src sits before dst, we need to copy from
      // back to front
      // If the destination is below the source, we copy front to back
      if(src < dst)
      {
        return memcpy_backward(dst, src, len);
      }
      else
      {
        return memcpy(dst, src, len);
      }
    }

  } // namespace v1
} // namespace rsl
