#include "rex_renderer_core/resources/upload_buffer.h"

#include "rex_engine/memory/pointer_math.h"

namespace rex
{
  namespace gfx
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
      // First we align the offset to the value where the data actually got written,
      // only then we cache the old offset as otherwise we'd get graphics API warnings
      // that the offset is not aligned
      m_offset = align(m_offset, alignment);
      s64 old_offset = m_offset;
      m_offset += size;
      return old_offset;
    }

  }
}