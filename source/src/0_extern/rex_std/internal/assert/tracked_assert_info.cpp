// ============================================ 
//
// REX - STANDARD LIBRARY IMPLEMENTATION
//
// Author: Nick De Breuck
// Twitter: @nick_debreuck
// 
// File: tracked_assert_info.cpp
// Copyright (c) Nick De Breuck 2022
//
// ============================================

#include "rex_std/internal/assert/tracked_assert_info.h"

rsl::assert::TrackedAssertInfo::TrackedAssertInfo(AssertInfo&& assert_info)
  : m_assert_info(assert_info)
  , m_triggered_count(1)
{}

void rsl::assert::TrackedAssertInfo::inc_trigger_count()
{
  ++m_triggered_count;
}

const rsl::AssertInfo& rsl::assert::TrackedAssertInfo::info() const
{
  return m_assert_info;
}
