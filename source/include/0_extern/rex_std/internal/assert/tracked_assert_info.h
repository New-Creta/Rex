// ============================================ 
//
// REX - STANDARD LIBRARY IMPLEMENTATION
//
// Author: Nick De Breuck
// Twitter: @nick_debreuck
// 
// File: tracked_assert_info.h
// Copyright (c) Nick De Breuck 2022
//
// ============================================

#pragma once

#include "assert_info.h"

namespace rsl::assert
{
  class TrackedAssertInfo
  {
  public:
    TrackedAssertInfo(AssertInfo&& assert_info);

    void inc_trigger_count();

    const AssertInfo& info() const;
  private:
    AssertInfo m_assert_info;
    count_t m_triggered_count;
  };
}