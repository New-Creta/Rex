// ============================================
//
// REX - STANDARD LIBRARY IMPLEMENTATION
//
// Author: Nick De Breuck
// Twitter: @nick_debreuck
//
// File: call_traits.h
// Copyright (c) Nick De Breuck 2022
//
// ============================================

#pragma once

#include "rex_std/internal/type_traits/is_arithmetic.h"
#include "rex_std/internal/type_traits/is_pointer.h"

namespace rsl
{
  inline namespace v1
  {

    namespace internal
    {
      template <typename T, bool IsSmall>
      struct call_traits_impl2
      {
        using param_type = const T&;
      };

      template <typename T>
      struct call_traits_impl2<T, true>
      {
        using param_type = T;
      };

      template <typename T, bool IsPointerV, bool B1>
      struct call_traits_impl
      {
        using param_type = const T&;
      };

      template <typename T, bool IsPointerV>
      struct call_traits_impl<T, IsPointerV, true>
      {
        using param_type = typename call_traits_impl2<T, sizeof(T) <= sizeof(void*)>::param_type;
      };

      template <typename T, bool B1>
      struct call_traits_impl<T, true, B1>
      {
        using param_type = const T;
      };
    } // namespace internal

    template <typename T>
    struct call_traits
    {
    public:
      using value_type      = T;
      using reference       = T&;
      using const_reference = const T&;
      using param_type      = typename internal::call_traits_impl<T, is_pointer_v<T>, is_arithmetic_v<T>>::param_type;
    };

    template <typename T>
    struct call_traits<T&>
    {
    public:
      using value_type      = T&;
      using reference       = T&;
      using const_reference = const T&;
      using param_type      = T&;
    };

    template <typename T, card32 N>
    struct call_traits<T[N]> // NOLINT(modernize-avoid-c-arrays)
    {
    private:
      using array_type = T[N]; // NOLINT(modernize-avoid-c-arrays)

    public:
      using value_type      = const T*;
      using reference       = array_type&;
      using const_reference = const array_type&;
      using param_type      = const T* const;
    };

    template <typename T, card32 N>
    struct call_traits<const T[N]> // NOLINT(modernize-avoid-c-arrays)
    {
    private:
      using array_type = const T[N]; // NOLINT(modernize-avoid-c-arrays)

    public:
      using value_type      = const T*;
      using reference       = array_type&;
      using const_reference = const array_type&;
      using param_type      = const T* const;
    };

  } // namespace v1
} // namespace rsl
