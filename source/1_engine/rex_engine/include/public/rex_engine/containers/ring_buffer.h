#pragma once

#include "rex_engine/diagnostics/assert.h"
#include "rex_engine/engine/types.h"
#include "rex_std/bonus/utility/casting.h"
#include "rex_std/memory.h"

#include "rex_std/bonus/memory.h"
#include "rex_std/bonus/types.h"
#include "rex_std/algorithm.h"

namespace rex
{
  template <typename T>
  class RingBuffer
  {
  public:
    // Create a ring buffer with a given amount of elements
    explicit RingBuffer(s32 numElements);

    RingBuffer(const RingBuffer&) = delete;
    RingBuffer(RingBuffer&&)      = delete;

    ~RingBuffer() = default;

    RingBuffer& operator=(const RingBuffer&) = delete;
    RingBuffer& operator=(RingBuffer&&)      = delete;

    // Put a new item at the next available location in the ring buffer
    // returns true if the next put would be at the start of the buffer,
    // returns false otherwise
    bool write(const T& item);

    // get the next element and increment the get pos
    T read();
    // get the next element but don't increment the get pos
    T peek() const;

    // reset the buffer, setting the put and get position back to zero
    void reset();

    // skip count items in the buffer
    void skip(s32 count);

    // returns the number of items in the buffer
    s32 count() const;
    // returns the maximum number of items the buffer can hold
    s32 max_count() const;
    // returns the byte size of the buffer
    s32 size() const;

  private:
    rsl::unique_array<T> m_data;

    s32 m_get_pos;
    s32 m_put_pos;
    s32 m_num_reads_available;
  };

  //-------------------------------------------------------------------------
  template <typename T>
  RingBuffer<T>::RingBuffer(s32 numElements)
      : m_data(nullptr)
      , m_get_pos(0)
      , m_put_pos(0)
      , m_num_reads_available(0)
  {
    m_data = rsl::make_unique<T[]>(numElements);
  }

  //-------------------------------------------------------------------------
  template <typename T>
  bool RingBuffer<T>::write(const T& item)
  {
    m_data[m_put_pos] = rsl::move(item);
    m_num_reads_available = rsl::min(m_num_reads_available + 1, m_data.count());
    ++m_put_pos;
    if (m_put_pos == m_data.count())
    {
      m_put_pos = 0;
      return true;
    }

    return false;
  }

  //-------------------------------------------------------------------------
  template <typename T>
  T RingBuffer<T>::read()
  {
    T res = peek();

    skip(1);

    return res;
  }

  //-------------------------------------------------------------------------
  template <typename T>
  T RingBuffer<T>::peek() const
  {
    REX_ASSERT_X(m_num_reads_available > 0, "Not enough bytes available for reading, would read invalid memory from buffer");
    return m_data[m_get_pos];
  }

  //-------------------------------------------------------------------------
  template <typename T>
  void RingBuffer<T>::reset()
  {
    m_get_pos = 0;
    m_put_pos = 0;
    m_num_reads_available = 0;
  }

  //-------------------------------------------------------------------------
  template <typename T>
  void RingBuffer<T>::skip(s32 count)
  {
    REX_ASSERT_X(count >= 0, "Negative count given to typeless ring buffer for skipping, this is not allowed");
    REX_ASSERT_X(count <= m_num_reads_available, "Count given for skipping in ring buffer is bigger than the number of reads available, this is not allowed");

    m_get_pos += count;
    m_num_reads_available -= count;
    if (m_get_pos >= m_data.count())
    {
      m_get_pos -= m_data.count();
    }

  }

  //-------------------------------------------------------------------------
  template <typename T>
  s32 RingBuffer<T>::count() const
  {
    return m_num_reads_available;
  }

  //-------------------------------------------------------------------------
  template <typename T>
  s32 RingBuffer<T>::max_count() const
  {
    return m_data.count();
  }
  
  //-------------------------------------------------------------------------
  template <typename T>
  s32 RingBuffer<T>::size() const
  {
    return m_data.byte_size();
  }
} // namespace rex