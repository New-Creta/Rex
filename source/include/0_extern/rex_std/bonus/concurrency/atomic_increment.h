// ============================================
//
// REX - STANDARD LIBRARY IMPLEMENTATION
//
// Author: Nick De Breuck
// Twitter: @nick_debreuck
//
// File: atomic_increment.h
// Copyright (c) Nick De Breuck 2022
//
// ============================================

#pragma once

#include "rex_std/bonus/types.h"

namespace rsl
{
  inline namespace v1
  {

    int8 atomic_increment(int8& val);
    int16 atomic_increment(int16& val);
    int32 atomic_increment(int32& val);
    int64 atomic_increment(int64& val);

    uint8 atomic_increment(uint8& val);
    uint16 atomic_increment(uint16& val);
    uint32 atomic_increment(uint32& val);
    uint64 atomic_increment(uint64& val);

  } // namespace v1
} // namespace rsl
