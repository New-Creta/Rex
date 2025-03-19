#include "rex_engine/timing/timepoint.h"

namespace rex
{
  rsl::string timepoint_str(rsl::time_point timepoint)
  {
    rsl::string timepoint_str(rsl::format("{}_{}", timepoint.date().to_string_without_weekday(), timepoint.time().to_string()));
    timepoint_str.replace("/", "_");
    timepoint_str.replace(":", "_");
    return timepoint_str;
  }

  rsl::string current_timepoint_str()
  {
    const rsl::time_point current_time = rsl::current_timepoint();
    return timepoint_str(current_time);
  }
}