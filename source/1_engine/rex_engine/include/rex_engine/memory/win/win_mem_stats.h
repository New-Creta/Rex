#pragma once

#include "rex_engine/memory/memory_stats.h"
#include "rex_engine/platform.h"

namespace rex
{
struct MemoryStats;

  namespace win
  {
    MemoryStats query_memory_stats();
  } // namespace win
} // namespace rex