// ============================================
//
// REX - STANDARD LIBRARY IMPLEMENTATION
//
// Author: Nick De Breuck
// Twitter: @nick_debreuck
//
// File: future.h
// Copyright (c) Nick De Breuck 2022
//
// ============================================

#pragma once

#include "rex_std/bonus/defines.h"
#include "rex_std/disable_std_checking.h"
#include "rex_std/internal/memory/uses_allocator.h"
#include "rex_std/std_alias_defines.h"

#include <future>

namespace rsl
{
  inline namespace v1
  {

    REX_STD_TEMPLATED_CLASS_ALIAS(template <typename R>, promise, R);

    REX_STD_TEMPLATED_CLASS_ALIAS(template <typename R>, packaged_task, R);

    REX_STD_TEMPLATED_CLASS_ALIAS(template <typename T>, future, T);

    REX_STD_TEMPLATED_CLASS_ALIAS(template <typename T>, shared_future, T);

    REX_STD_CLASS_ALIAS(launch);
    REX_STD_CLASS_ALIAS(future_status);
    REX_STD_CLASS_ALIAS(future_errc);

    REX_STD_FUNC_ALIAS(async);
    REX_STD_FUNC_ALIAS(future_category);
    // REX_STD_FUNC_ALIAS(swap); // Doesn't work because rsl has its own swap // Doesn't work because rsl has its own swap

  } // namespace v1
} // namespace rsl

#include "rex_std/enable_std_checking.h"
