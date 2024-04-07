#include "rex_engine/timing/timepoint.h"

namespace rex
{
  rsl::string current_timepoint_str()
  {
    const rsl::time_point current_time = rsl::current_timepoint();
    rsl::string timepoint_str(rsl::format("{}_{}", current_time.date().to_string_without_weekday(), current_time.time().to_string()));
    timepoint_str.replace("/", "_");
    timepoint_str.replace(":", "_");
    return timepoint_str;
  }
}