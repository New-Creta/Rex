// ============================================
//
// REX - STANDARD LIBRARY IMPLEMENTATION
//
// Author: Nick De Breuck
// Twitter: @nick_debreuck
//
// File: fenv.h
// Copyright (c) Nick De Breuck 2022
//
// ============================================

#pragma once

#include "rex_std/bonus/defines.h"
#include "rex_std/disable_std_checking.h"
#include "rex_std/std_alias_defines.h"

#include <cfenv>

namespace rsl
{
  inline namespace v1
  {

    REX_STD_CLASS_ALIAS(fenv_t);
    REX_STD_CLASS_ALIAS(fexcept_t);

    REX_STD_FUNC_ALIAS(feclearexcept);
    REX_STD_FUNC_ALIAS(fetestexcept);
    REX_STD_FUNC_ALIAS(feraiseexcept);
    REX_STD_FUNC_ALIAS(fegetexceptflag);
    REX_STD_FUNC_ALIAS(fesetexceptflag);
    REX_STD_FUNC_ALIAS(fegetround);
    REX_STD_FUNC_ALIAS(fesetround);
    REX_STD_FUNC_ALIAS(fegetenv);
    REX_STD_FUNC_ALIAS(fesetenv);
    REX_STD_FUNC_ALIAS(feholdexcept);
    REX_STD_FUNC_ALIAS(feupdateenv);

  } // namespace v1
} // namespace rsl

#include "rex_std/enable_std_checking.h"
