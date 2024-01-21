#pragma once

#include "rex_engine/engine/types.h"
#include "rex_std/ostream.h"
#include "rex_std/string.h"
#include "rex_std/bonus/memory/memory_size.h"

namespace rex
{
  struct GpuDescription
  {
    GpuDescription();

    rsl::small_stack_string name;
    rsl::small_stack_string vendor_name;

    u32 vendor_id;
    u32 device_id;

    rsl::memory_size dedicated_video_memory;
    rsl::memory_size dedicated_system_memory;
    rsl::memory_size shared_system_memory;
  };

  rsl::ostream& operator<<(rsl::ostream& os, const GpuDescription& desc);
} // namespace rex