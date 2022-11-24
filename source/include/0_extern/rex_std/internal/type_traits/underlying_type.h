// ============================================
//
// REX - STANDARD LIBRARY IMPLEMENTATION
//
// Author: Nick De Breuck
// Twitter: @nick_debreuck
//
// File: underlying_type.h
// Copyright (c) Nick De Breuck 2022
//
// ============================================

#pragma once

#include "rex_std/internal/type_traits/is_enum.h"

namespace rsl
{
  inline namespace v1
  {

    namespace internal
    {
      template <typename T, bool = is_enum_v<T>>
      struct underlying_type_helper
      {
        using type = __underlying_type(T);
      };

      template <typename T>
      struct underlying_type_helper<T, false>
      {
      };
    } // namespace internal
    template <typename T>
    struct underlying_type : internal::underlying_type_helper<T>
    {
    };

    template <typename T>
    using underlying_type_t = typename underlying_type<T>::type;

  } // namespace v1
} // namespace rsl
