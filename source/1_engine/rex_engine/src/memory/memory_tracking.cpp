#include "rex_engine/diagnostics/logging.h"
#include "rex_engine/filesystem/filesystem.h"
#include "rex_engine/memory/memory_tracking.h"
#include "rex_engine/frameinfo/frameinfo.h"
#include "rex_std/iostream.h"
#include "rex_std/limits.h"

namespace rex
{
  rsl::array<MemoryTag, 100>& thread_local_memory_tag_stack()
  {
    thread_local static rsl::array<MemoryTag, 100> stack = { MemoryTag::Global };
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
    m_is_initialized = true;
  }

  void MemoryTracker::track_alloc(void* /*mem*/, MemoryHeader* header)
  {
    REX_ASSERT_X(m_is_initialized, "Trying to track allocation but memory tracker isn't initialized yet");

    const rsl::unique_lock lock(m_mem_tracking_mutex);
    increment_mem_usage(header->size().size_in_bytes());
    increment_mem_tag_usage(header->tag(), header->size().size_in_bytes());
    
    save_to_file(header, AllocOp::Allocation);

    REX_ERROR_X(m_mem_usage.value() <= m_max_mem_usage, "Using more memory than allowed! usage: {} max: {}", m_mem_usage.value(), m_max_mem_usage);
  }
  void MemoryTracker::track_dealloc(void* /*mem*/, MemoryHeader* header)
  {
    REX_ASSERT_X(m_is_initialized, "Trying to track allocation but memory tracker isn't initialized yet");

    const rsl::unique_lock lock(m_mem_tracking_mutex);
    decrement_mem_usage(header->size().size_in_bytes());
    decrement_mem_tag_usage(header->tag(), header->size().size_in_bytes());

    save_to_file(header, AllocOp::Deallocation);
  }

  void MemoryTracker::push_tag(MemoryTag tag)
  {
    REX_ASSERT_X(m_is_initialized, "Trying to track allocation but memory tracker isn't initialized yet");

    const rsl::unique_lock lock(m_mem_tag_tracking_mutex);
    thread_local_memory_tag_stack()[++thread_local_mem_tag_index()] = tag;
  }
  void MemoryTracker::pop_tag()
  {
    REX_ASSERT_X(m_is_initialized, "Trying to track allocation but memory tracker isn't initialized yet");

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

  void MemoryTracker::increment_mem_usage(card64 size)
  {
    m_mem_usage += size;
  }

  void MemoryTracker::increment_mem_tag_usage(MemoryTag tag, card64 size)
  {
    m_usage_per_tag[rsl::enum_refl::enum_integer(tag)] += size;

  }

  void MemoryTracker::decrement_mem_usage(card64 size)
  {
    m_mem_usage -= size;
  }

  void MemoryTracker::decrement_mem_tag_usage(MemoryTag tag, card64 size)
  {
    m_usage_per_tag[rsl::enum_refl::enum_integer(tag)] -= size;
  }

  void MemoryTracker::save_to_file(MemoryHeader* header, AllocOp op)
  {
    // Serialize the header and save it to disk
    // Alloc Operation
    // Ptr
    // Size
    // Thread ID
    // Tag
    // Frame Idx
    // Callstack

    rsl::array<rsl::byte, 100> dummy_array{};
    vfs::save_to_file("dummy_file", &op, sizeof(AllocOp));
    vfs::save_to_file("dummy_file", header, sizeof(MemoryHeader));
  }

  MemoryTracker& mem_tracker()
  {
    static MemoryTracker tracker;
    return tracker;
  }
} // namespace rex
