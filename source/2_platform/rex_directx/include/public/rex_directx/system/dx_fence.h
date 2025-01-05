#pragma once

#include "rex_directx/utility/dx_util.h"
#include "rex_engine/engine/types.h"
#include "rex_engine/gfx/system/fence.h"
#include "rex_engine/threading/thread_event.h"

namespace rex
{
  namespace gfx
  {
    class DxFence : public Fence
    {
    public:
      explicit DxFence(const wrl::ComPtr<ID3D12Fence>& fence);

      // Return the wrapped fence object
      ID3D12Fence* dx_object() const;

      // Get the fence value on the gpu
      u64 gpu_value() const override;
      // Incrase the fence value, both on the cpu and the gpu
      u64 inc(CommandQueue* cmdQueue) override;
      // Wait for the cpu value to be in sync with the gpu value
      void wait_for_me() override;

    private:
      wrl::ComPtr<ID3D12Fence> m_fence;
      ThreadEvent m_sync_object;
    };
  } // namespace gfx
} // namespace rex