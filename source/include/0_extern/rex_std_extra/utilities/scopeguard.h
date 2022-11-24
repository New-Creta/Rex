// ============================================
//
// REX - STANDARD LIBRARY IMPLEMENTATION
//
// Author: Nick De Breuck
// Twitter: @nick_debreuck
//
// File: scopeguard.h
// Copyright (c) Nick De Breuck 2022
//
// ============================================

#pragma once

#include "rex_std/functional.h"

namespace rsl
{
  inline namespace v1
  {
    class ScopeGuard
    {
    public:
      ScopeGuard(rsl::function<void()> func);
      ~ScopeGuard();

      void release();

    private:
      rsl::function<void()> m_function;
      bool m_is_released;
    };

  }
}
