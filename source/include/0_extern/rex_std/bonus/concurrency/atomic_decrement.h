// ============================================
//
// REX - STANDARD LIBRARY IMPLEMENTATION
//
// Author: Nick De Breuck
// Twitter: @nick_debreuck
//
// File: atomic_decrement.h
// Copyright (c) Nick De Breuck 2022
//
// ============================================

#pragma once

#include "rex_std/bonus/types.h"

namespace rsl
{
  inline namespace v1
  {

    int8 atomic_decrement(int8& val);
    int16 atomic_decrement(int16& val);
    int32 atomic_decrement(int32& val);
    int64 atomic_decrement(int64& val);

    uint8 atomic_decrement(uint8& val);
    uint16 atomic_decrement(uint16& val);
    uint32 atomic_decrement(uint32& val);
    uint64 atomic_decrement(uint64& val);

  } // namespace v1
} // namespace rsl
