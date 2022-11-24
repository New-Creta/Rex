// ============================================
//
// REX - STANDARD LIBRARY IMPLEMENTATION
//
// Author: Nick De Breuck
// Twitter: @nick_debreuck
//
// File: leap_second.h
// Copyright (c) Nick De Breuck 2022
//
// ============================================

#pragma once

#ifdef REX_ENABLE_WITH_CPP20

  #include "rex_std/disable_std_checking.h"

  #include <chrono>

namespace rsl
{
  inline namespace v1
  {

    using leap_second      = std::chrono::leap_second;
    using leap_second_info = std::chrono::leap_second_info;

    using std::chrono::get_leap_second_info;
    using std::chrono::operator==;
    using std::chrono::operator<;
    using std::chrono::operator<=;
    using std::chrono::operator>;
    using std::chrono::operator>=;

  } // namespace v1
} // namespace rsl

  #include "rex_std/enable_std_checking.h"

#endif
