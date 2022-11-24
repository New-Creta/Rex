// ============================================
//
// REX - STANDARD LIBRARY IMPLEMENTATION
//
// Author: Nick De Breuck
// Twitter: @nick_debreuck
//
// File: expected.h
// Copyright (c) Nick De Breuck 2022
//
// ============================================

#pragma once

#ifdef REX_ENABLE_WITH_CPP23

  #include "rex_std/disable_std_checking.h"
  #include "rex_std/std_alias_defines.h"

  #include <expected>

namespace rsl
{
  inline namespace v1
  {

    REX_STD_TEMPLATED_CLASS_ALIAS(expected, typename T, typename E);
    REX_STD_TEMPLATED_CLASS_ALIAS(unexpected, typename E);

    REX_STD_CLASS_ALIAS(unexpect_t);
    REX_STD_OBJECT_ALIAS(unexpect);

  } // namespace v1
} // namespace rsl

  #include "rex_std/enable_std_checking.h"

#endif
