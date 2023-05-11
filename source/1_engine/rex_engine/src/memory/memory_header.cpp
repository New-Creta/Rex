#include "rex_engine/memory/memory_header.h"

namespace rex
{
  MemoryHeader::MemoryHeader(MemoryTag tag, void* ptr, rsl::memory_size size, rsl::thread::id threadId, card32 frameIdx, const rsl::shared_ptr<CallStack>& callstack)
      : m_callstack(callstack)
      , m_size(size)
      , m_ptr(ptr)
      , m_thread_id(threadId)
      , m_tag(tag)
      , m_frame_idx(frameIdx)
  {
  }

  const CallStack* MemoryHeader::callstack() const
  {
    return m_callstack.get();
  }
  count_t MemoryHeader::callstack_count() const
  {
    return m_callstack.use_count();
  }
  rsl::memory_size MemoryHeader::size() const
  {
    return m_size;
  }
  void* MemoryHeader::ptr() const
  {
    return m_ptr;
  }
  rsl::thread::id MemoryHeader::thread_id() const
  {
    return m_thread_id;
  }
  MemoryTag MemoryHeader::tag() const
  {
    return m_tag;
  }
  card32 MemoryHeader::frame_index() const
  {
    return m_frame_idx;
  }

} // namespace rex