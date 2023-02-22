#pragma once

#include "rex_engine/types.h"
#include "rex_std_extra/memory/memory_size.h"
#include "rex_std_extra/utility/high_water_mark.h"

namespace rex
{
  class MemoryManager
  {
  public:
    MemoryManager();

    void initialize(rsl::memory_size maxMemUsage);

    void track_alloc(u64 size);
    void track_dealloc(u64 size);

  private:
    rsl::high_water_mark<s64> m_mem_usage; // current memory usage
    s64 m_max_mem_usage;                   // maximum allowed memory usage
  };

  MemoryManager& mem_manager();
} // namespace rex
