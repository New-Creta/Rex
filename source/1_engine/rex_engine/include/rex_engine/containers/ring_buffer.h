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
  // lockless single producer single consumer - thread safe ring buffer
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
    bool put(const T& item);

    // get the next element and increment the get pos
    T* get();
    // get the next element and don't increment the get pos
    T* check();

    // reset the buffer, setting the put and get position back to zero
    void reset();

  private:
    rsl::unique_array<T> m_data;

    s32 m_get_pos;
    s32 m_put_pos;
    s32 m_num_reads_available;
    s32 m_capacity;
  };

  //-------------------------------------------------------------------------
  template <typename T>
  RingBuffer<T>::RingBuffer(s32 numElements)
      : m_data(nullptr)
      , m_get_pos(0)
      , m_put_pos(0)
      , m_num_reads_available(0)
      , m_capacity(numElements)
  {
    m_data = rsl::make_unique<T[]>(rsl::safe_numeric_cast<u32>(sizeof(T) * m_capacity));
  }

  //-------------------------------------------------------------------------
  template <typename T>
  bool RingBuffer<T>::put(const T& item)
  {
    m_data[m_put_pos] = rsl::move(item);
    m_num_reads_available = rsl::min(m_num_reads_available + 1, m_capacity);
    ++m_put_pos;
    if (m_put_pos == m_capacity)
    {
      m_put_pos = 0;
    }

    return true;
  }

  //-------------------------------------------------------------------------
  template <typename T>
  T* RingBuffer<T>::get()
  {
    T* res = check();

    if (res)
    {
      --m_num_reads_available;
      ++m_get_pos;
    
      if (m_get_pos == m_capacity)
      {
        m_get_pos = 0;
      }
    }

    return res;
  }

  //-------------------------------------------------------------------------
  template <typename T>
  T* RingBuffer<T>::check()
  {
    if (m_num_reads_available == 0)
    {
      return nullptr;
    }

    return &m_data[m_get_pos];
  }

  //-------------------------------------------------------------------------
  template <typename T>
  void RingBuffer<T>::reset()
  {
    m_get_pos = 0;
    m_put_pos = 0;
  }
} // namespace rex