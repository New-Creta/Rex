#pragma once

#include <rex_std/atomic.h>

namespace rex
{
    class SpinLock
    {
    public:
        SpinLock();

        void lock();
        void unlock();

    private:
        rsl::atomic_flag m_flag;
    };
}