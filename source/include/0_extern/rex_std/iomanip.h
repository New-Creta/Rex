// ============================================
//
// REX - STANDARD LIBRARY IMPLEMENTATION
//
// Author: Nick De Breuck
// Twitter: @nick_debreuck
//
// File: iomanip.h
// Copyright (c) Nick De Breuck 2022
//
// ============================================

#pragma once

#include "rex_std/bonus/defines.h"
#include "rex_std/disable_std_checking.h"
#include "rex_std/std_alias_defines.h"

#include <iomanip>

namespace rsl
{
  inline namespace v1
  {

    REX_STD_FUNC_ALIAS(resetiosflags);
    REX_STD_FUNC_ALIAS(setiosflags);
    REX_STD_FUNC_ALIAS(setbase);
    REX_STD_FUNC_ALIAS(setfill);
    REX_STD_FUNC_ALIAS(setprecision);
    REX_STD_FUNC_ALIAS(setw);
    REX_STD_FUNC_ALIAS(get_money);
    REX_STD_FUNC_ALIAS(put_money);
    REX_STD_FUNC_ALIAS(get_time);
    REX_STD_FUNC_ALIAS(put_time);
    REX_STD_FUNC_ALIAS(quoted);

  } // namespace v1
} // namespace rsl

#include "rex_std/enable_std_checking.h"
