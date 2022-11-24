// ============================================
//
// REX - STANDARD LIBRARY IMPLEMENTATION
//
// Author: Nick De Breuck
// Twitter: @nick_debreuck
//
// File: time_zone.h
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
      using tzdb                   = std::chrono::tzdb;
      using tzdb_list              = std::chrono::tzdb_list;
      using time_zone              = std::chrono::time_zone;
      using sys_info               = std::chrono::sys_info;
      using local_info             = std::chrono::local_info;
      using choose                 = std::chrono::choose;
      using zoned_traits           = std::chrono::zoned_traits;
      using zoned_time             = std::chrono::zoned_time;
      using time_zone_link         = std::chrono::time_zone_link;
      using nonexistent_local_time = std::chrono::nonexistent_local_time;
      using ambiguous_local_time   = std::chrono::ambiguous_local_time;

      using std::chrono::get_tzdb;
      using std::chrono::get_tzdb_list;
      using std::chrono::reload_tzdb;
    }

  #include "rex_std/enable_std_checking.h"

#endif
