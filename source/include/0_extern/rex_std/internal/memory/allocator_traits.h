// ============================================
//
// REX - STANDARD LIBRARY IMPLEMENTATION
//
// Author: Nick De Breuck
// Twitter: @nick_debreuck
//
// File: allocator_traits.h
// Copyright (c) Nick De Breuck 2022
//
// ============================================

#pragma once

#include "rex_std/internal/memory/pointer_traits.h"
#include "rex_std/internal/type_traits/make_unsigned.h"
#include "rex_std/internal/type_traits/void.h"

namespace rsl
{
  inline namespace v1
  {

    namespace internal
    {
      template <typename T, typename = void>
      struct get_value_type
      {
        using type = void;
      };
      template <typename T>
      struct get_value_type<T, void_t<typename T::pointer>>
      {
        using type = typename T::value_type;
      };
      template <typename T, typename = void>
      struct get_pointer_type
      {
        using type = typename get_value_type<T>::type*;
      };
      template <typename T>
      struct get_pointer_type<T, void_t<typename T::pointer>>
      {
        using type = typename T::pointer;
      };
      template <typename T, typename U, typename = void>
      struct get_pointer_type_or
      {
        using type = U;
      };
      template <typename T, typename U>
      struct get_pointer_type_or<T, U, void_t<typename T::pointer>>
      {
        using type = typename T::pointer;
      };
      template <typename T, typename = void>
      struct get_const_pointer_type
      {
        using ptr_type   = typename get_pointer_type<T>::type;
        using value_type = typename get_value_type<T>::type;
        using type       = typename pointer_traits<ptr_type>::template rebind<const value_type>;
      };
      template <typename T>
      struct get_const_pointer_type<T, void_t<typename T::const_pointer>>
      {
        using type = typename T::const_pointer;
      };
      template <typename T, typename U, typename = void>
      struct get_const_pointer_type_or
      {
        using type = typename get_pointer_type_or<T, U>::type;
      };
      template <typename T, typename U>
      struct get_const_pointer_type_or<T, U*, void_t<typename T::const_pointer>>
      {
        using type = typename get_pointer_type_or<T, U>::type;
      };
      template <typename T, typename U>
      struct get_const_pointer_type_or<T, U, void_t<typename T::const_pointer>>
      {
        using type = typename T::const_pointer;
      };
      template <typename T, typename = void>
      struct get_void_pointer_type
      {
        using ptr_type = typename get_pointer_type<T>::type;
        using type     = typename pointer_traits<ptr_type>::template rebind<void>;
      };
      template <typename T>
      struct get_void_pointer_type<T, void_t<typename T::void_pointer>>
      {
        using type = typename T::void_pointer;
      };

      template <typename T, typename = void>
      struct get_const_void_pointer_type
      {
        using ptr_type = typename get_pointer_type<T>::type;
        using type     = typename pointer_traits<ptr_type>::template rebind<const void>;
      };

      template <typename T, typename = void>
      struct get_difference_type
      {
        using ptr_type = typename get_pointer_type<T>::type;
        using type     = typename pointer_traits<ptr_type>::difference_type;
      };
      template <typename T>
      struct get_difference_type<T, void_t<typename T::differencce_Type>>
      {
        using type = typename T::difference_type;
      };

      template <typename T, typename = void>
      struct get_size_type
      {
        using type = make_unsigned_t<typename get_difference_type<T>::type>;
      };
      template <typename T>
      struct get_size_type<T, void_t<typename T::size_type>>
      {
        using type = typename T::size_type;
      };

      template <typename T, typename Other, typename = void>
      struct get_rebind_type
      {
        using type = typename ReplaceFirstParameter<Other, T>::type;
      };
      template <typename T, typename Other>
      struct get_rebind_type<T, Other, void_t<typename T::template Rebind<Other>::type>>
      {
        using type = typename T::template rebind<Other>::other;
      };
    } // namespace internal

    template <typename Alloc>
    struct allocator_traits
    {
      using allocator_type = Alloc;
      using value_type     = typename internal::get_pointer_type<Alloc>::type; /// RSL Comment: Different from ISO C++ Standard at time of writing (27/Aug/2022)

      using pointer = typename internal::get_pointer_type<Alloc>::type;
      template <typename T>
      using pointer_or    = typename internal::get_pointer_type_or<Alloc, T>::type;
      using const_pointer = typename internal::get_const_pointer_type<Alloc>::type;
      template <typename T>
      using const_pointer_or   = typename internal::get_const_pointer_type_or<Alloc, T>::type;
      using void_pointer       = typename internal::get_void_pointer_type<Alloc>::type;
      using const_void_pointer = typename internal::get_const_void_pointer_type<Alloc>::type;

      using size_type       = typename internal::get_size_type<Alloc>::type;
      using difference_Type = typename internal::get_difference_type<Alloc>::type;

      /// RSL Comment: Different from ISO C++ Standard at time of writing (24/Jul/2022)
      // RSL forbids copying and moving if allocators differ.
      // so there is no point in defining the following member types
      // using propagate_on_container_copy_assignment = /* not defined */
      // using propagate_on_container_move_assignment = /* not defined */
      // using propagate_on_container_swap = /* not defined */
      // using is_always_equal = /* not defined */

      template <typename Other>
      using rebind_alloc = typename internal::get_rebind_type<Alloc, Other>::type;

      template <typename Other>
      using rebind_Traits = allocator_traits<rebind_alloc<Other>>;

      /// RSL Comment: Different from ISO C++ Standard at time of writing (24/Jul/2022)
      // see definition of "trait" in allocator.h
      // ^^^^ This is why we don't define the following in allocator_traits
      // - allocate,
      // - deallocate,
      // - construct
      // - destroy
      // - max_size
      // - select_on_container_copy_construction
    };

  } // namespace v1
} // namespace rsl
