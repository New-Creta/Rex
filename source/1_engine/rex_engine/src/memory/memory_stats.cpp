#include "rex_engine/memory/memory_stats.h"

#include "rex_engine/diagnostics/debug.h"
#include "rex_engine/diagnostics/log.h"
#include "rex_engine/memory/memory_tracking.h"

namespace rex
{
  DEFINE_LOG_CATEGORY(LogMemoryStats);

  MemoryTrackingStats query_mem_tracking_stats()
  {
    return mem_tracker().current_tracking_stats();
  }
  MemoryStats query_all_memory_stats()
  {
    MemoryStats stats{};
    stats.mem_tracking_stats = query_mem_tracking_stats();
    stats.system_mem_stats = query_system_mem_stats();
    stats.process_mem_stats = query_process_mem_stats();
    return stats;
  }

  void debug_log_mem_usage()
  {
    const MemoryStats stats = query_all_memory_stats();

    // The memory manager may not be initialized yet, so we can only log to the debug output who's allocations do not get tracked
    // If a user wants 
    auto physical_mem_usage = rsl::format("Current used physical memory: {} bytes.", stats.process_mem_stats.used_physical_mem.size_in_bytes());
    auto virtual_mem_usage = rsl::format("Current used virtual memory: {} bytes.", stats.process_mem_stats.used_virtual_mem.size_in_bytes());

    // Output debug string logging
    output_debug_string(physical_mem_usage);
    output_debug_string(virtual_mem_usage);
  }
} // namespace rex