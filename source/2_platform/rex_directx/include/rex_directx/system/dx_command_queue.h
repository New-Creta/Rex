#pragma once

#include "rex_directx/system/dx_fence.h"
#include "rex_directx/utility/dx_util.h"
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
      CommandQueue(const CommandQueue&) = delete;
      CommandQueue(CommandQueue&&) = delete;
      ~CommandQueue();

      CommandQueue& operator=(const CommandQueue&) = delete;
      CommandQueue& operator=(CommandQueue&&) = delete;

      void inc_fence();
      void flush();
      void execute(ID3D12CommandList* commandlist);
      void wait(s32 val);

      ID3D12CommandQueue* get();
      s32 fence_value();

    private:
      wrl::ComPtr<ID3D12CommandQueue> m_command_queue;
      Fence m_fence;
    };
  } // namespace rhi
} // namespace rex