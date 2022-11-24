// ============================================
//
// REX - STANDARD LIBRARY IMPLEMENTATION
//
// Author: Nick De Breuck
// Twitter: @nick_debreuck
//
// File: return_type.h
// Copyright (c) Nick De Breuck 2022
//
// ============================================

#pragma once

#include "rex_std/internal/type_traits/common_type.h"
#include "rex_std/internal/type_traits/conditional.h"
#include "rex_std/internal/type_traits/is_integral.h"

namespace rsl
{
  inline namespace v1
  {

    template <typename T>
    using return_t = typename rsl::conditional<rsl::is_integral<T>::value, float64, T>::type;

    template <typename... T>
    using common_t = typename rsl::common_type<T...>::type;

    template <typename... T>
    using common_return_t = return_t<common_t<T...>>;

  } // namespace v1
} // namespace rsl
