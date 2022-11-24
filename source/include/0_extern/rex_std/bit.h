// ============================================
//
// REX - STANDARD LIBRARY IMPLEMENTATION
//
// Author: Nick De Breuck
// Twitter: @nick_debreuck
//
// File: bit.h
// Copyright (c) Nick De Breuck 2022
//
// ============================================

#pragma once

#include "rex_std/disable_std_checking.h"
#include "rex_std/std_alias_defines.h"

#include <bit>

namespace rsl
{
  inline namespace v1
  {

    REX_STD_CLASS_ALIAS(endian);

    REX_STD_FUNC_ALIAS(bit_cast);
    REX_STD_FUNC_ALIAS(has_single_bit);
    REX_STD_FUNC_ALIAS(bit_ceil);
    REX_STD_FUNC_ALIAS(bit_floor);
    REX_STD_FUNC_ALIAS(bit_width);
    REX_STD_FUNC_ALIAS(rotl);
    REX_STD_FUNC_ALIAS(rotr);
    REX_STD_FUNC_ALIAS(countl_zero);
    REX_STD_FUNC_ALIAS(countl_one);
    REX_STD_FUNC_ALIAS(countr_zero);
    REX_STD_FUNC_ALIAS(countr_one);
    REX_STD_FUNC_ALIAS(popcount);

#ifdef REX_ENABLE_WITH_CPP23
    REX_STD_FUNC_ALIAS(byteswap);
#endif // REX_ENABLE_WITH_CPP23

  } // namespace v1
} // namespace rsl

#include "rex_std/enable_std_checking.h"
