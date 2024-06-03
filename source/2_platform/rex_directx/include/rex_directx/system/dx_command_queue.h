#pragma once

#include "rex_directx/system/dx_commandlist.h"
#include "rex_directx/system/dx_fence.h"
#include "rex_directx/utility/dx_util.h"
#include "rex_engine/engine/types.h"
#include "rex_engine/threading/thread_event.h"

#include "rex_renderer_core/system/command_queue.h"
#include "rex_renderer_core/rhi/graphics_context.h"

namespace rex
{
  namespace rhi
  {
    class DxCommandQueue : public CommandQueue
    {
    public:
      DxCommandQueue(CommandType type, const wrl::ComPtr<ID3D12CommandQueue>& commandQueue, rsl::unique_ptr<DxFence> fence);
      DxCommandQueue(const CommandQueue&) = delete;
      DxCommandQueue(CommandQueue&&) = delete;
      ~DxCommandQueue();

      DxCommandQueue& operator=(const DxCommandQueue&) = delete;
      DxCommandQueue& operator=(DxCommandQueue&&) = delete;

      // Halt the cpu until the fence value is reached
      // This is often used to block cpu from adding more commands
      // Which gives the gpu some time to catch up
      void cpu_wait(u64 fenceValue) override;
      // Halt the gpu until the fence value is reached
      // Example: This is so the compute queue can wait for the copy queue to have finished.
      void gpu_wait(SyncInfo& sync_info) override;
      // Returns if the last completed fence is equal or lower than the given fence value
      // Meaning that the commands before the given fence value got signaled have executed
      bool is_fence_completed(u64 fenceValue) const override;

      ScopedPoolObject<SyncInfo> execute_context(GraphicsContext* ctx) override;

      ID3D12CommandQueue* dx_object();

    private:
      // Halts this thread until the internal fence has reached or surpassed the given fence value
      void wait_for_fence(u64 fenceValue);

      ID3D12GraphicsCommandList* cmdlist_from_ctx(GraphicsContext* ctx) const;

    private:
      wrl::ComPtr<ID3D12CommandQueue> m_command_queue;
      rsl::unique_ptr<DxFence> m_fence;
      Event m_fence_event;
      //Fence m_fence;
    };
  } // namespace rhi
} // namespace rex