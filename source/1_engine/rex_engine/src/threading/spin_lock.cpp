#include "rex_engine/threading/spin_lock.h"

namespace rex
{
    //-------------------------------------------------------------------------
    SpinLock::SpinLock() 
        : m_flag(ATOMIC_FLAG_INIT) 
    {}

    //-------------------------------------------------------------------------
    void SpinLock::lock()
    {
        while (m_flag.test_and_set(rsl::memory_order_acquire)) {}
    }
    //-------------------------------------------------------------------------
    void SpinLock::unlock()
    {
        m_flag.clear(rsl::memory_order_release);
    }
}