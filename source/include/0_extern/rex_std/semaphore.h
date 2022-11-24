// ============================================
//
// REX - STANDARD LIBRARY IMPLEMENTATION
//
// Author: Nick De Breuck
// Twitter: @nick_debreuck
//
// File: semaphore.h
// Copyright (c) Nick De Breuck 2022
//
// ============================================

#pragma once

#include "rex_std/bonus/types.h"
#include "rex_std/disable_std_checking.h"
#include "rex_std/std_alias_defines.h"

#include <semaphore>

namespace rsl
{
  inline namespace v1
  {

    REX_STD_TEMPLATED_CLASS_ALIAS(template <ptrdiff LeastMaxValue>, counting_semaphore, LeastMaxValue);

    REX_STD_CLASS_ALIAS(binary_semaphore);

  } // namespace v1
} // namespace rsl

#include "rex_std/enable_std_checking.h"
