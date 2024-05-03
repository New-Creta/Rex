#include "rex_directx/system/dx_fence.h"

#include "rex_directx/diagnostics/dx_call.h"
#include "rex_engine/threading/thread_event.h"

namespace rex
{
  namespace rhi
  {
    Fence::Fence(const wrl::ComPtr<ID3D12Fence>& fence)
        : m_fence(fence)
        , m_target_val(0)
    {
    }

    void Fence::inc(ID3D12CommandQueue* commandQueue)
    {
      ++m_target_val;
      commandQueue->Signal(m_fence.Get(), m_target_val);
    }

    void Fence::wait_for_val()
    {
      if(m_fence->GetCompletedValue() != m_target_val)
      {
        Event evt;
        DX_CALL(m_fence->SetEventOnCompletion(m_target_val, evt.get()));
        evt.wait_for_me();
      }
    }

    ID3D12Fence* Fence::get()
    {
      return m_fence.Get();
    }

  } // namespace rhi
} // namespace rex