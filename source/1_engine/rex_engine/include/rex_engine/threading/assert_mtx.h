#pragma once

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
    // This doesn't need to be an atomic
    // A volatile is good enough
    // see Game Engine Architecture Third Edition (page 326)
    volatile bool m_is_locked;
  };
} // namespace rex