// ============================================
//
// REX - STANDARD LIBRARY IMPLEMENTATION
//
// Author: Nick De Breuck
// Twitter: @nick_debreuck
//
// File: make_unsigned.h
// Copyright (c) Nick De Breuck 2022
//
// ============================================

#pragma once

#include "rex_std/bonus/types.h"
#include "rex_std/internal/type_traits/add_const.h"
#include "rex_std/internal/type_traits/add_cv.h"
#include "rex_std/internal/type_traits/add_volatile.h"
#include "rex_std/internal/type_traits/is_enum.h"
#include "rex_std/internal/type_traits/is_integral.h"

namespace rsl
{
  inline namespace v1
  {

    namespace internal
    {
      template <typename T, bool = is_enum_v<T> || is_integral_v<T>>
      struct make_unsigned_helper0
      {
        struct char_helper
        {
          using type = uint8;
        };

        struct short_helper
        {
          using type = uint16;
        };

        struct int_helper
        {
          using type = uint32;
        };

        struct long_helper
        {
          using type = ulong;
        };

        struct long_long_helper
        {
          using type = uint64;
        };

        struct no_type_helper
        {
        };

        using type =
            typename conditional_t<sizeof(T) <= sizeof(uint8), char_helper,
                                   conditional_t<sizeof(T) <= sizeof(uint16), short_helper,
                                                 conditional_t<sizeof(T) <= sizeof(uint32), int_helper, conditional_t<sizeof(T) <= sizeof(uint64), long_long_helper, conditional_t<sizeof(T) <= sizeof(ulong), long_helper, no_type_helper>>>>>::type;
      };

      template <typename T>
      struct make_unsigned_helper0<T, false>
      {
        struct no_type_helper
        {
        };

        using type = no_type_helper;
      };

      template <typename T>
      struct make_unsigned_helper1
      {
        using type = typename T::type;
      };

      template <typename T>
      struct make_unsigned_helper
      {
        using type = typename make_unsigned_helper1<typename make_unsigned_helper0<T>::type>::type;
      };
    } // namespace internal

    template <typename T>
    struct make_unsigned
    {
      using type = typename internal::make_unsigned_helper<T>::type;
    };

    template <>
    struct make_unsigned<bool>
    {
    };
    template <>
    struct make_unsigned<int8>
    {
      using type = uint8;
    };
    template <>
    struct make_unsigned<uint8>
    {
      using type = uint8;
    };
    template <>
    struct make_unsigned<int16>
    {
      using type = uint16;
    };
    template <>
    struct make_unsigned<uint16>
    {
      using type = uint16;
    };
    template <>
    struct make_unsigned<int32>
    {
      using type = uint32;
    };
    template <>
    struct make_unsigned<uint32>
    {
      using type = uint32;
    };
    template <>
    struct make_unsigned<int64>
    {
      using type = uint64;
    };
    template <>
    struct make_unsigned<uint64>
    {
      using type = uint64;
    };
    template <>
    struct make_unsigned<long>
    {
      using type = ulong;
    };
    template <>
    struct make_unsigned<ulong>
    {
      using type = ulong;
    };

    template <>
    struct make_unsigned<char>
    {
      using type = uint8;
    };
#ifdef REX_ENABLE_WITH_CPP20
    template <>
    struct make_unsigned<char8_t>
    {
      using type = uint8;
    };
#endif
    template <typename T>
    struct make_unsigned<const T>
    {
      using type = add_const_t<typename make_unsigned<T>::type>;
    };
    template <typename T>
    struct make_unsigned<volatile T>
    {
      using type = add_volatile_t<typename make_unsigned<T>::type>;
    };
    template <typename T>
    struct make_unsigned<const volatile T>
    {
      using type = add_cv<typename make_unsigned<T>::type>;
    };

    template <typename T>
    using make_unsigned_t = typename make_unsigned<T>::type;

  } // namespace v1
} // namespace rsl
