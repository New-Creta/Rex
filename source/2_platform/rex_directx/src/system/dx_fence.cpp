#include "rex_directx/system/dx_fence.h"

#include "rex_directx/diagnostics/dx_call.h"
#include "rex_engine/threading/thread_event.h"

namespace rex
{
  namespace rhi
  {
    DxFence::DxFence(const wrl::ComPtr<ID3D12Fence>& fence)
        : m_fence(fence)
        , m_target_val(0)
    {
    }

    void DxFence::inc(ID3D12CommandQueue* commandQueue)
    {
      ++m_target_val;
      commandQueue->Signal(m_fence.Get(), m_target_val);
    }

    void DxFence::wait_for_val()
    {
      if(m_fence->GetCompletedValue() != m_target_val)
      {
        ThreadEvent evt;
        DX_CALL(m_fence->SetEventOnCompletion(m_target_val, evt.get()));
        evt.wait_for_me();
      }
    }

    ID3D12Fence* DxFence::get() const
    {
      return m_fence.Get();
    }
    s32 DxFence::target_value() const
    {
      return m_target_val;
    }

  } // namespace rhi
} // namespace rex