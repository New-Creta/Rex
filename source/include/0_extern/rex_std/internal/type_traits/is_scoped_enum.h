// ============================================
//
// REX - STANDARD LIBRARY IMPLEMENTATION
//
// Author: Nick De Breuck
// Twitter: @nick_debreuck
//
// File: is_scoped_enum.h
// Copyright (c) Nick De Breuck 2022
//
// ============================================

#include "rex_std/internal/type_traits/integral_constant.h"

#ifdef REX_ENABLE_WITH_CPP23

namespace rsl
{
  inline namespace v1
  {

    template <typename T>
    struct is_scoped_enum : public bool_constant<__is_scoped_enum(T)>
    {
    };

    template <typename T>
    inline constexpr bool is_scoped_enum_v = is_scoped_enum<T>::value;

  } // namespace v1
} // namespace rsl

#endif