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

    u64 SyncInfo::fence_val() const
    {
      return m_fence_value;
    }
    void* SyncInfo::fence_object()
    {
      return m_fence_object;
    }

    void SyncInfo::reset(u64 fenceValue, void* fenceObject)
    {
      m_fence_value = fenceValue;
      m_fence_object = fenceObject;
    }

  }
}