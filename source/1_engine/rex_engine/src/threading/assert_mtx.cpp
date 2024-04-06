#include "rex_engine/threading/assert_mtx.h"

#include "rex_engine/diagnostics/assert.h"

namespace rex
{
  AssertMtx::AssertMtx()
      : m_is_locked(false)
  {
  }

  bool AssertMtx::try_lock()
  {
    lock();
    return true;
  }

  void AssertMtx::lock()
  {
    REX_ASSERT_X(!m_is_locked, "Assert lock was already locked.");
    m_is_locked = true;
  }

  void AssertMtx::unlock()
  {
    REX_ASSERT_X(m_is_locked, "Assert lock was unlocked when trying to get locked");
    m_is_locked = false;
  }

} // namespace rex