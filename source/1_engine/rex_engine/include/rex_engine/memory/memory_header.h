#pragma once

#include "rex_engine/diagnostics/win/win_stacktrace.h" // for CallStack
#include "rex_std/bonus/types.h"              // for card32
#include "rex_std/thread.h"                   // for thread
#include "rex_std_extra/memory/memory_size.h" // for memory_size

namespace rex
{
  enum class MemoryTag;

  class MemoryHeader
  {
  public:
    MemoryHeader(MemoryTag tag, void* ptr, rsl::memory_size size, rsl::thread::id threadId, card32 frameIdx, const CallStack& callstack);

    const CallStack& callstack() const;
    rsl::memory_size size() const;
    void* ptr() const;
    rsl::thread::id thread_id() const;
    MemoryTag tag() const;
    card32 frame_index() const;

  private:
    CallStack m_callstack;
    rsl::memory_size m_size;     // size of the memory allocated
    void* m_ptr;                 // the pointer that's allocated
    rsl::thread::id m_thread_id; // the thread id this was allocated on
    MemoryTag m_tag;             // memory tag that allocated this memory
    card32 m_frame_idx;          // frame index when this memory was allocated
  };

} // namespace rex