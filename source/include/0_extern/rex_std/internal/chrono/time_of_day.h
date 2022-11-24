// ============================================
//
// REX - STANDARD LIBRARY IMPLEMENTATION
//
// Author: Nick De Breuck
// Twitter: @nick_debreuck
//
// File: time_of_day.h
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
    ::chrono
    {
      using hh_mm_ss = std::chrono::hh_mm_ss;

      using std::chrono::is_am;
      using std::chrono::is_pm;
      using std::chrono::make12;
      using std::chrono::make24;
    }

  #include "rex_std/enable_std_checking.h"

#endif
