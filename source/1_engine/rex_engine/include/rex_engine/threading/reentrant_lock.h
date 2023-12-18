#pragma once

#include "rex_engine/types.h"
#include "rex_engine/diagnostics/assert.h"

#include <rex_std/atomic.h>
#include <rex_std/thread.h>

#include <thread>

namespace rsl
{
    inline namespace v1
    {
        template <>
        struct hash<rsl::thread::id>
        {
            rsl::hash_result operator()(const rsl::thread::id& tid) const
            {
                card64 seed = 0;

                seed = internal::hash_combine(seed, tid.m_id);

                return static_cast<rsl::hash_result>(seed);
            }
        };
    } // namespace v1
} // namespace rsl

namespace rex
{
    template<typename TRefCount>
    class BaseReentrantLock
    {
    public:
        BaseReentrantLock();

        bool try_lock();

        void lock();
        void unlock();

    private:
        rsl::atomic<u64> m_atomic;
        TRefCount        m_ref_count;
    };

    //-------------------------------------------------------------------------
    template<typename TRefCount>
    BaseReentrantLock<TRefCount>::BaseReentrantLock()
        :m_atomic(0)
        ,m_ref_count(0)
    {}

    //-------------------------------------------------------------------------
    template<typename TRefCount>
    bool BaseReentrantLock<TRefCount>::try_lock()
    {
        rsl::hash<rsl::thread::id> hasher;
        auto tid = hasher(rsl::this_thread::get_id());

        bool aquired = false;

        if (m_atomic.load(rsl::memory_order_relaxed) == tid)
        {
            acquired = true;
        }
        else
        {
            u64 unlock_value = 0;
            acquired = m_atomic.compare_exchange_strong(unlock_value, tid, rsl::memory_order_relaxed, rsl::memory_order_relaxed);
        }

        if (aquired)
        {
            ++m_ref_count;
            rsl::atomic_thread_fence(rsl::memory_order_acquire);
        }

        return aquired;
    }

    //-------------------------------------------------------------------------
    template<typename TRefCount>
    void BaseReentrantLock<TRefCount>::lock()
    {
        rsl::hash<rsl::thread::id> hasher;
        auto tid = hasher(rsl::this_thread::get_id());

        // if this thread doesn't already hold the lock ...
        if (m_atomic.load(rsl::memory_order_relaxed) != tid)
        {
            // ... spin wait until we hold it
            u64 unlock_value = 0;
            while (!m_atomic.compare_exchange_weak(unlock_value, tid, rsl::memory_order_relaxed, rsl::memory_order_relaxed))
            {
                unlock_value = 0;
                rsl::this_thread::yield();
            }
        }

        // increment reference count so we can verify that
        // lock() and unlock() are called in pairs

        ++m_ref_count;

        // use an acquire fence to ensure all subsequent
        // reads by this thread will be valid
        rsl::atomic_thread_fence(rsl::memory_order_acquire);
    }

    //-------------------------------------------------------------------------
    template<typename TRefCount>
    void BaseReentrantLock<TRefCount>::unlock()
    {
        // use release semantics to ensure that all prior
        // writes have been fully committed before unlock
        rsl::atomic_thread_fence(rsl::memory_order_release);

        rsl::hash<rsl::thread::id> hasher;
        auto tid = hasher(rsl::this_thread::get_id());
        auto actual = m_atomic.load(rsl::memory_order_relaxed);

        REX_ASSERT_X(actual == tid, "This thread id is expected to be equal to the stored thread id when unlocking");

        --m_ref_count;
        if (m_ref_count == 0)
        {
            // release lock, which is safe because we own it
            m_atomic.store(0, rsl::memory_order_relaxed);
        }
    }

    using ReentrantLock32 = BaseReentrantLock<s32>;
    using ReentrantLock64 = BaseReentrantLock<s64>;

    using ReentrantLock = ReentrantLock32;
}