// ============================================
//
// REX - STANDARD LIBRARY IMPLEMENTATION
//
// Author: Nick De Breuck
// Twitter: @nick_debreuck
//
// File: remove_reference_wrapper.h
// Copyright (c) Nick De Breuck 2022
//
// ============================================

#include "rex_std/internal/functional/reference_wrapper.h"

namespace rsl
{
  inline namespace v1
  {

    template <typename T>
    struct remove_reference_wrapper
    {
      using type = T;
    };
    template <typename T>
    struct remove_reference_wrapper<reference_wrapper<T>>
    {
      using type = T&;
    };
    template <typename T>
    struct remove_reference_wrapper<const reference_wrapper<T>>
    {
      using type = T&;
    };

  } // namespace v1
} // namespace rsl
