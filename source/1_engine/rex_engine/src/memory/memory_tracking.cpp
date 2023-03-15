#include "rex_engine/memory/memory_tracking.h"

#include "rex_std/iostream.h"
#include "rex_std/limits.h"

//  // we need to have an internal error reporting here
//  // so that we don't allocate heap memory when overflowing
//  // as this would cause a stack overflow
#ifdef REX_ENABLE_MEM_TRACKING
  #define REX_HEAP_TRACK_ERR(cond, msg)                                                                                                                                                                                                                  \
    if(!(cond))                                                                                                                                                                                                                                          \
    {                                                                                                                                                                                                                                                    \
      rsl::cout << "Err: " << msg << "\n";                                                                                                                                                                                                               \
    }
#else
  #define REX_HEAP_TRACK_ERR(cond, msg)
#endif

namespace rex
{
  rsl::array<MemoryTag, 100>& thread_local_memory_tag_stack()
  {
    thread_local static rsl::array<MemoryTag, 100> stack;
    return stack;
  }

  card32& thread_local_mem_tag_index()
  {
    thread_local static card32 tag = 0;
    return tag;
  }

  MemoryTracker::MemoryTracker()
      : m_mem_usage(0)
      , m_max_mem_usage(rsl::numeric_limits<s64>::max())
  {
  }

  void MemoryTracker::initialize(rsl::memory_size maxMemUsage)
  {
    m_max_mem_usage = rsl::high_water_mark<s64>(static_cast<s64>(maxMemUsage));
  }

  void MemoryTracker::track_alloc(void* /*mem*/, MemoryHeader* header)
  {
    rsl::unique_lock lock(m_mem_tracking_mutex);
    m_mem_usage += header->size().size_in_bytes();
    m_usage_per_tag[rsl::enum_refl::enum_integer(header->tag())] += header->size().size_in_bytes();
    REX_HEAP_TRACK_ERR(m_mem_usage.value() <= m_max_mem_usage, "Using more memory than allowed! usage: " << m_mem_usage.value() << " max: " << m_max_mem_usage);
  }
  void MemoryTracker::track_dealloc(void* /*mem*/, MemoryHeader* header)
  {
    rsl::unique_lock lock(m_mem_tracking_mutex);
    m_mem_usage -= header->size().size_in_bytes();
    m_usage_per_tag[rsl::enum_refl::enum_integer(header->tag())] -= header->size().size_in_bytes();
  }

  void MemoryTracker::push_tag(MemoryTag tag)
  {
    rsl::unique_lock lock(m_mem_tag_tracking_mutex);
    thread_local_memory_tag_stack()[++thread_local_mem_tag_index()] = tag;
  }
  void MemoryTracker::pop_tag()
  {
    rsl::unique_lock lock(m_mem_tag_tracking_mutex);
    --thread_local_mem_tag_index();
  }

  MemoryTag MemoryTracker::current_tag() const
  {
    return thread_local_memory_tag_stack()[thread_local_mem_tag_index()];
  }

  MemoryTracker::UsagePerTag MemoryTracker::current_stats()
  {
    const rsl::unique_lock lock(m_mem_tracking_mutex);
    MemoryTracker::UsagePerTag usage_per_tag_copy = m_usage_per_tag;
    return usage_per_tag_copy;
  }

  MemoryTracker& mem_tracker()
  {
    static MemoryTracker tracker;
    return tracker;
  }
} // namespace rex
