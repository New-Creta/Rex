#include "rex_renderer_core/gfx/sync_info.h"

namespace rex
{
  namespace rhi
  {
    SyncInfo::SyncInfo()
      : m_fence_value()
      , m_fence_object()
    {
    }

    // Return the fence value the fence needs to have on the gpu
    // In order to have the commands that were queued before the fence was set
    // to be executed
    u64 SyncInfo::fence_val() const
    {
      return m_fence_value;
    }
    // Return the fence object itself that needs to be checked for the fence value
    Fence* SyncInfo::fence_object()
    {
      return m_fence_object;
    }

    // Reset the object with a new fence value and fence object
    void SyncInfo::reset(u64 fenceValue, Fence* fenceObject)
    {
      m_fence_value = fenceValue;
      m_fence_object = fenceObject;
    }

  }
}