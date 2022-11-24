// ============================================
//
// REX - STANDARD LIBRARY IMPLEMENTATION
//
// Author: Nick De Breuck
// Twitter: @nick_debreuck
//
// File: is_swappable_utils.h
// Copyright (c) Nick De Breuck 2022
//
// ============================================

#pragma once

#include "rex_std/bonus/type_traits/unused.h"

namespace rsl
{
  inline namespace v1
  {

    namespace internal
    {
      struct argument_sink
      {
        template <typename... Args>
        explicit argument_sink(Args&&... /*unused*/)
        {
        }
      };

      internal::unused swap(argument_sink, argument_sink);

    } // namespace internal

  } // namespace v1
} // namespace rsl
