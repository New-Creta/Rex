#include "rex_renderer_core/system/upload_buffer.h"

#include "rex_engine/memory/pointer_math.h"

namespace rex
{
  namespace rhi
  {
    UploadBuffer::UploadBuffer(rsl::memory_size size)
      : Buffer(size)
      , m_offset(0)
    {}

    void UploadBuffer::reset()
    {
      m_offset = 0;
    }

    bool UploadBuffer::can_fit_alloc(rsl::memory_size allocSize, s32 alignment)
    {
      s64 aligned_offset = align(m_offset, alignment);
      return aligned_offset + allocSize < size();
    }

    s64 UploadBuffer::current_offset() const
    {
      return m_offset;
    }
    s64 UploadBuffer::inc_offset(s64 size, s32 alignment)
    {
      s64 old_offset = m_offset;
      m_offset = align(m_offset, alignment);
      m_offset += size;
      return old_offset;
    }

  }
}