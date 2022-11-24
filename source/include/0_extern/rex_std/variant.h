// ============================================
//
// REX - STANDARD LIBRARY IMPLEMENTATION
//
// Author: Nick De Breuck
// Twitter: @nick_debreuck
//
// File: variant.h
// Copyright (c) Nick De Breuck 2022
//
// ============================================

#pragma once

#include "rex_std/bonus/defines.h"
#include "rex_std/compare.h"
#include "rex_std/disable_std_checking.h"
#include "rex_std/std_alias_defines.h"

#include <variant>

namespace rsl
{
  inline namespace v1
  {

    REX_STD_TEMPLATED_CLASS_ALIAS(template <typename... Types>, variant, Types...);

    REX_STD_CLASS_ALIAS(monostate);

    REX_STD_TEMPLATED_CLASS_ALIAS(template <typename T>, variant_size, T);

    REX_STD_TEMPLATED_OBJECT_ALIAS(template <typename T>, variant_size_v, T);

    REX_STD_TEMPLATED_CLASS_ALIAS(template <size_t I, typename T>, variant_alternative, I, T);

    REX_STD_TEMPLATED_OBJECT_ALIAS(template <size_t I, typename T>, variant_alternative_t, I, T);

    template <typename T>
    struct hash;

    REX_STD_OBJECT_ALIAS(variant_npos);

    REX_STD_FUNC_ALIAS(visit);
    REX_STD_FUNC_ALIAS(holds_alternative);
    REX_STD_FUNC_ALIAS(get);
    REX_STD_FUNC_ALIAS(get_if);
    REX_STD_FUNC_ALIAS(operator==);
    REX_STD_FUNC_ALIAS(operator!=);
    REX_STD_FUNC_ALIAS(operator<);
    REX_STD_FUNC_ALIAS(operator<=);
    REX_STD_FUNC_ALIAS(operator>);
    REX_STD_FUNC_ALIAS(operator>=);

  } // namespace v1
} // namespace rsl

#include "rex_std/enable_std_checking.h"
