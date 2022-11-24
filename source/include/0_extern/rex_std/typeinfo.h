// ============================================
//
// REX - STANDARD LIBRARY IMPLEMENTATION
//
// Author: Nick De Breuck
// Twitter: @nick_debreuck
//
// File: typeinfo.h
// Copyright (c) Nick De Breuck 2022
//
// ============================================

#pragma once

#include "rex_std/bonus/defines.h"
#include "rex_std/disable_std_checking.h"
#include "rex_std/std_alias_defines.h"

#include <typeinfo>

namespace rsl
{
  inline namespace v1
  {

    REX_STD_CLASS_ALIAS(type_info);

  }
} // namespace rsl

#include "rex_std/enable_std_checking.h"
