#pragma once

#include "rex_std/string.h"
#include "rex_std/chrono.h"
#include "rex_std/bonus/time/timepoint.h"

namespace rex
{
  rsl::string timepoint_str(rsl::time_point timepoint);
  rsl::string current_timepoint_str();
}