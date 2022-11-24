// ============================================
//
// REX - STANDARD LIBRARY IMPLEMENTATION
//
// Author: Nick De Breuck
// Twitter: @nick_debreuck
//
// File: uninitialized_default_construct.h
// Copyright (c) Nick De Breuck 2022
//
// ============================================

#pragma once

namespace rsl
{
  inline namespace v1
  {

    template <typename T>
    void uninitialized_default_construct(T* ptr)
    {
      new(ptr) T();
    }

  } // namespace v1
} // namespace rsl
