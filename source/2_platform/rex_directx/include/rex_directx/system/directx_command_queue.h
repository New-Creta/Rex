#pragma once

#include "rex_directx/utility/directx_util.h"

#include "rex_engine/engine/types.h"

namespace rex
{
  namespace rhi
  {
    class CommandQueue
    {
    public:
      CommandQueue(const wrl::ComPtr<ID3D12CommandQueue>& commandQueue);

    private:
      s32 m_current_fence_value;

      wrl::ComPtr<ID3D12CommandQueue> m_command_queue;
    };
  }
}