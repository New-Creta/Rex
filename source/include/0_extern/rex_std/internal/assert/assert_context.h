// ============================================ 
//
// REX - STANDARD LIBRARY IMPLEMENTATION
//
// Author: Nick De Breuck
// Twitter: @nick_debreuck
// 
// File: assert_context.h
// Copyright (c) Nick De Breuck 2022
//
// ============================================

#pragma once

#include "rex_std/string_view.h"

namespace rsl
{
  class AssertContext
  {
  public:
    AssertContext(rsl::string_view name);

    rsl::string_view name() const;

  private:
    rsl::string_view m_name;
  };
}