#pragma once

#include "rex_engine/memory/debug_allocator.h" // for DebugAllocator
#include "rex_engine/memory/memory_stats.h" // for MemoryStats
#include "rex_engine/memory/untracked_allocator.h" // for UntrackedAllocator
#include "rex_engine/types.h"                      // for s64
#include "rex_std/array.h"                         // for array
#include "rex_std/bonus/defines.h"                 // for ANONYMOUS_VARIABLE
#include "rex_std/mutex.h"                         // for mutex
#include "rex_std/vector.h"                        // for vector
#include "rex_std_extra/memory/memory_size.h"      // for memory_size
#include "rex_std_extra/utility/enum_reflection.h" // for enum_count
#include "rex_std_extra/utility/high_water_mark.h" // for high_water_mark

namespace rex
{
  class MemoryHeader;
  enum class MemoryTag;

  struct MemoryUsageStats
  {
    using UsagePerTag = rsl::array<rsl::high_water_mark<s64>, rsl::enum_refl::enum_count<MemoryTag>()>;

    UsagePerTag usage_per_tag;
    rsl::vector<MemoryHeader*, DebugAllocator<UntrackedAllocator>> allocation_headers;
  };

  class MemoryTracker
  {
  public:
    using UsagePerTag = rsl::array<rsl::high_water_mark<s64>, rsl::enum_refl::enum_count<MemoryTag>()>;

    MemoryTracker();

    void initialize(rsl::memory_size maxMemUsage);

    void track_alloc(void* mem, MemoryHeader* header);
    void track_dealloc(void* mem, MemoryHeader* header);

    void push_tag(MemoryTag tag);
    void pop_tag();

    MemoryTag current_tag() const;

    MemoryUsageStats current_stats(); // deliberate copy as we don't want to have any race conditions when accessing

  private:
    rsl::high_water_mark<s64> m_mem_usage; // current memory usage
    s64 m_max_mem_usage;                   // maximum allowed memory usage
    MemoryStats m_mem_stats_on_startup;    // stats queried from the OS at init time
    rsl::mutex m_mem_tracking_mutex;
    UsagePerTag m_usage_per_tag;
  };

  MemoryTracker& mem_tracker();

  class MemoryTagScope
  {
  public:
    explicit MemoryTagScope(MemoryTag tag)
    {
      mem_tracker().push_tag(tag);
    }
    MemoryTagScope(const MemoryTagScope&) = delete;
    MemoryTagScope(MemoryTagScope&&)      = delete;
    ~MemoryTagScope()
    {
      mem_tracker().pop_tag();
    }

    MemoryTagScope& operator=(const MemoryTagScope&) = delete;
    MemoryTagScope& operator=(MemoryTagScope&&)      = delete;
  };

#define REX_MEM_TAG_SCOPE(tag) MemoryTagScope ANONYMOUS_VARIABLE(mem_tag_scope)(tag)
} // namespace rex