#pragma once

namespace rex
{
  struct MemoryStats;

  namespace win
  {
    MemoryStats query_memory_stats();
  } // namespace win
} // namespace rex