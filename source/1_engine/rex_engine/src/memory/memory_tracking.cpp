#include "rex_engine/diagnostics/logging.h"
#include "rex_engine/filesystem/filesystem.h"
#include "rex_engine/memory/memory_tracking.h"
#include "rex_engine/frameinfo/frameinfo.h"
#include "rex_std/iostream.h"
#include "rex_std/limits.h"
#include "rex_std_extra/utility/enum_reflection.h"

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
    , m_status(Status::Uninit)
  {
  }

  void MemoryTracker::pre_init()
  {
    m_status = Status::PreInit;
  }

  void MemoryTracker::initialize(rsl::memory_size maxMemUsage)
  {
    m_status = Status::Init;
    m_max_mem_usage = rsl::high_water_mark<s64>(static_cast<s64>(maxMemUsage));
    m_status = Status::Running;
  }

  void MemoryTracker::track_alloc(void* /*mem*/, MemoryHeader* header)
  {
    REX_ASSERT_X(m_status != Status::Uninit, "Trying to track allocation but memory tracker isn't initialized yet");

    const rsl::unique_lock lock(m_mem_tracking_mutex);
    increment_mem_usage(header->size().size_in_bytes());
    increment_mem_tag_usage(header->tag(), header->size().size_in_bytes());

    save_to_file(header, AllocOp::Allocation);

    REX_ERROR_X(m_mem_usage.value() <= m_max_mem_usage, "Using more memory than allowed! usage: {} max: {}", m_mem_usage.value(), m_max_mem_usage);
  }
  void MemoryTracker::track_dealloc(void* /*mem*/, MemoryHeader* header)
  {
    REX_ASSERT_X(m_status != Status::Uninit, "Trying to track allocation but memory tracker isn't initialized yet");

    const rsl::unique_lock lock(m_mem_tracking_mutex);
    decrement_mem_usage(header->size().size_in_bytes());
    decrement_mem_tag_usage(header->tag(), header->size().size_in_bytes());

    save_to_file(header, AllocOp::Deallocation);
  }

  void MemoryTracker::push_tag(MemoryTag tag)
  {
    REX_ASSERT_X(m_status != Status::Uninit, "Trying to track allocation but memory tracker isn't initialized yet");

    const rsl::unique_lock lock(m_mem_tag_tracking_mutex);
    thread_local_memory_tag_stack()[++thread_local_mem_tag_index()] = tag;
  }
  void MemoryTracker::pop_tag()
  {
    REX_ASSERT_X(m_status != Status::Uninit, "Trying to track allocation but memory tracker isn't initialized yet");

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

    rsl::string_view filename = "mem_tracking.log";

    rsl::string_view op_name = rsl::enum_refl::enum_name(op);
    void* ptr = header->ptr();
    card64 size = header->size();
    rsl::thread::id thread_id = header->thread_id();
    rsl::string_view tag_name = rsl::enum_refl::enum_name(header->tag());
    card32 frame_idx = header->frame_index();

    rsl::stack_string<char, 3000> callstack;
    for (card32 i = 0; i < header->callstack().size(); ++i)
    {
      rsl::stack_string<char, 512> entry = header->callstack()[i].description();
      callstack.append(entry.data(), entry.length());
      callstack += "\n";
    }

    vfs::save_to_file(filename, op_name.data(), op_name.length(), true);
    vfs::save_to_file(filename, &ptr, sizeof(ptr), true);
    vfs::save_to_file(filename, &size, sizeof(size), true);
    vfs::save_to_file(filename, &thread_id, sizeof(thread_id), true);
    vfs::save_to_file(filename, tag_name.data(), tag_name.length(), true);
    vfs::save_to_file(filename, &frame_idx, sizeof(frame_idx), true);
    vfs::save_to_file(filename, callstack.data(), callstack.length(), true);
    vfs::save_to_file(filename, "\n", 1, true);
  }

  MemoryTracker& mem_tracker()
  {
    static MemoryTracker tracker;
    return tracker;
  }
} // namespace rex
