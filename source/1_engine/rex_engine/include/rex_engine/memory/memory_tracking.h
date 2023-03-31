#pragma once

#include "rex_engine/memory/memory_tags.h"
#include "rex_engine/types.h"
#include "rex_std/array.h"
#include "rex_std/mutex.h"
#include "rex_std/stacktrace.h"
#include "rex_std/thread.h"
#include "rex_std_extra/memory/memory_size.h"
#include "rex_std_extra/utility/enum_reflection.h"
#include "rex_std_extra/utility/high_water_mark.h"

namespace rex
{
  struct MemoryHeader
  {
  public:
    MemoryHeader(MemoryTag tag, void* ptr, rsl::memory_size size, rsl::thread::id threadId, card32 frameIdx, const rsl::stacktrace& callstack)
        : m_callstack(callstack)
        , m_size(size)
        , m_ptr(ptr)
        , m_thread_id(threadId)
        , m_tag(tag)
        , m_frame_idx(frameIdx)
    {
    }

    const rsl::stacktrace& callstack() const
    {
      return m_callstack;
    }
    rsl::memory_size size() const
    {
      return m_size;
    }
    void* ptr() const
    {
      return m_ptr;
    }
    rsl::thread::id thread_id() const
    {
      return m_thread_id;
    }
    MemoryTag tag() const
    {
      return m_tag;
    }
    card32 frame_index() const
    {
      return m_frame_idx;
    }

  private:
    rsl::stacktrace m_callstack;
    rsl::memory_size m_size;     // size of the memory allocated
    void* m_ptr;                 // the pointer that's allocated
    rsl::thread::id m_thread_id; // the thread id this was allocated on
    MemoryTag m_tag;             // memory tag that allocated this memory
    card32 m_frame_idx;          // frame index when this memory was allocated
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

    UsagePerTag current_stats(); // deliberate copy as we don't want to have any race conditions when accessing

  private:
    rsl::high_water_mark<s64> m_mem_usage; // current memory usage
    s64 m_max_mem_usage;                   // maximum allowed memory usage
    rsl::mutex m_mem_tracking_mutex;
    rsl::mutex m_mem_tag_tracking_mutex;
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

#define MEM_TAG_SCOPE(tag) MemoryTagScope ANONYMOUS_VARIABLE(mem_tag_scope)(tag)
} // namespace rex