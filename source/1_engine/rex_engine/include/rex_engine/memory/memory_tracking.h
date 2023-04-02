#pragma once

#include "rex_engine/memory/memory_tags.h"
#include "rex_engine/memory/memory_header.h"
#include "rex_engine/memory/memory_stats.h"
#include "rex_engine/types.h"
#include "rex_std/mutex.h"
#include "rex_std/stacktrace.h"
#include "rex_std/thread.h"
#include "rex_std_extra/memory/memory_size.h"
#include "rex_std_extra/utility/enum_reflection.h"
#include "rex_std_extra/utility/high_water_mark.h"

namespace rex
{
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

    UsagePerTag current_stats(); // deliberate copy as we don't want to have any race conditions when accessing

  private:
    rsl::high_water_mark<s64> m_mem_usage; // current memory usage
    s64 m_max_mem_usage;                   // maximum allowed memory usage
    MemoryStats m_mem_stats;                // stats queried from the OS at init time
    rsl::mutex m_mem_tracking_mutex;
    rsl::mutex m_mem_tag_tracking_mutex;
    UsagePerTag m_usage_per_tag;
    bool m_is_initialized;
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

#define MEM_TAG_SCOPE(tag) MemoryTagScope ANONYMOUS_VARIABLE(mem_tag_scope)(tag)
} // namespace rex