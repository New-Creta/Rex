// ============================================
//
// REX - STANDARD LIBRARY IMPLEMENTATION
//
// Author: Nick De Breuck
// Twitter: @nick_debreuck
//
// File: make_signed.h
// Copyright (c) Nick De Breuck 2022
//
// ============================================

#pragma once

#include "rex_std/bonus/types.h"
#include "rex_std/internal/type_traits/is_enum.h"
#include "rex_std/internal/type_traits/is_integral.h"

namespace rsl
{
  inline namespace v1
  {

    namespace internal
    {
      template <typename T, bool = is_enum_v<T> || is_integral_v<T>>
      struct make_signed_helper0
      {
        struct char_helper
        {
          using type = int8;
        };

        struct short_helper
        {
          using type = int16;
        };

        struct int_helper
        {
          using type = int32;
        };

        struct long_helper
        {
          using type = long;
        };

        struct long_long_helper
        {
          using type = int64;
        };

        struct no_type_helper
        {
        };

        using type =
            typename conditional_t<sizeof(T) <= sizeof(int8), char_helper,
                                   conditional_t<sizeof(T) <= sizeof(int16), short_helper,
                                                 conditional_t<sizeof(T) <= sizeof(int32), int_helper, conditional_t<sizeof(T) <= sizeof(long), long_helper, conditional_t<sizeof(T) <= sizeof(int64), long_long_helper, no_type_helper>>>>>::type;
      };

      template <typename T>
      struct make_signed_helper0<T, false>
      {
        struct no_type_helper
        {
        };

        using type = no_type_helper;
      };

      template <typename T>
      struct make_signed_helper1
      {
        using type = typename T::type;
      };

      template <typename T>
      struct make_signed_helper
      {
        using type = typename make_signed_helper1<typename make_signed_helper0<T>::type>::type;
      };
    } // namespace internal

    template <typename T>
    struct make_signed
    {
      using type = typename internal::make_signed_helper<T>::type;
    };

    template <>
    struct make_signed<bool>
    {
    };
    template <>
    struct make_signed<int8>
    {
      using type = int8;
    };
    template <>
    struct make_signed<uint8>
    {
      using type = int8;
    };
    template <>
    struct make_signed<int16>
    {
      using type = int16;
    };
    template <>
    struct make_signed<uint16>
    {
      using type = int16;
    };
    template <>
    struct make_signed<int32>
    {
      using type = int32;
    };
    template <>
    struct make_signed<uint32>
    {
      using type = int32;
    };
    template <>
    struct make_signed<int64>
    {
      using type = int64;
    };
    template <>
    struct make_signed<uint64>
    {
      using type = int64;
    };
    template <>
    struct make_signed<long>
    {
      using type = long;
    };
    template <>
    struct make_signed<ulong>
    {
      using type = long;
    };

    template <>
    struct make_signed<char>
    {
      using type = int8;
    };
#ifdef REX_ENABLE_WITH_CPP20
    template <>
    struct make_signed<char8_t>
    {
      using type = int8;
    };
#endif

    template <typename T>
    struct make_signed<const T>
    {
      using add_const_t<typename make_signed<T>::type>::type;
    };
    template <typename T>
    struct make_signed<volatile T>
    {
      using add_volatile_t<typename make_signed<T>::type>::type;
    };
    template <typename T>
    struct make_signed<const volatile T>
    {
      using add_cv<typename make_signed<T>::type>::type;
    };

    template <typename T>
    using make_signed_t = typename make_signed<T>::type;

  } // namespace v1
} // namespace rsl
