// ============================================
//
// REX - STANDARD LIBRARY IMPLEMENTATION
//
// Author: Nick De Breuck
// Twitter: @nick_debreuck
//
// File: is_nan.h
// Copyright (c) Nick De Breuck 2022
//
// ============================================

#pragma once

template <typename T>
constexpr bool is_nan(const T x)
{
  return x != x; // NOLINT(misc-redundant-expression)
}