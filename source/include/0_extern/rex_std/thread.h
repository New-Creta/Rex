// ============================================
//
// REX - STANDARD LIBRARY IMPLEMENTATION
//
// Author: Nick De Breuck
// Twitter: @nick_debreuck
//
// File: thread.h
// Copyright (c) Nick De Breuck 2022
//
// ============================================

#pragma once

#include "rex_std/bonus/defines.h"
#include "rex_std/disable_std_checking.h"
#include "rex_std/std_alias_defines.h"

#include <thread>

namespace rsl
{
  inline namespace v1
  {

    namespace this_thread
    {
      using namespace std::this_thread;
    }

    REX_STD_CLASS_ALIAS(thread);
    REX_STD_CLASS_ALIAS(jthread);

    // REX_STD_FUNC_ALIAS(swap); // Doesn't work because rsl has its own swap

    REX_STD_FUNC_ALIAS(operator==);
    REX_STD_FUNC_ALIAS(operator!=);
    REX_STD_FUNC_ALIAS(operator<);
    REX_STD_FUNC_ALIAS(operator<=);
    REX_STD_FUNC_ALIAS(operator>);
    REX_STD_FUNC_ALIAS(operator>=);

    REX_STD_FUNC_ALIAS(operator<<);

  } // namespace v1
} // namespace rsl

#include "rex_std/enable_std_checking.h"
