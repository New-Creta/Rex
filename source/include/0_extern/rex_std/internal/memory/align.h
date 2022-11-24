// ============================================
//
// REX - STANDARD LIBRARY IMPLEMENTATION
//
// Author: Nick De Breuck
// Twitter: @nick_debreuck
//
// File: align.h
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

    void* align(count_t alignment, size_t size, void*& ptr, size_t space)
    {
      if(space >= size)
      {
        byte* ptrAligned = (byte*)(((size_t)ptr + (alignment - 1)) & -alignment);
        size_t offset    = (size_t)(ptrAligned - (byte*)ptr);

        if((space - size) >= offset) // have to implement this in terms of subtraction instead of addition in order to handle possible overflow.
        {
          ptr = ptrAligned;
          space -= offset;

          return ptrAligned;
        }
      }

      return nullptr;
    }

  } // namespace v1
} // namespace rsl
