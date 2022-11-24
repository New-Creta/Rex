// ============================================
//
// REX - STANDARD LIBRARY IMPLEMENTATION
//
// Author: Nick De Breuck
// Twitter: @nick_debreuck
//
// File: compare.h
// Copyright (c) Nick De Breuck 2022
//
// ============================================

#pragma once

#ifdef REX_ENABLE_WITH_CPP20

  #include "rex_std/disable_std_checking.h"
  #include "rex_std/std_alias_defines.h"

  #include <compare>

namespace rsl
{
  inline namespace v1
  {

    REX_STD_CLASS_ALIAS(partial_ordering);

    template <typename T, typename Cat = rsl::partial_ordering>
    using three_way_comparable = rsl::three_way_comparable<T, Cat>;
    template <typename T, typename U, typename Cat = rsl::partial_ordering>
    using three_way_comparable_with = rsl::three_way_comparable_with<T, U, Cat>;

    REX_STD_CLASS_ALIAS(weak_ordering);
    REX_STD_CLASS_ALIAS(strong_ordering);

    REX_STD_CLASS_ALIAS(common_comparison_category);
    REX_STD_CLASS_ALIAS(compare_three_way_result);
    REX_STD_CLASS_ALIAS(compare_three_way);

    REX_STD_OBJECT_ALIAS(strong_order);
    REX_STD_OBJECT_ALIAS(weak_order);
    REX_STD_OBJECT_ALIAS(partial_order);
    REX_STD_OBJECT_ALIAS(compare_strong_order_fallback);
    REX_STD_OBJECT_ALIAS(compare_weak_order_fallback);
    REX_STD_OBJECT_ALIAS(compare_partial_order_fallback);

    REX_STD_FUNC_ALIAS(is_eq);
    REX_STD_FUNC_ALIAS(is_neq);
    REX_STD_FUNC_ALIAS(is_lt);
    REX_STD_FUNC_ALIAS(is_lteq);
    REX_STD_FUNC_ALIAS(is_gt);
    REX_STD_FUNC_ALIAS(is_gteq);

  } // namespace v1
} // namespace rsl

  #include "rex_std/enable_std_checking.h"

#endif // REX_ENABLE_WITH_CPP20
