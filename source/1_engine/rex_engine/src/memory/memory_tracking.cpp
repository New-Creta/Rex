#include "rex_engine/memory/memory_tracking.h"

#include "rex_engine/core_application.h"
#include "rex_engine/diagnostics/assert.h"
#include "rex_engine/frameinfo/frameinfo.h"
#include "rex_engine/filesystem/vfs.h"
#include "rex_engine/memory/debug_allocator.h"
#include "rex_engine/memory/global_allocator.h"
#include "rex_engine/log.h"
#include "rex_std/iostream.h"
#include "rex_std/limits.h"
#include "rex_std_extra/time/timepoint.h"
#include "rex_std/algorithm.h"

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

  class AllocationCallstack
  {
  public:
    AllocationCallstack(Callstack callstack)
      : m_callstack(callstack)
    {}

  private:
    m_callstack;
  };

  // stores the headers for all allocations
  auto& allocation_headers()
  {
    static UntrackedAllocator allocator {};
    static DebugAllocator dbg_alloc(allocator); // NOLINT(misc-const-correctness)
    static DebugVector<MemoryHeader*> alloc_headers(dbg_alloc);
    return alloc_headers;
  }

  // stores the unique callstacks for all allocations
  auto& allocation_callstacks()
  {
    static UntrackedAllocator allocator{};
    static DebugAllocator dbg_alloc(allocator); // NOLINT(misc-const-correctness)
    static DebugVector<rsl::shared_ptr<CallStack>> callstack_to_allocations(dbg_alloc);
    return callstack_to_allocations;
  }

  // stores deleters for all deleted allocations
  auto& deleter_callstacks()
  {
    static UntrackedAllocator allocator{};
    static DebugAllocator dbg_alloc(allocator); // NOLINT(misc-const-correctness)
    static DebugHashTable<const CallStack*, DebugVector<CallStack>> callstack_to_deleters(dbg_alloc);
    return callstack_to_deleters;
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

  rsl::shared_ptr<CallStack> MemoryTracker::track_callstack()
  {
    const rsl::unique_lock lock(m_mem_tracking_mutex);

    static UntrackedAllocator allocator{};
    static DebugAllocator dbg_alloc(allocator); // NOLINT(misc-const-correctness)

    CallStack this_callstack = current_callstack();
    auto& alloc_callstacks = allocation_callstacks();
    auto it = rsl::find_if(alloc_callstacks.begin(), alloc_callstacks.end(),
      [&](const rsl::shared_ptr<CallStack>& callstack)
      {
        return this_callstack == *callstack;
      });
    if (it == allocation_callstacks().end())
    {
      allocation_callstacks().push_back(rsl::allocate_shared<CallStack>(dbg_alloc, this_callstack));
      return allocation_callstacks().back();
    }

    return *it;
  }

  MemoryHeader* MemoryTracker::track_alloc(void* mem, card64 size)
  {
    const MemoryTag tag = current_tag();
    const rsl::thread::id thread_id = rsl::this_thread::get_id();

    rex::GlobalDebugAllocator& dbg_alloc = rex::global_debug_allocator();
    rex::MemoryHeader* dbg_header_addr = static_cast<rex::MemoryHeader*>(dbg_alloc.allocate(sizeof(MemoryHeader)));
    const card32 frame_idx = globals::frame_info().index();
    CallStack callstack = current_callstack();

    auto& alloc_callstacks = allocation_callstacks();
    auto it = alloc_callstacks.find(callstack);
    if (it == alloc_callstacks.end())
    {
      alloc_callstacks[callstack] = AllocationCallstack(callstack, size);
    }
    it->value->add_size(size);

    mem_tracker().track_callstack();
    rex::MemoryHeader* header = new(dbg_header_addr) MemoryHeader(tag, mem, size, thread_id, frame_idx, mem_tracker().track_callstack());

    const rsl::unique_lock lock(m_mem_tracking_mutex);
    m_mem_usage += header->size().size_in_bytes();
    m_usage_per_tag[rsl::enum_refl::enum_integer(header->tag())] += header->size().size_in_bytes();
    allocation_headers().push_back(header);

    return header;
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
    
    // add unique deleter callstacks
    DebugVector<CallStack>& del_callstacks = deleter_callstacks()[header->callstack()];
    CallStack current_callstack = rex::current_callstack();
    if (rsl::find(del_callstacks.cbegin(), del_callstacks.cend(), current_callstack) == del_callstacks.cend())
    {
      deleter_callstacks()[header->callstack()].push_back(rex::current_callstack());
    }

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

  void MemoryTracker::dump_stats_to_file(rsl::string_view filepath)
  {
    MemoryUsageStats stats = current_stats();

    rsl::stringstream ss;

    for (count_t i = 0; i < stats.usage_per_tag.size(); ++i)
    {
      MemoryTag tag = static_cast<MemoryTag>(i);
      ss << rsl::format("{}: {} bytes\n", rsl::enum_refl::enum_name(tag), stats.usage_per_tag[i]);
    }

    ss << "----------------------------\n";

    ss << rsl::format("Number of unique callstacks: {}\n", allocation_callstacks().size());

    static UntrackedAllocator allocator{};
    static DebugAllocator dbg_alloc(allocator); // NOLINT(misc-const-correctness)
    static DebugHashTable<const CallStack*, ResolvedCallstack> callstack_to_resolved(dbg_alloc);

    for (MemoryHeader* header : stats.allocation_headers)
    {
      const CallStack* callstack = header->callstack();
      auto it = callstack_to_resolved.find(callstack);
      ResolvedCallstack* resolved_callstack = nullptr;
      if (it == callstack_to_resolved.end())
      {
        callstack_to_resolved.emplace(callstack, ResolvedCallstack(*callstack));
      }
      resolved_callstack = &callstack_to_resolved.find(callstack)->value;

      ss << rsl::format("Count: {}\n", header->callstack_count());
      ss << rsl::format("Frame: {}\n", header->frame_index());
      ss << rsl::format("Thread ID: {}\n", header->thread_id());
      ss << rsl::format("Memory Tag: {}\n", rsl::enum_refl::enum_name(header->tag()));
      ss << rsl::format("Size: {}\n", header->size());

      for (count_t i = 0; i < resolved_callstack->size(); ++i)
      {
        ss << rsl::format("{}\n", (*resolved_callstack)[i]);
      }
    }

    rsl::string_view content = ss.view();

    rsl::time_point time_point = rsl::current_timepoint();
    rsl::string dated_filepath;
    dated_filepath += time_point.date().to_string_without_weekday();
    dated_filepath += "_";
    dated_filepath += time_point.time().to_string();
    dated_filepath += "_";
    dated_filepath += filepath;
    rsl::replace(dated_filepath.begin(), dated_filepath.end(), ':', '_');
    rsl::replace(dated_filepath.begin(), dated_filepath.end(), '/', '_');
    
    vfs::save_to_file(dated_filepath, content.data(), content.length(), vfs::AppendToFile::no);
  }

  MemoryUsageStats MemoryTracker::current_stats()
  {
    const rsl::unique_lock lock(m_mem_tracking_mutex);
    MemoryUsageStats stats {};
    stats.usage_per_tag      = m_usage_per_tag;
    stats.allocation_headers = allocation_headers();
    return stats;
  }

  MemoryUsageStats MemoryTracker::get_pre_init_stats()
  {
    return get_stats_for_frame(-1); // allocations that happen before initialization have -1 as frame index
  }

  MemoryUsageStats MemoryTracker::get_init_stats()
  {
    return get_stats_for_frame(0); // allocations that happen at initialization have 0 as frame index
  }

  MemoryUsageStats MemoryTracker::get_stats_for_frame(card32 idx)
  {
    rsl::unique_lock lock(m_mem_tracking_mutex);
    DebugVector<MemoryHeader*> alloc_headers = allocation_headers();
    lock.unlock();

    MemoryUsageStats stats{};
    stats.allocation_headers.reserve(alloc_headers.size());

    for (MemoryHeader* header : alloc_headers)
    {
      if (header->frame_index() == idx)
      {
        stats.allocation_headers.push_back(header);
        stats.usage_per_tag[rsl::enum_refl::enum_integer(header->tag())] += header->size();
      }
      else if (header->frame_index() > idx)
      {
        break;
      }
    }

    return stats;
  }

  MemoryTracker& mem_tracker()
  {
    static MemoryTracker tracker;
    return tracker;
  }
} // namespace rex

namespace rsl
{
  inline namespace v1
  {
    template<>
    struct hash<rex::AllocationCallstack>
    {
      hash_result operator()(const rex::AllocationCallstack& callstack) const
      {

      }
    };
  }
}