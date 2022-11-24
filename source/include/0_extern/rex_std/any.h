// ============================================
//
// REX - STANDARD LIBRARY IMPLEMENTATION
//
// Author: Nick De Breuck
// Twitter: @nick_debreuck
//
// File: any.h
// Copyright (c) Nick De Breuck 2022
//
// ============================================

#pragma once

#include "rex_std/disable_std_checking.h"
#include "rex_std/std_alias_defines.h"

#include <any>

namespace rsl
{
  inline namespace v1
  {

    REX_STD_CLASS_ALIAS(any);

    ////REX_STD_FUNC_ALIAS(swap); // Doesn't work because rsl has its own swap // Doesn't work because rsl has its own swap
    REX_STD_FUNC_ALIAS(make_any);
    REX_STD_FUNC_ALIAS(any_cast);

  } // namespace v1
} // namespace rsl

#include "rex_std/enable_std_checking.h"
