// ============================================
//
// REX - STANDARD LIBRARY IMPLEMENTATION
//
// Author: Nick De Breuck
// Twitter: @nick_debreuck
//
// File: add_reference.h
// Copyright (c) Nick De Breuck 2022
//
// ============================================

#pragma once

#include "rex_std/internal/type_traits/void.h"

namespace rsl
{
  inline namespace v1
  {

    namespace bonus
    {
      // add reference (non-referenceable type)
      template <typename T, typename = void>
      struct add_reference
      {
        using lvalue = T;
        using rvalue = T;
      };

      // (referenceable type)
      template <typename T>
      struct add_reference<T, void_t<T&>>
      {
        using lvalue = T&;
        using rvalue = T&&;
      };
    } // namespace bonus

  } // namespace v1
} // namespace rsl
