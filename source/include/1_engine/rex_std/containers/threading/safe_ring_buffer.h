#pragma once

#include <thread>
#include <mutex>

namespace rex
{
    template <typename T, size_t capacity>
    class safe_ringbuffer
    {
    public:
        bool push_back(const T& item);
        bool pop_front(T& item);

        bool front(T& item) const;
        bool back(T& item) const;

        T* data() const;

        size_t head_index() const;
        size_t tail_index() const;

        size_t size() const;

        bool is_empty() const;

    private:
        T m_buffer[capacity];

        size_t m_head = 0;
        size_t m_tail = 0;

        std::mutex m_lock;
    };

    //-------------------------------------------------------------------------
    template <typename T, size_t capacity>
    bool rex::safe_ringbuffer<T, capacity>::push_back(const T& item)
    {
        bool result = false;

        m_lock.lock();

        size_t next = (m_head + 1) % capacity;

        if (next != m_tail)
        {
            m_buffer[m_head] = item;
            m_head = next;
            result = true;
        }

        m_lock.unlock();

        return result;
    }
    //-------------------------------------------------------------------------
    template <typename T, size_t capacity>
    bool rex::safe_ringbuffer<T, capacity>::pop_front(T& item)
    {
        bool result = false;

        m_lock.lock();

        if (m_tail != m_head)
        {
            item = m_buffer[m_tail];
            m_tail = (m_tail + 1) % capacity;
            result = true;
        }

        m_lock.unlock();

        return result;
    }

    //-------------------------------------------------------------------------
    template <typename T, size_t capacity>
    bool rex::safe_ringbuffer<T, capacity>::front(T& item) const
    {
        if (size() == 0)
            return false;

        item = m_buffer[m_head];
        return true;
    }
    //-------------------------------------------------------------------------
    template <typename T, size_t capacity>
    bool rex::safe_ringbuffer<T, capacity>::back(T& item) const
    {
        if (size() == 0)
            return false;

        item = m_buffer[m_tail];
        return true;
    }

    //-------------------------------------------------------------------------
    template <typename T, size_t capacity>
    T* rex::safe_ringbuffer<T, capacity>::data() const
    {
        return &m_buffer[0];
    }

    //-------------------------------------------------------------------------
    template <typename T, size_t capacity>
    size_t rex::safe_ringbuffer<T, capacity>::head_index() const
    {
        return m_head;
    }
    //-------------------------------------------------------------------------
    template <typename T, size_t capacity>
    size_t rex::safe_ringbuffer<T, capacity>::tail_index() const
    {
        return m_tail;
    }

    //-------------------------------------------------------------------------
    template <typename T, size_t capacity>
    size_t rex::safe_ringbuffer<T, capacity>::size() const
    {
        size_t size = m_tail - m_head;
        if (size < 0)
        {
            size = (-1) * size;
        }

        return static_cast<size_t>(size);
    }

    //-------------------------------------------------------------------------
    template <typename T, size_t capacity>
    bool rex::safe_ringbuffer<T, capacity>::is_empty() const
    {
        return size() == 0;
    }
}