// ============================================
//
// REX - STANDARD LIBRARY IMPLEMENTATION
//
// Author: Nick De Breuck
// Twitter: @nick_debreuck
//
// File: cuchar.h
// Copyright (c) Nick De Breuck 2022
//
// ============================================

#pragma once

#include "rex_std/disable_std_checking.h"
#include "rex_std/std_alias_defines.h"

#include <cuchar>

namespace rsl
{
  inline namespace v1
  {

    REX_STD_CLASS_ALIAS(mbstate_t);
    REX_STD_CLASS_ALIAS(size_t);

    REX_STD_FUNC_ALIAS(mbrtoc16);
    REX_STD_FUNC_ALIAS(c16rtomb);
    REX_STD_FUNC_ALIAS(mbrtoc32);
    REX_STD_FUNC_ALIAS(c32rtomb);

#ifdef REX_ENABLE_WITH_CPP20
    REX_STD_FUNC_ALIAS(mbrtoc8);
    REX_STD_FUNC_ALIAS(c8rtomb);
#endif // REX_ENABLE_WITH_CPP20

  } // namespace v1
} // namespace rsl

#include "rex_std/enable_std_checking.h"
