#include "rex_engine/memory/memory_stats.h"

#include "rex_engine/diagnostics/debug.h"

namespace rex
{
  void log_mem_usage()
  {
    const MemoryStats stats = query_memory_stats();

    // Physical memory
    rsl::medium_stack_string msg;
    msg += "[REX] Current used physical memory: ";
    msg += rsl::to_stack_string(stats.used_physical_mem.size_in_bytes());
    msg += " bytes.";
    output_debug_string(msg);

    msg.clear();

    // Virtual memory
    msg += "[REX] Current used virtual memory:  ";
    msg += rsl::to_stack_string(stats.used_virtual_mem.size_in_bytes());
    msg += " bytes.";
    output_debug_string(msg);
  }
} // namespace rex