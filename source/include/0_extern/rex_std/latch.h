// ============================================
//
// REX - STANDARD LIBRARY IMPLEMENTATION
//
// Author: Nick De Breuck
// Twitter: @nick_debreuck
//
// File: latch.h
// Copyright (c) Nick De Breuck 2022
//
// ============================================

#pragma once

#ifdef REX_ENABLE_WITH_CPP20

  #include "rex_std/disable_std_checking.h"
  #include "rex_std/std_alias_defines.h"

  #include <latch>

namespace rsl
{
  inline namespace v1
  {

    REX_STD_CLASS_ALIAS(latch);

  }
} // namespace rsl

  #include "rex_std/enable_std_checking.h"

#endif // REX_ENABLE_WITH_CPP20
