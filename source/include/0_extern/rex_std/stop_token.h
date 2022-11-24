// ============================================
//
// REX - STANDARD LIBRARY IMPLEMENTATION
//
// Author: Nick De Breuck
// Twitter: @nick_debreuck
//
// File: stop_token.h
// Copyright (c) Nick De Breuck 2022
//
// ============================================

#pragma once

#ifdef REX_ENABLE_WITH_CPP20

  #include "rex_std/disable_std_checking.h"
  #include "rex_std/std_alias_defines.h"

  #include <stop_token>

namespace rsl
{
  inline namespace v1
  {

    REX_STD_CLASS_ALIAS(stop_token);
    REX_STD_CLASS_ALIAS(stop_source);

    REX_STD_TEMPLATED_CLASS_ALIAS(template <typename Callback>, stop_callback, Callback);

    REX_STD_CLASS_ALIAS(nostopstate_t);

    REX_STD_OBJECT_ALIAS(nostopstate);

  } // namespace v1
} // namespace rsl

  #include "rex_std/enable_std_checking.h"

#endif // REX_ENABLE_WITH_CPP20
