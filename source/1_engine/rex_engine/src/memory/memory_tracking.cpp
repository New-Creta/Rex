#include "rex_engine/memory/memory_tracking.h"

#include "rex_engine/core_application.h"
#include "rex_engine/diagnostics/legacy/logging.h"
#include "rex_engine/frameinfo/frameinfo.h"
#include "rex_std/iostream.h"
#include "rex_std/limits.h"

namespace rex
{
#ifdef REX_MAX_ALLOWED_MEM_TAGS
  inline constexpr card32 g_max_allowed_mem_tags = REX_MAX_ALLOWED_MEM_TAGS;
#else
  inline constexpr card32 g_max_allowed_mem_tags = 100;
#endif

  rsl::array<MemoryTag, g_max_allowed_mem_tags>& thread_local_memory_tag_stack()
  {
    thread_local static rsl::array<MemoryTag, g_max_allowed_mem_tags> stack = {MemoryTag::Global};
    return stack;
  }

  card32& thread_local_mem_tag_index()
  {
    thread_local static card32 tag = 0;
    return tag;
  }

  MemoryTracker::MemoryTracker()
      : m_mem_usage(0)
      , m_max_mem_usage((rsl::numeric_limits<s64>::max)())
      , m_is_initialized(false)
  {
  }

  void MemoryTracker::initialize(rsl::memory_size maxMemUsage)
  {
    // Remember that the OS has an overhead of memory allocation for the process itself
    // eg. Windows has a few MBs overhead for the process on startup, including page file usage and page faults
    // therefore a very low max memory usage could could fall below this
    // That's why we track the initial memory usage before the initialization
    // so we can subtract this later, making sure that we only track the memory
    // that got allocated at runtime
    m_max_mem_usage = rsl::high_water_mark<s64>(static_cast<s64>(maxMemUsage));
    m_mem_stats = query_memory_stats();
    m_is_initialized = true;
  }

  void MemoryTracker::track_alloc(void* /*mem*/, MemoryHeader* header)
  {
    // It's possible static or global variables perform heap allocation
    // this is not allowed however, so we need to check here if that happened
    REX_ASSERT_X(m_is_initialized, "Memory allocation before the memory tracker is initialized, this is forbidden");

    const rsl::unique_lock lock(m_mem_tracking_mutex);
    m_mem_usage += header->size().size_in_bytes();
    m_usage_per_tag[rsl::enum_refl::enum_integer(header->tag())] += header->size().size_in_bytes();
    REX_ERROR_X(m_mem_usage.value() <= m_max_mem_usage, "Using more memory than allowed! usage: {} max: {}", m_mem_usage.value(), m_max_mem_usage);
  }
  void MemoryTracker::track_dealloc(void* /*mem*/, MemoryHeader* header)
  {
    const rsl::unique_lock lock(m_mem_tracking_mutex);
    REX_WARN_X(header->frame_index() != globals::frame_info().index(), "Memory freed in the same frame it's allocated (please use single frame allocator for this)");
    m_mem_usage -= header->size().size_in_bytes();
    m_usage_per_tag[rsl::enum_refl::enum_integer(header->tag())] -= header->size().size_in_bytes();
  }

  void MemoryTracker::push_tag(MemoryTag tag)
  {
    const rsl::unique_lock lock(m_mem_tag_tracking_mutex);
    thread_local_memory_tag_stack()[thread_local_mem_tag_index()] = tag;
    ++thread_local_mem_tag_index();
  }
  void MemoryTracker::pop_tag()
  {
    const rsl::unique_lock lock(m_mem_tag_tracking_mutex);
    --thread_local_mem_tag_index();
  }

  MemoryTag MemoryTracker::current_tag() const // NOLINT(readability-convert-member-functions-to-static)
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
