#pragma once

#include "rex_directx/system/dx_fence.h"
#include "rex_directx/utility/dx_util.h"
#include "rex_engine/engine/types.h"
#include "rex_engine/threading/thread_event.h"

#include "rex_renderer_core/system/command_queue.h"
#include "rex_renderer_core/gfx/graphics_context.h"

namespace rex
{
  namespace gfx
  {
    class DxCommandQueue : public CommandQueue
    {
    public:
      DxCommandQueue(GraphicsEngineType type, const wrl::ComPtr<ID3D12CommandQueue>& commandQueue, rsl::unique_ptr<DxFence> fence);
      DxCommandQueue(const CommandQueue&) = delete;
      DxCommandQueue(CommandQueue&&) = delete;
      ~DxCommandQueue();

      DxCommandQueue& operator=(const DxCommandQueue&) = delete;
      DxCommandQueue& operator=(DxCommandQueue&&) = delete;

      // Halt the cpu until the fence value is reached
      // This is often used to block cpu from adding more commands
      // Which gives the gpu some time to catch up
      void cpu_wait() override;
      // Halt the gpu until the fence value is reached
      // Example: This is so the compute queue can wait for the copy queue to have finished.
      void gpu_wait(SyncInfo& sync_info) override;

      // Execute a context on this command queue
      // Returning a sync object that can be used to make sure
      // another context cannot execute until this context has finished executing
      ScopedPoolObject<SyncInfo> execute_context(GraphicsContext* ctx, WaitForFinish waitForFinish) override;

      // Return the wrapped directx object
      ID3D12CommandQueue* dx_object();

    protected:
      // Return the value of our fence on the gpu
      u64 gpu_fence_value() const;

    private:
      // Retrieve the DirectX commandlist from a context object
      ID3D12GraphicsCommandList* cmdlist_from_ctx(GraphicsContext* ctx) const;

    private:
      wrl::ComPtr<ID3D12CommandQueue> m_command_queue;
      rsl::unique_ptr<DxFence> m_fence;
      ThreadEvent m_fence_event;
    };
  } // namespace gfx
} // namespace rex