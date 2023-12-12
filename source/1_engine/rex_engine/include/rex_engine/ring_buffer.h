#pragma once

#include "rex_engine/types.h"
#include "rex_engine/memory/memory_allocation.h"
#include "rex_engine/diagnostics/assert.h"

#include <rex_std/memory.h>

namespace rex
{
    // lockless single producer single consumer - thread safe ring buffer
    template <typename T>
    class RingBuffer
    {
    public:
        RingBuffer();
        RingBuffer(u32 cap);

        ~RingBuffer();

        void initialize(u32 cap);
        bool put(T&& item);

        T* get();
        T* check();

    private:
        T* m_data = nullptr;

        a_u32 m_get_pos;
        a_u32 m_put_pos;

        rsl::atomic<size_t> m_capacity;
    };

    //-------------------------------------------------------------------------
    template <typename T>
    RingBuffer<T>::RingBuffer()
        :m_data(nullptr)
    {
        initialize(0);
    }

    //-------------------------------------------------------------------------
    template <typename T>
    RingBuffer<T>::RingBuffer(u32 cap)
        :m_data(nullptr)
    {
        initialize(cap);
    }

    //-------------------------------------------------------------------------
    template <typename T>
    RingBuffer<T>::~RingBuffer()
    {
        if (m_data != nullptr)
        {
            rex::memory_free(m_data);
        }
    }

    //-------------------------------------------------------------------------
    template <typename T>
    void RingBuffer<T>::initialize(u32 cap)
    {
        if (m_data != nullptr)
        {
            rex::memory_free(m_data);
        }

        m_get_pos = 0;
        m_put_pos = 0;
        m_capacity = cap;

        if (cap == 0)
        {
            return;
        }

        m_data = (T*)rex::memory_alloc(sizeof(T) * m_capacity.load());
        rsl::memset(m_data, 0x0, sizeof(T) * m_capacity.load());
    }

    //-------------------------------------------------------------------------
    template <typename T>
    bool RingBuffer<T>::put(T&& item)
    {
        m_data[m_put_pos] = rsl::move(item);
        m_put_pos = (m_put_pos + 1) % m_capacity;

        return true;
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