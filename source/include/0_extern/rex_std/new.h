// ============================================
//
// REX - STANDARD LIBRARY IMPLEMENTATION
//
// Author: Nick De Breuck
// Twitter: @nick_debreuck
//
// File: new.h
// Copyright (c) Nick De Breuck 2022
//
// ============================================

#pragma once

#include "rex_std/bonus/defines.h"
#include "rex_std/disable_std_checking.h"
#include "rex_std/std_alias_defines.h"

#include <new>

namespace rsl
{
  inline namespace v1
  {

    REX_STD_CLASS_ALIAS(nothrow_t);
    REX_STD_CLASS_ALIAS(align_val_t);
    REX_STD_CLASS_ALIAS(destroying_delete_t);

    REX_STD_CLASS_ALIAS(new_handler);

    REX_STD_OBJECT_ALIAS(nothrow);
    REX_STD_OBJECT_ALIAS(destroying_delete);
    REX_STD_OBJECT_ALIAS(hardware_destructive_interference_size);
    REX_STD_OBJECT_ALIAS(hardware_constructive_interference_size);

    REX_STD_FUNC_ALIAS(get_new_handler);
    REX_STD_FUNC_ALIAS(set_new_handler);
    REX_STD_FUNC_ALIAS(launder);

  } // namespace v1
} // namespace rsl

#include "rex_std/enable_std_checking.h"
