// ============================================
//
// REX - STANDARD LIBRARY IMPLEMENTATION
//
// Author: Nick De Breuck
// Twitter: @nick_debreuck
//
// File: pointer_traits.h
// Copyright (c) Nick De Breuck 2022
//
// ============================================

#pragma once

#include "rex_std/internal/memory/addressof.h"
#include "rex_std/internal/type_traits/conditional.h"
#include "rex_std/internal/type_traits/is_void.h"
#include "rex_std/internal/type_traits/void.h"

namespace rsl
{
  inline namespace v1
  {

    namespace internal
    {
      template <typename T>
      struct GetFirstParameter;

      template <template <typename, typename...> typename T, typename First, typename... Rest>
      struct GetFirstParameter<T<First, Rest...>>
      {
        using type = First;
      };

      template <typename T, typename = void>
      struct get_element_type
      {
        using type = typename GetFirstParameter<T>::type;
      };
      template <typename T>
      struct get_element_type<T, void_t<typename T::element_type>>
      {
        using type = typename T::element_type;
      };

      template <typename T, typename = void>
      struct get_ptr_difference_type
      {
        using type = ptrdiff;
      };
      template <typename T>
      struct get_ptr_difference_type<T, void_t<typename T::difference_type>>
      {
        using type = typename T::difference_type;
      };

      template <typename NewFirst, typename T>
      struct ReplaceFirstParameter;

      template <typename NewFirst, template <typename, typename...> typename T, typename First, typename... Rest>
      struct ReplaceFirstParameter<NewFirst, T<First, Rest...>>
      {
        using type = T<NewFirst, Rest...>;
      };

      template <typename T, typename Other, typename = void>
      struct get_rebind_alias
      {
        using type = typename ReplaceFirstParameter<Other, T>::type;
      };
      template <typename T, typename Other>
      struct get_rebind_alias<T, Other, void_t<typename T::template rebind<Other>>>
      {
        using type = typename T::template rebind<Other>;
      };
    }; // namespace internal

    template <typename T>
    struct pointer_traits
    {
      using pointer         = T;
      using element_type    = typename internal::get_element_type<T>::type;
      using difference_type = typename internal::get_ptr_difference_type<T>::type;

      template <typename Other>
      using rebind = typename internal::get_rebind_alias<T, Other>::type;

      using ref_type = conditional_t<is_void_v<element_type>, char, element_type>&;

      REX_NO_DISCARD static pointer pointer_to(ref_type val)
      {
        return T::pointer_to(val);
      }
    };
    template <typename T>
    struct pointer_traits<T*>
    {
      using pointer         = T*;
      using element_type    = T;
      using difference_type = ptrdiff;

      template <typename Other>
      using rebind = Other*;

      using ref_type = conditional_t<is_void_v<T>, char, T>&;

      REX_NO_DISCARD constexpr pointer pointer_to(ref_type val)
      {
        return rsl::addressof(val);
      }
    };

  } // namespace v1
} // namespace rsl
