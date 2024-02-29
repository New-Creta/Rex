#pragma once

#include "rex_std/atomic.h"

namespace rex
{
  // A fake mutex that doesn't actually lock
  // Instead it raises an assert if a lock is requested
  // while it's already held by another thread
  class AssertMtx
  {
  public:
    AssertMtx();

    bool try_lock();

    void lock();
    void unlock();

  private:
    rsl::atomic<bool> m_is_locked;
  };
}