#include "rex_engine/gfx/resources/upload_buffer.h"

#include "rex_engine/memory/pointer_math.h"

namespace rex
{
  namespace gfx
  {
    UploadDataAllocation::UploadDataAllocation(UploadBuffer* uploadBuffer, rsl::span<rsl::byte> allocation, s64 allocationWriteOffset)
      : m_upload_buffer(uploadBuffer)
      , m_allocation(allocation)
      , m_write_offset(allocationWriteOffset)
    {
    }

    rsl::byte& UploadDataAllocation::operator[](s32 idx)
    {
      return m_allocation[idx];
    }
    const rsl::byte& UploadDataAllocation::operator[](s32 idx) const
    {
      return m_allocation[idx];
    }

    rsl::byte* UploadDataAllocation::data()
    {
      return m_allocation.data();
    }
    s64 UploadDataAllocation::size() const
    {
      return m_allocation.size_bytes();
    }
    s64 UploadDataAllocation::write_offset() const
    {
      return m_write_offset;
    }

    bool UploadDataAllocation::is_from_buffer(UploadBuffer* uploadBuffer) const
    {
      return m_upload_buffer == uploadBuffer;
    }


    UploadBuffer::UploadBuffer(rsl::memory_size size)
      : Buffer(size)
      , m_texture_write_offset(0)
      , m_buffer_write_offset(size)
    {}

    void UploadBuffer::reset()
    {
      m_texture_write_offset = 0;
      m_buffer_write_offset = size();
    }

    bool UploadBuffer::can_fit_alloc(rsl::memory_size allocSize, s32 alignment)
    {
      s64 aligned_offset = align(m_texture_write_offset, alignment);
      return aligned_offset + allocSize < m_buffer_write_offset;
    }

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
    s64 UploadBuffer::inc_texture_offset(s64 size, s32 alignment)
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