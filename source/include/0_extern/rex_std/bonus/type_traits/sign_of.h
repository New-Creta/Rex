// ============================================
//
// REX - STANDARD LIBRARY IMPLEMENTATION
//
// Author: Nick De Breuck
// Twitter: @nick_debreuck
//
// File: sign_of
// Copyright (c) Nick De Breuck 2022
//
// ============================================

#include "rex_std/bonus/types.h"
#include "rex_std/internal/type_traits/integral_constant.h"

namespace rsl
{
  inline namespace v1
  {

    template <intmax Val>
    struct sign_of : integral_constant<intmax, (Val < 0 ? -1 : 1)>
    {
    };

    template <intmax Val>
    constexpr intmax sign_of_v = sign_of<Val>::value;

  } // namespace v1
} // namespace rsl
