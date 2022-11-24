// ============================================
//
// REX - STANDARD LIBRARY IMPLEMENTATION
//
// Author: Nick De Breuck
// Twitter: @nick_debreuck
//
// File: stdfloat.h
// Copyright (c) Nick De Breuck 2022
//
// ============================================

#pragma once

#ifdef REX_ENABLE_WITH_CPP23

  #include "rex_std/disable_std_checking.h"
  #include "rex_std/std_alias_defines.h"

  #include <stdfloat>

namespace rsl
{
  inline namespace v1
  {

    REX_STD_CLASS_ALIAS(float16_t);
    REX_STD_CLASS_ALIAS(float32_t);
    REX_STD_CLASS_ALIAS(float64_t);
    REX_STD_CLASS_ALIAS(float128_t);
    REX_STD_CLASS_ALIAS(bfloat16_t);

  } // namespace v1
} // namespace rsl

  #include "rex_std/enable_std_checking.h"

#endif