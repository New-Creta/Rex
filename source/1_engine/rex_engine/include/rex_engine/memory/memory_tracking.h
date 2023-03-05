#pragma once

#include "rex_engine/types.h"

#include "rex_std_extra/memory/memory_size.h"
#include "rex_std_extra/utility/high_water_mark.h"
#include "rex_std_extra/utility/enum_reflection.h"

#include "rex_std/mutex.h"
#include "rex_std/array.h"

namespace rex
{
  enum class MemoryTag
  {
    Global,

    Engine,
    Editor
  };

  struct MemoryHeader
  {
  public:
    MemoryHeader(MemoryTag tag, rsl::memory_size size)
      : m_tag(tag)
      , m_size(size)
    {}

    MemoryTag tag() const
    {
      return m_tag;
    }
    rsl::memory_size size() const
    {
      return m_size;
    }

  private:
    MemoryTag m_tag;
    rsl::memory_size m_size;
  };

  class MemoryTracker
  {
  public:
    MemoryTracker();

    void initialize(rsl::memory_size maxMemUsage);

    void track_alloc(void* mem, MemoryHeader* header);
    void track_dealloc(void* mem, MemoryHeader* header);

    void push_tag(MemoryTag tag);
    void pop_tag();

    MemoryTag current_tag() const;

  private:
    rsl::high_water_mark<s64> m_mem_usage; // current memory usage
    s64 m_max_mem_usage; // maximum allowed memory usage
    rsl::mutex m_mem_tracking_mutex;
    rsl::mutex m_mem_tag_tracking_mutex;
    rsl::array<rsl::high_water_mark<s64>, rsl::enum_refl::enum_count<MemoryTag>()> m_usage_per_tag;
  };

  MemoryTracker& mem_tracker();

  class MemoryTagScope
  {
  public:
    MemoryTagScope(MemoryTag tag)
    {
      mem_tracker().push_tag(tag);
    }
    ~MemoryTagScope()
    {
      mem_tracker().pop_tag();
    }
  };

#define MEM_TAG_SCOPE(tag) MemoryTagScope ANONYMOUS_VARIABLE(mem_tag_scope)(tag)
}