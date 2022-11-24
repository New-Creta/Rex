// ============================================
//
// REX - STANDARD LIBRARY IMPLEMENTATION
//
// Author: Nick De Breuck
// Twitter: @nick_debreuck
//
// File: ranges.h
// Copyright (c) Nick De Breuck 2022
//
// ============================================

#pragma once

#ifdef REX_ENABLE_WITH_CPP20

  #include "rex_std/disable_std_checking.h"
  #include "rex_std/std_alias_defines.h"

  #include <ranges>

namespace rsl
{
  inline namespace v1
  {

    using namespace std::ranges;
    using namespace std::ranges::views;

  } // namespace v1
} // namespace rsl

  #include "rex_std/enable_std_checking.h"

#endif // REX_ENABLE_WITH_CPP20
