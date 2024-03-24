#pragma once

#include "rex_directx/utility/directx_util.h"

#include "rex_directx/system/directx_fence.h"

#include "rex_engine/engine/types.h"

namespace rex
{
  namespace rhi
  {
    class CommandList;

    class CommandQueue
    {
    public:
      CommandQueue(const wrl::ComPtr<ID3D12CommandQueue>& commandQueue, const wrl::ComPtr<ID3D12Fence>& fence);
      ~CommandQueue();

      void inc_fence();
      void flush();
      void execute(ID3D12CommandList* commandlist);

      ID3D12CommandQueue* get();

    private:
      wrl::ComPtr<ID3D12CommandQueue> m_command_queue;
      Fence m_fence;
    };
  }
}