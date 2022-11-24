// ============================================
//
// REX - STANDARD LIBRARY IMPLEMENTATION
//
// Author: Nick De Breuck
// Twitter: @nick_debreuck
//
// File: barrier.h
// Copyright (c) Nick De Breuck 2022
//
// ============================================

#pragma once

#ifdef REX_ENABLE_WITH_CPP20

  #include "rex_std/disable_std_checking.h"
  #include "rex_std/std_alias_defines.h"

  #include <barrier>

namespace rsl
{
  inline namespace v1
  {
    {
      REX_STD_TEMPLATED_CLASS_ALIAS(template <typename CompletionFunction>, barrier, CompletionFunction);
    }

  #include "rex_std/enable_std_checking.h"

#endif // REX_ENABLE_WITH_CPP20