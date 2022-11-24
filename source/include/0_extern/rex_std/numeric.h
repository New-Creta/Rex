// ============================================
//
// REX - STANDARD LIBRARY IMPLEMENTATION
//
// Author: Nick De Breuck
// Twitter: @nick_debreuck
//
// File: numeric.h
// Copyright (c) Nick De Breuck 2022
//
// ============================================

#pragma once

#include "rex_std/bonus/defines.h"
#include "rex_std/disable_std_checking.h"
#include "rex_std/std_alias_defines.h"

#include <numeric>

namespace rsl
{
  inline namespace v1
  {

    REX_STD_FUNC_ALIAS(iota);
    REX_STD_FUNC_ALIAS(accumulate);
    REX_STD_FUNC_ALIAS(reduce);
    REX_STD_FUNC_ALIAS(transform_reduce);
    REX_STD_FUNC_ALIAS(inner_product);
    REX_STD_FUNC_ALIAS(adjacent_difference);
    REX_STD_FUNC_ALIAS(partial_sum);
    REX_STD_FUNC_ALIAS(inclusive_scan);
    REX_STD_FUNC_ALIAS(exclusive_scan);
    REX_STD_FUNC_ALIAS(transform_inclusive_scan);
    REX_STD_FUNC_ALIAS(transform_exclusive_scan);
    REX_STD_FUNC_ALIAS(gcd);
    REX_STD_FUNC_ALIAS(lcm);
    REX_STD_FUNC_ALIAS(midpoint);

  } // namespace v1
} // namespace rsl

#include "rex_std/enable_std_checking.h"