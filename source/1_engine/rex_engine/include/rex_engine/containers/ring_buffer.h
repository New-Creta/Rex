#pragma once

#include "rex_engine/diagnostics/assert.h"
#include "rex_engine/engine/types.h"
#include "rex_std/bonus/utility/casting.h"
#include "rex_std/memory.h"

#include "rex_std/bonus/memory.h"
#include "rex_std/bonus/types.h"

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

    // Check if the next put will wrap to the beginning of the buffer
    bool will_wrap_on_next_put() const;
    // Check if the next get will wrap to the beginning of the buffer
    bool will_wrap_on_next_get() const;

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

    s64 m_capacity;
  };

  //-------------------------------------------------------------------------
  template <typename T>
  RingBuffer<T>::RingBuffer(s32 numElements)
      : m_data(nullptr)
      , m_get_pos(0)
      , m_put_pos(0)
      , m_capacity(numElements)
  {
    m_data = rsl::make_unique<T[]>(rsl::safe_numeric_cast<u32>(sizeof(T) * m_capacity));
  }

  //-------------------------------------------------------------------------
  template <typename T>
  bool RingBuffer<T>::put(const T& item)
  {
    if (m_put_pos == m_capacity)
    {
      m_put_pos = 0;
    }
    m_data[m_put_pos] = rsl::move(item);
    ++m_put_pos;

    return true;
  }

  //-------------------------------------------------------------------------
  template <typename T>
  T* RingBuffer<T>::get()
  {
    if (m_get_pos == m_capacity)
    {
      m_get_pos = 0;
    }

    const u32 gp = m_get_pos;
    if(gp == m_put_pos)
    {
      return nullptr;
    }
    ++m_get_pos;
    return &m_data[gp];
  }

  //-------------------------------------------------------------------------
  template <typename T>
  T* RingBuffer<T>::check()
  {
    u32 gp = m_get_pos;
    if(gp == m_put_pos)
    {
      return nullptr;
    }
    return &m_data[gp];
  }

  //-------------------------------------------------------------------------
  template <typename T>
  bool RingBuffer<T>::will_wrap_on_next_put() const
  {
    return m_put_pos >= m_capacity;
  }

  //-------------------------------------------------------------------------
  template <typename T>
  bool RingBuffer<T>::will_wrap_on_next_get() const
  {
    return m_get_pos >= m_capacity;
  }

  //-------------------------------------------------------------------------
  template <typename T>
  void RingBuffer<T>::reset()
  {
    m_get_pos = 0;
    m_put_pos = 0;
  }
} // namespace rex