#include "rex_engine/memory/memory_stats.h"

#include "rex_engine/diagnostics/debug.h"

namespace rex
{
  void log_mem_usage()
  {
    const MemoryStats stats = query_memory_stats();

    // Physical memory
    output_debug_string(rsl::format("Current used physical memory: {} bytes.", stats.used_physical_mem.size_in_bytes()));
    output_debug_string(rsl::format("Current used virtual memory: {} bytes.", stats.used_virtual_mem.size_in_bytes()));
  }
} // namespace rex