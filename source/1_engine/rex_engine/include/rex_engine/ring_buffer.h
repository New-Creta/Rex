#pragma once

#include "rex_engine/types.h"
#include "rex_engine/memory/memory_allocation.h"

#include <rex_std/memory.h>

namespace rex
{
    // lockless single producer single consumer - thread safe ring buffer
    template <typename T>
    class RingBuffer
    {
    public:
        RingBuffer();
        ~RingBuffer();

        void create(u32 cap);
        void put(const T& item);
        T* get();
        T* check();

    private:
        T* m_data = nullptr;

        a_u32 m_get_pos;
        a_u32 m_put_pos;

        std::atomic<size_t> m_capacity;
    };

    //-------------------------------------------------------------------------
    template <typename T>
    RingBuffer<T>::RingBuffer()
    {
        m_get_pos = 0;
        m_put_pos = 0;
        m_capacity = 0;
    }
    //-------------------------------------------------------------------------
    template <typename T>
    RingBuffer<T>::~RingBuffer()
    {
        rex::memory_free(m_data);
    }
    //-------------------------------------------------------------------------
    template <typename T>
    inline void RingBuffer<T>::create(u32 capacity)
    {
        m_get_pos = 0;
        m_put_pos = 0;
        m_capacity = capacity;

        m_data = (T*)rex::memory_alloc(sizeof(T) * m_capacity.load());
        rsl::memset(m_data, 0x0, sizeof(T) * m_capacity.load());
    }
    //-------------------------------------------------------------------------
    template <typename T>
    void RingBuffer<T>::put(const T& item)
    {
        m_data[m_put_pos] = item;
        m_put_pos = (m_put_pos + 1) % m_capacity;
    }
    //-------------------------------------------------------------------------
    template <typename T>
    T* RingBuffer<T>::get()
    {
        u32 gp = m_get_pos;
        if (gp == m_put_pos)
        {
            return nullptr;
        }

        m_get_pos = (m_get_pos + 1) % m_capacity;

        return &m_data[gp];
    }
    //-------------------------------------------------------------------------
    template <typename T>
    T* RingBuffer<T>::check()
    {
        u32 gp = m_get_pos;
        if (gp == m_put_pos)
        {
            return nullptr;
        }
        return &m_data[gp];
    }
}