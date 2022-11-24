// ============================================
//
// REX - STANDARD LIBRARY IMPLEMENTATION
//
// Author: Nick De Breuck
// Twitter: @nick_debreuck
//
// File: cmp.h
// Copyright (c) Nick De Breuck 2022
//
// ============================================

#pragma once

#include "rex_std/internal/type_traits/is_signed.h"
#include "rex_std/internal/type_traits/make_unsigned.h"

namespace rsl
{
  inline namespace v1
  {

    template <typename T, typename U>
    constexpr bool cmp_equal(T t, U u)
    {
      using UT = make_unsigned_t<T>;
      using UU = make_unsigned_t<U>;

      if constexpr(is_signed_v<T> == is_signed_v<U>)
      {
        return t == u;
      }
      else if constexpr(is_signed_v<T>)
      {
        return t < 0 ? false : static_cast<UT>(t) == u;
      }
      else
      {
        return u < 0 ? false : t == static_cast<UU>(u);
      }
    }

    template <typename T, typename U>
    constexpr bool cmp_not_equal(T t, U u)
    {
      return !cmp_equal(t, u);
    }

    template <typename T, typename U>
    constexpr bool cmp_less(T t, U u)
    {
      using UT = make_unsigned_t<T>;
      using UU = make_unsigned_t<U>;

      if constexpr(is_signed_v<T> == is_signed_v<U>)
      {
        return t < u;
      }
      else if constexpr(is_signed_v<T>)
      {
        return t < 0 ? true : static_cast<UT>(t) < u;
      }
      else
      {
        return u < 0 ? false : t < static_cast<UU>(u);
      }
    }

    template <typename T, typename U>
    constexpr bool cmp_greater(T t, U u)
    {
      return cmp_less(u, t);
    }
    template <typename T, typename U>
    constexpr bool cmp_less_equal(T t, U u)
    {
      return !cmp_greater(t, u);
    }
    template <typename T, typename U>
    constexpr bool cmp_greater_equal(T t, U u)
    {
      return !cmp_less(t, u);
    }

  } // namespace v1
} // namespace rsl
