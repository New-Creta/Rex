// ============================================
//
// REX - STANDARD LIBRARY IMPLEMENTATION
//
// Author: Nick De Breuck
// Twitter: @nick_debreuck
//
// File: piecewise_construct.h
// Copyright (c) Nick De Breuck 2022
//
// ============================================

#pragma once

namespace rsl
{
  inline namespace v1
  {

    struct piecewise_construct_t
    {
      explicit piecewise_construct_t() = default;
    };

    constexpr piecewise_construct_t piecewise_construct = piecewise_construct_t();

  } // namespace v1
} // namespace rsl
