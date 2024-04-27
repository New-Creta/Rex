#include "rex_engine/containers/typeless_ring_buffer.h"
#include "rex_engine/diagnostics/assert.h"
#include "rex_engine/diagnostics/log.h"

#include "rex_std/memory.h"
#include "rex_std/bonus/memory.h"

namespace rex
{
  // construct the ring buffer with the given size, in bytes
  TypelessRingBuffer::TypelessRingBuffer(rsl::memory_size size)
    : m_data(rsl::make_unique<rsl::byte[]>(static_cast<s32>(size)))
    , m_get_pos(0)
    , m_put_pos(0)
    , m_num_reads_available(0)
  {
    REX_ASSERT_X(size > 0, "Creating a typeless ring buffer of invalid size");
  }

  // Write x amount of bytes into the buffer
  void TypelessRingBuffer::write(const void* data, rsl::memory_size size)
  {
    REX_ASSERT_X(size < m_data.count(), "Overwriting all data of the ring buffer, this doesn't make sense");

    // Write as much as we can into the buffer
    // we start by adding to the buffer if possible, wrapping around if we can't append everything
    s32 space_available_until_end = m_data.count() - m_put_pos;
    s32 size_to_copy = rsl::min(space_available_until_end, static_cast<s32>(size.size_in_bytes()));

    // the src is simply the input
    // the dst is wherever we currently are in the buffer for writing
    const rsl::byte* src = static_cast<const rsl::byte*>(data);
    rsl::byte* dst = m_data.get() + m_put_pos;
    rsl::memcpy(dst, src, size_to_copy);
    m_put_pos += size_to_copy;

    // If we still have bytes to write, wrap around and write the remaining bytes
    s32 remaining_size = static_cast<s32>(size.size_in_bytes() - size_to_copy);
    if (remaining_size > 0)
    {
      // If we wrap around, we reset our destination to the beginning of our buffer
      // we need to make sure we advance our source as well to where we've already copied
      dst = m_data.get();
      src += size_to_copy;
      rsl::memcpy(dst, src, remaining_size);
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
    m_num_reads_available -= static_cast<s32>(size);

    // update the get pos to the right position
    skip(static_cast<s32>(size));
  }
  // Read x amount of bytes from the buffer, without increasing the read offset
  void TypelessRingBuffer::peek(void* data, rsl::memory_size size) const
  {
    REX_ASSERT_X(m_num_reads_available >= size, "Not enough bytes available for reading, would read invalid memory from buffer");
   
    // Read as much of the buffer as we can
    s32 space_available_until_end = m_data.count() - m_get_pos;
    s32 size_to_copy = rsl::min(space_available_until_end, static_cast<s32>(size.size_in_bytes()));

    // the src is where we currently are in the buffer for reading
    // the dst is simply the data argument passed in
    const rsl::byte* src = m_data.get() + m_get_pos;
    rsl::byte* dst = static_cast<rsl::byte*>(data);
    rsl::memcpy(dst, src, size_to_copy);
    s32 remaining_size = static_cast<s32>(size.size_in_bytes() - size_to_copy);

    // If there's still data we need to read, wrap around
    // and read it from the beginning of the buffer
    if (remaining_size > 0)
    {
      // If we wrap around, we reset our source to the beginning of our buffer
      // we need to make sure we advance our destination as well to where we've already copied
      src = m_data.get();
      dst += size_to_copy;
      rsl::memcpy(dst, src, remaining_size);
    }
  }

  // Resets the read and write offset to the start of the buffer
  void TypelessRingBuffer::reset()
  {
    m_get_pos = 0;
    m_put_pos = 0;
    m_num_reads_available = 0;
  }
  // increments the read offset by the given amount
  void TypelessRingBuffer::skip(s32 offset)
  {
    REX_ASSERT_X(offset >= 0, "Invalid offset given to typeless ring buffer");

    m_get_pos += offset;
    if (m_get_pos > m_data.count())
    {
      m_get_pos -= m_data.count();
    }
  }
}