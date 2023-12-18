#pragma once

namespace rex
{
    template <typename TLock>
    class ScopedLock
    {
    public:
        explicit ScopedLock(TLock& lock);
        ~ScopedLock();

    private:
        TLock* m_lock;
    };

    //-------------------------------------------------------------------------
    template <typename TLock>
    ScopedLock<TLock>::ScopedLock(TLock& lock)
        :m_lock(&lock)
    {
        m_lock->lock();
    }

    //-------------------------------------------------------------------------
    template <typename TLock>
    ScopedLock<TLock>::~ScopedLock()
    {
        m_lock->unlock();
    }
}