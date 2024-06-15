#include "rex_directx/system/dx_fence.h"

#include "rex_directx/diagnostics/dx_call.h"
#include "rex_engine/threading/thread_event.h"

namespace rex
{
  namespace gfx
  {
    DxFence::DxFence(const wrl::ComPtr<ID3D12Fence>& fence)
        : m_fence(fence)
    {
    }

    ID3D12Fence* DxFence::get() const
    {
      return m_fence.Get();
    }

  } // namespace gfx
} // namespace rex