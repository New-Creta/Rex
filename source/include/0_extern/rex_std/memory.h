// ============================================
//
// REX - STANDARD LIBRARY IMPLEMENTATION
//
// Author: Nick De Breuck
// Twitter: @nick_debreuck
//
// File: memory.h
// Copyright (c) Nick De Breuck 2022
//
// ============================================

//-----------------------------------------------------------------------------
// https://en.cppreference.com/w/cpp/header/memory
//
// High level memory management utilities
//-----------------------------------------------------------------------------

#pragma once

#include "rex_std/internal/memory/allocator.h"
#include "rex_std/internal/memory/allocator_arg_t.h"
#include "rex_std/internal/memory/allocator_traits.h"
#include "rex_std/internal/memory/pointer_traits.h"
#include "rex_std/internal/memory/shared_ptr.h"
#include "rex_std/internal/memory/unique_ptr.h"
#include "rex_std/internal/memory/uses_allocator.h"
// #include "rex_std/internal/memory/weak_ptr.h"
// #include "rex_std/internal/memory/owner_less.h"
// #include "rex_std/internal/memory/enable_shared_from_this.h"
// #include "rex_std/internal/memory/bad_weak_ptr.h"
#include "rex_std/internal/memory/construct_at.h"
#include "rex_std/internal/memory/default_delete.h"
#include "rex_std/internal/memory/destroy.h"
#include "rex_std/internal/memory/destroy_at.h"
#include "rex_std/internal/memory/destroy_n.h"
#include "rex_std/internal/memory/uninitialized_copy.h"
#include "rex_std/internal/memory/uninitialized_copy_n.h"
#include "rex_std/internal/memory/uninitialized_default_construct.h"
#include "rex_std/internal/memory/uninitialized_default_construct_n.h"
#include "rex_std/internal/memory/uninitialized_fill.h"
#include "rex_std/internal/memory/uninitialized_fill_n.h"
#include "rex_std/internal/memory/uninitialized_move.h"
#include "rex_std/internal/memory/uninitialized_move_n.h"
#include "rex_std/internal/memory/uninitialized_value_construct.h"
#include "rex_std/internal/memory/uninitialized_value_construct_n.h"

namespace rsl
{
  inline namespace v1
  {
    template <typename T>
    struct hash;

    template <typename T>
    class atomic;

  } // namespace v1
} // namespace rsl
