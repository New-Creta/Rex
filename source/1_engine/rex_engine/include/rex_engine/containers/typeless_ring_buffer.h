#pragma once

#include "rex_engine/engine/types.h"
#include "rex_std/memory.h"
#include "rex_std/bonus/memory.h"

namespace rex
{
  class TypelessRingBuffer
  {
  public:
    // construct the ring buffer with the given size, in bytes
    TypelessRingBuffer(rsl::memory_size size);

    TypelessRingBuffer(const TypelessRingBuffer&) = delete;
    TypelessRingBuffer(TypelessRingBuffer&&) = delete;

    ~TypelessRingBuffer() = default;

    TypelessRingBuffer& operator=(const TypelessRingBuffer&) = delete;
    TypelessRingBuffer& operator=(TypelessRingBuffer&&) = delete;

    // Write x amount of bytes into the buffer
    void write(const void* data, rsl::memory_size size);
    // Read x amount of bytes from the buffer
    void read(void* data, rsl::memory_size size);
    // Read x amount of bytes from the buffer, without increasing the read offset
    void peek(void* data, rsl::memory_size size) const;

    // Resets the read and write offset to the start of the buffer
    void reset();
    // increments the read offset by the given amount
    void skip(s32 offset);

  private:
    rsl::unique_array<rsl::byte> m_data;

    s32 m_get_pos;
    s32 m_put_pos;
    s32 m_num_reads_available;
  };
}