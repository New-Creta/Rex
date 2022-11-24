// ============================================
//
// REX - STANDARD LIBRARY IMPLEMENTATION
//
// Author: Nick De Breuck
// Twitter: @nick_debreuck
//
// File: add_pointer.h
// Copyright (c) Nick De Breuck 2022
//
// ============================================

#pragma once

#include "rex_std/internal/type_traits/remove_reference.h"
#include "rex_std/internal/type_traits/void.h"

namespace rsl
{
  inline namespace v1
  {

    namespace internal
    {
      template <typename T, typename = void>
      struct add_pointer_impl
      {
        using type = T;
      };
      template <typename T>
      struct add_pointer_impl<T, void_t<remove_reference_t<T>*>>
      {
        using type = remove_reference_t<T>*;
      };
    } // namespace internal

    template <typename T>
    struct add_pointer
    {
      using type = typename internal::add_pointer_impl<T>::type;
    };

    template <typename T>
    using add_pointer_t = typename add_pointer<T>::type;

  } // namespace v1
} // namespace rsl
