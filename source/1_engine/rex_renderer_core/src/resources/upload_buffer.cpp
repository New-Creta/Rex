#include "rex_renderer_core/resources/upload_buffer.h"

#include "rex_engine/memory/pointer_math.h"

namespace rex
{
  namespace gfx
  {
    UploadBuffer::UploadBuffer(rsl::memory_size size)
      : Buffer(size)
      //, m_offset(0)
      , m_texture_write_offset(0)
      , m_buffer_write_offset(size)
    {}

    void UploadBuffer::reset()
    {
      //m_offset = 0;
      m_texture_write_offset = 0;
      m_buffer_write_offset = size();
    }

    bool UploadBuffer::can_fit_alloc(rsl::memory_size allocSize, s32 alignment)
    {
      s64 aligned_offset = align(m_texture_write_offset, alignment);
      return aligned_offset + allocSize < m_buffer_write_offset;
    }

    //s64 UploadBuffer::current_offset() const
    //{
    //  return m_offset;
    //}
    //s64 UploadBuffer::inc_offset(s64 size, s32 alignment)
    //{
    //  // First we align the offset to the value where the data actually got written,
    //  // only then we cache the old offset as otherwise we'd get graphics API warnings
    //  // that the offset is not aligned
    //  m_offset = align(m_offset, alignment);
    //  s64 old_offset = m_offset;
    //  m_offset += size;
    //  return old_offset;
    //}

    s64 UploadBuffer::current_buffer_offset() const
    {
      return m_buffer_write_offset;
    }
    s64 UploadBuffer::current_texture_offset() const
    {
      return m_texture_write_offset;
    }

    s64 UploadBuffer::dec_buffer_offset(s64 size)
    {
      s64 old_offset = m_buffer_write_offset;
      m_buffer_write_offset -= size;
      return old_offset;
    }
    s64 UploadBuffer::inc_texture_offsset(s64 size, s32 alignment)
    {
      // First we align the offset to the value where the data actually got written,
      // only then we cache the old offset as otherwise we'd get graphics API warnings
      // that the offset is not aligned
      m_texture_write_offset = align(m_texture_write_offset, alignment);
      s64 old_offset = m_texture_write_offset;
      m_texture_write_offset += size;
      return old_offset;
    }


  }
}