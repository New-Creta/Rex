#pragma once

#include "rex_engine/memory/debug_allocator.h"
#include "rex_engine/memory/memory_stats.h"
#include "rex_engine/memory/memory_tags.h"
#include "rex_engine/memory/untracked_allocator.h"
#include "rex_engine/engine/types.h"
#include "rex_std/array.h"
#include "rex_std/bonus/attributes.h"
#include "rex_std/bonus/defines.h"
#include "rex_std/bonus/memory/memory_size.h"
#include "rex_std/bonus/types.h"
#include "rex_std/bonus/utility/enum_reflection.h"
#include "rex_std/bonus/utility/high_water_mark.h"
#include "rex_std/mutex.h"
#include "rex_std/vector.h"

namespace rex
{
  class MemoryHeader;

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
    MemoryTracker(const MemoryTracker&) = delete;
    MemoryTracker(MemoryTracker&&)      = delete;
    ~MemoryTracker();

    MemoryTracker& operator=(const MemoryTracker&) = delete;
    MemoryTracker& operator=(MemoryTracker&&)      = delete;

    void initialize(rsl::memory_size maxMemUsage);

    MemoryHeader* track_alloc(void* mem, card64 size);
    void track_dealloc(MemoryHeader* header);

    void push_tag(MemoryTag tag);
    void pop_tag();

    MemoryTag current_tag() const;

    void dump_stats_to_file(rsl::string_view filepath);

    REX_NO_DISCARD MemoryUsageStats current_stats();      // deliberate copy as we don't want to have any race conditions when accessing
    REX_NO_DISCARD MemoryUsageStats get_pre_init_stats(); // deliberate copy as we don't want to have any race conditions when accessing
    REX_NO_DISCARD MemoryUsageStats get_init_stats();     // deliberate copy as we don't want to have any race conditions when accessing

  private:
    REX_NO_DISCARD MemoryUsageStats get_stats_for_frame(card32 idx);

    rsl::high_water_mark<s64> m_mem_usage; // current memory usage
    s64 m_max_mem_usage;                   // maximum allowed memory usage
    MemoryStats m_mem_stats_on_startup;    // stats queried from the OS at init time
    rsl::mutex m_mem_tracking_mutex;
    UsagePerTag m_usage_per_tag;
    bool m_is_active;
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

#define REX_MEM_TAG_SCOPE(tag) const MemoryTagScope ANONYMOUS_VARIABLE(mem_tag_scope)(tag)
} // namespace rex