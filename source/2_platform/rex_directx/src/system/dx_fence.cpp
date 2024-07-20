#include "rex_directx/system/dx_fence.h"

#include "rex_directx/diagnostics/dx_call.h"
#include "rex_engine/threading/thread_event.h"
#include "rex_directx/system/dx_command_queue.h"

namespace rex
{
  namespace gfx
  {
    DxFence::DxFence(const wrl::ComPtr<ID3D12Fence>& fence)
        : m_fence(fence)
    {
    }

    ID3D12Fence* DxFence::dx_object() const
    {
      return m_fence.Get();
    }

    u64 DxFence::gpu_value() const
    {
      return m_fence->GetCompletedValue();
    }
    u64 DxFence::inc(CommandQueue* cmdQueue)
    {
      inc_cpu_val();
      
      ID3D12CommandQueue* dx_cmd_queue = d3d::to_dx12(cmdQueue)->dx_object();
      dx_cmd_queue->Signal(m_fence.Get(), cpu_value());

      return cpu_value();
    }
    void DxFence::wait_for_me()
    {
      if (m_fence->GetCompletedValue() < cpu_value())
      {
        m_fence->SetEventOnCompletion(cpu_value(), m_sync_object.get());
        m_sync_object.wait_for_me();
      }
    }

  } // namespace gfx
} // namespace rex