// ============================================
//
// REX - STANDARD LIBRARY IMPLEMENTATION
//
// Author: Nick De Breuck
// Twitter: @nick_debreuck
//
// File: destroy.h
// Copyright (c) Nick De Breuck 2022
//
// ============================================

//-----------------------------------------------------------------------------
// https://en.cppreference.com/w/cpp/memory/destroy
//
// Destroy a range of objects
//-----------------------------------------------------------------------------

#pragma once

#include "rex_std/internal/memory/addressof.h"
#include "rex_std/internal/memory/destroy_at.h"

namespace rsl
{
  inline namespace v1
  {

    template <typename ForwardIt>
    constexpr void destroy(ForwardIt first, ForwardIt last)
    {
      for(; first != last; ++first)
      {
        destroy_at(addressof(*first));
      }
    }

  } // namespace v1
} // namespace rsl
