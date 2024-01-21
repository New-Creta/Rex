#pragma once

#include "rex_engine/engine/types.h"
#include "rex_engine/diagnostics/assert.h"

#include "rex_std/memory.h"
#include "rex_std/bonus/utility/casting.h"

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

        void shutdown();

        bool put(const T& item);
        bool will_wrap_on_next_put() const;
        bool will_wrap_on_next_get() const;

        T* get();
        T* check();

    private:
        void initialize(u32 cap);

    private:
        rsl::unique_array<T> m_data = nullptr;

        a_u32 m_get_pos;
        a_u32 m_put_pos;

        a_u64 m_capacity;
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
        shutdown();
    }

    //-------------------------------------------------------------------------
    template <typename T>
    void RingBuffer<T>::initialize(u32 cap)
    {
        if (m_data)
        {
            m_data.reset();
        }

        m_get_pos = 0;
        m_put_pos = 0;
        m_capacity = cap;

        if (cap == 0)
        {
            return;
        }

        m_data = rsl::make_unique<T[]>(rsl::safe_numeric_cast<u32>(sizeof(T) * m_capacity.load()));
    }

    //-------------------------------------------------------------------------
    template <typename T>
    void RingBuffer<T>::shutdown()
    {
        if (m_data)
        {
            m_data.reset();
        }
    }

    //-------------------------------------------------------------------------
    template <typename T>
    bool RingBuffer<T>::put(const T& item)
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

    //-------------------------------------------------------------------------
    template <typename T>
    bool RingBuffer<T>::will_wrap_on_next_put() const
    {
        return (m_put_pos + 1) % m_capacity == 0;
    }

    //-------------------------------------------------------------------------
    template <typename T>
    bool RingBuffer<T>::will_wrap_on_next_get() const
    {
        return (m_get_pos + 1) % m_capacity == 0;
    }
}