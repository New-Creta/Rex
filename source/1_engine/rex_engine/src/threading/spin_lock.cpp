#include "rex_engine/threading/spin_lock.h"

#include <rex_std/thread.h>

namespace rex
{
    //-------------------------------------------------------------------------
    SpinLock::SpinLock() 
        : m_flag(false) 
    {}

    //-------------------------------------------------------------------------
    bool SpinLock::try_lock()
    {
        // use an acquire fence to ensure all subsequent
        // reads by this thread will be valid
        bool already_locked = m_flag.test_and_set(rsl::memory_order_acquire);
        
        return !already_locked;
    }

    //-------------------------------------------------------------------------
    void SpinLock::lock()
    {
        // spin until successful lock
        while (!try_lock())
        {
            // reduce power consumption
            rsl::this_thread::yield();
        }
    }

    //-------------------------------------------------------------------------
    void SpinLock::unlock()
    {
        // use release semantics to ensure that all prior
        // writes have been fully comitted before unlock
        m_flag.clear(rsl::memory_order_release);
    }
}