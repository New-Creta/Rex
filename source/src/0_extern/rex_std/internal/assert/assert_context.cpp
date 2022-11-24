// ============================================ 
//
// REX - STANDARD LIBRARY IMPLEMENTATION
//
// Author: Nick De Breuck
// Twitter: @nick_debreuck
// 
// File: assert_context.cpp
// Copyright (c) Nick De Breuck 2022
//
// ============================================

#include "rex_std/internal/assert/assert_context.h"

rsl::AssertContext::AssertContext(rsl::string_view name)
  : m_name(name)
{}

rsl::string_view rsl::AssertContext::name() const
{
  return m_name;
}