// ============================================
//
// REX - STANDARD LIBRARY IMPLEMENTATION
//
// Author: Nick De Breuck
// Twitter: @nick_debreuck
//
// File: bitset.h
// Copyright (c) Nick De Breuck 2022
//
// ============================================

#pragma once

#include "rex_std/disable_std_checking.h"
#include "rex_std/iosfwd.h"
#include "rex_std/std_alias_defines.h"
#include "rex_std/string.h"

#include <bitset>

namespace rsl
{
  inline namespace v1
  {

    REX_STD_TEMPLATED_CLASS_ALIAS(template <size_t N>, bitset, N);

    REX_STD_FUNC_ALIAS(operator&);
    REX_STD_FUNC_ALIAS(operator|);
    REX_STD_FUNC_ALIAS(operator^);

    REX_STD_FUNC_ALIAS(operator<<);
    REX_STD_FUNC_ALIAS(operator>>);

  } // namespace v1
} // namespace rsl

#include "rex_std/enable_std_checking.h"
