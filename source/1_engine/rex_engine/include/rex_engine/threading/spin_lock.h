#pragma once

#include <rex_std/atomic.h>

namespace rex
{
    class SpinLock
    {
    public:
        SpinLock();

        bool try_lock();

        void lock();
        void unlock();

    private:
        rsl::atomic_flag m_flag;
    };
}