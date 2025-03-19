#pragma once

#include "rex_std/atomic.h"

namespace rex
{
  class SpinLock
  {
  public:
    SpinLock();

    // Tries to lock the lock
    // returns true on success
    bool try_lock();

    void lock();
    void unlock();

  private:
    rsl::atomic_flag m_flag;
  };
} // namespace rex