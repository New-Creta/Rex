#pragma once

#include "rex_directx/utility/directx_util.h"

#include "rex_engine/engine/types.h"

namespace rex
{
  namespace rhi
  {
    class Fence
    {
    public:
      Fence(const wrl::ComPtr<ID3D12Fence>& fence);

      // Increment the internal target value
      // and signal the command queue to increment the value 
      // on the gpu as well
      void inc(ID3D12CommandQueue* commandQueue);

      // wait for the cpu value and the gpu value to be the same
      // this is a blocking call
      void wait_for_val();

    private:
      wrl::ComPtr<ID3D12Fence> m_fence;
      s32 m_target_val;
    };
  }
}