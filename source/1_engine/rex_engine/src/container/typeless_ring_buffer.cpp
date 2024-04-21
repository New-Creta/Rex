#include "rex_engine/containers/typeless_ring_buffer.h"
#include "rex_engine/diagnostics/assert.h"

#include "rex_std/memory.h"
#include "rex_std/bonus/memory.h"

namespace rex
{
  // construct the ring buffer with the given size, in bytes
  TypelessRingBuffer::TypelessRingBuffer(rsl::memory_size size)
    : m_data(rsl::make_unique<rsl::byte[]>(size))
    , m_get_pos(0)
    , m_put_pos(0)
    , m_num_reads_available(0)
  {}

  // Write x amount of bytes into the buffer
  void TypelessRingBuffer::write(const void* data, rsl::memory_size size)
  {
    // Write as much as we can into the buffer
    s32 space_available_until_end = m_data.count() - m_put_pos;
    rsl::memcpy(m_data.get() + m_put_pos, data, space_available_until_end);
    m_put_pos += size;

    // If we still have bytes to write, wrap around and write the remaining bytes
    s32 remaining_size = (size.size_in_bytes() - space_available_until_end);
    if (remaining_size > 0)
    {
      rsl::memcpy(m_data.get(), data, remaining_size);
      m_put_pos = remaining_size;
    }

    // specify how much data we can read from the buffer
    m_num_reads_available = rsl::min(static_cast<s32>(m_num_reads_available + size), m_data.count());
  }
  // Read x amount of bytes from the buffer
  void TypelessRingBuffer::read(void* data, rsl::memory_size size)
  {
    // Read in the data
    peek(data, size);

    // update how many bytes we have available for reading
    m_num_reads_available -= size;

    // update the get pos to the right position
    s32 space_available_until_end = m_data.count() - m_get_pos;
    m_get_pos = m_get_pos + size < m_data.count()
      ? m_get_pos += size
      : m_get_pos = (size.size_in_bytes() - space_available_until_end);
  }
  // Read x amount of bytes from the buffer, without increasing the read offset
  void TypelessRingBuffer::peek(void* data, rsl::memory_size size) const
  {
    REX_ASSERT_X(m_num_reads_available >= size, "Not enough bytes available for reading, would read invalid memory from buffer");
   
    // Read as much of the buffer as we can
    s32 space_available_until_end = rsl::min(static_cast<s32>(size.size_in_bytes()), m_data.count() - m_get_pos);
    rsl::memcpy(data, m_data.get() + m_get_pos, space_available_until_end);
    size -= space_available_until_end;

    // If there's still data we need to read, wrap around
    // and read it from the beginning of the buffer
    if (size > 0)
    {
      rsl::memcpy(data, m_data.get(), size);
    }
  }

  // Resets the read and write offset to the start of the buffer
  void TypelessRingBuffer::reset()
  {
    m_get_pos = 0;
    m_put_pos = 0;
    m_num_reads_available = 0;
  }
}