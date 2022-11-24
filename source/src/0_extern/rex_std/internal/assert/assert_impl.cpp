// ============================================
//
// REX - STANDARD LIBRARY IMPLEMENTATION
//
// Author: Nick De Breuck
// Twitter: @nick_debreuck
//
// File: assert_impl.cpp
// Copyright (c) Nick De Breuck 2022
//
// ============================================

#include "rex_std/internal/assert/assert_impl.h"

#include "rex_std/iostream.h"

namespace rsl
{
  inline namespace v1
  {
    namespace internal
    {
      void log_assert(const rsl::string& msg)
      {
        rsl::cerr << msg.data();
      }
    } // namespace internal
  }   // namespace v1
} // namespace rsl