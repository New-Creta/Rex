// ============================================
//
// REX - STANDARD LIBRARY IMPLEMENTATION
//
// Author: Nick De Breuck
// Twitter: @nick_debreuck
//
// File: common_type.h
// Copyright (c) Nick De Breuck 2022
//
// ============================================

#pragma once

#include "rex_std/internal/type_traits/decay.h"
#include "rex_std/internal/type_traits/void.h"
#include "rex_std/internal/utility/declval.h"

namespace rsl
{
  inline namespace v1
  {
    template <typename... T>
    struct common_type;

    template <typename T>
    struct common_type<T>
    {
      using type = decay_t<T>;
    };

    template <typename T, typename U>
    struct common_type<T, U>
    {
      using type = decay_t<decltype(true ? declval<T>() : declval<U>())>;
    };

    template <typename T, typename U, typename... V>
    struct common_type<T, U, V...>
    {
      using type = typename common_type<typename common_type<T, U>::type, V...>::type;
    };

    template <typename... T>
    using common_type_t = typename common_type<T...>::type;

  } // namespace v1
} // namespace rsl
