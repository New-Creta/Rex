#include "rex_engine/memory/memory_tracking.h"

#include "rex_engine/core_application.h"               // for frame_info
#include "rex_engine/diagnostics/assert.h"             // for REX_ASSERT_X
#include "rex_engine/diagnostics/logging/log_macros.h" // for REX_ERROR_X
#include "rex_engine/frameinfo/frameinfo.h"            // for FrameInfo
#include "rex_engine/log.h"                            // for LogEngine
#include "rex_engine/memory/memory_header.h"           // for MemoryHeader
#include "rex_engine/memory/memory_tags.h"             // for MemoryTag
#include "rex_engine/memory/win/win_mem_stats.h"       // for query_memory_...
#include "rex_std/bonus/types.h"                       // for card32
#include "rex_std/limits.h"                            // for numeric_limits
#include "rex_std/stacktrace.h"                        // for const_random_...

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

  auto& allocation_headers()
  {
    static UntrackedAllocator allocator {};
    static DebugAllocator dbg_alloc(allocator); // NOLINT(misc-const-correctness)
    static rsl::vector<MemoryHeader*, decltype(dbg_alloc)> alloc_headers(dbg_alloc);
    return alloc_headers;
  }

  card32& thread_local_mem_tag_index()
  {
    thread_local static card32 tag = 0;
    return tag;
  }

  MemoryTracker::MemoryTracker()
      : m_mem_usage(0)
      , m_max_mem_usage((rsl::numeric_limits<s64>::max)())
  {
  }

  void MemoryTracker::initialize(rsl::memory_size maxMemUsage)
  {
    // Remember that the OS has an overhead of memory allocation for the process itself
    // eg. Windows has a few MBs overhead for the process on startup, including page file usage and page faults
    // therefore a very low max memory usage could fall below this
    // That's why we track the initial memory usage before the initialization
    // so we can subtract this later, making sure that we only track the memory
    // that got allocated at runtime
    m_max_mem_usage        = rsl::high_water_mark<s64>(static_cast<s64>(maxMemUsage));
    m_mem_stats_on_startup = query_memory_stats();
  }

  void MemoryTracker::track_alloc(void* /*mem*/, MemoryHeader* header)
  {
    const rsl::unique_lock lock(m_mem_tracking_mutex);
    m_mem_usage += header->size().size_in_bytes();
    m_usage_per_tag[rsl::enum_refl::enum_integer(header->tag())] += header->size().size_in_bytes();
    allocation_headers().push_back(header);
    REX_ERROR_X(LogEngine, m_mem_usage.value() <= m_max_mem_usage, "Using more memory than allowed! usage: {} max: {}", m_mem_usage.value(), m_max_mem_usage);
  }
  void MemoryTracker::track_dealloc(void* /*mem*/, MemoryHeader* header)
  {
    const rsl::unique_lock lock(m_mem_tracking_mutex);
    REX_WARN_X(LogEngine, header->frame_index() != globals::frame_info().index(), "Memory freed in the same frame it's allocated (please use single frame allocator for this)");
    m_mem_usage -= header->size().size_in_bytes();
    auto it = rsl::find(allocation_headers().cbegin(), allocation_headers().cend(), header);
    REX_ASSERT_X(it != allocation_headers().cend(), "Trying to remove a memory header that wasn't tracked");
    REX_ASSERT_X(m_mem_usage >= 0, "Mem usage below 0");
    allocation_headers().erase(it);
    m_usage_per_tag[rsl::enum_refl::enum_integer(header->tag())] -= header->size().size_in_bytes();
  }

  void MemoryTracker::push_tag(MemoryTag tag) // NOLINT(readability-convert-member-functions-to-static)
  {
    thread_local_memory_tag_stack()[thread_local_mem_tag_index()] = tag;
    ++thread_local_mem_tag_index();
  }
  void MemoryTracker::pop_tag() // NOLINT(readability-convert-member-functions-to-static)
  {
    --thread_local_mem_tag_index();
  }

  MemoryTag MemoryTracker::current_tag() const // NOLINT(readability-convert-member-functions-to-static)
  {
    return thread_local_memory_tag_stack()[thread_local_mem_tag_index()];
  }

  MemoryUsageStats MemoryTracker::current_stats()
  {
    const rsl::unique_lock lock(m_mem_tracking_mutex);
    MemoryUsageStats stats {};
    stats.usage_per_tag      = m_usage_per_tag;
    stats.allocation_headers = allocation_headers();
    return stats;
  }

  MemoryTracker& mem_tracker()
  {
    static MemoryTracker tracker;
    return tracker;
  }
} // namespace rex
