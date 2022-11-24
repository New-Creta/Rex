// ============================================
//
// REX - STANDARD LIBRARY IMPLEMENTATION
//
// Author: Nick De Breuck
// Twitter: @nick_debreuck
//
// File: destroy_at.h
// Copyright (c) Nick De Breuck 2022
//
// ============================================

#pragma once

#include "rex_std/internal/memory/addressof.h"
#include "rex_std/internal/type_traits/is_array.h"

namespace rsl
{
  inline namespace v1
  {

    template <typename T>
    constexpr void destroy_at(T* p)
    {
      if constexpr(rsl::is_array_v<T>)
      {
        for(auto& elem: *p)
        {
          rsl::destroy_at(rsl::addressof(elem));
        }
      }
      else
      {
        p->~T();
      }
    }

  } // namespace v1
} // namespace rsl
