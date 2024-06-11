#include "rex_directx/system/dx_command_queue.h"
#include "rex_directx/diagnostics/dx_call.h"

#include "rex_directx/system/dx_render_context.h"
#include "rex_directx/system/dx_compute_context.h"
#include "rex_directx/system/dx_copy_context.h"

namespace rex
{
  namespace rhi
  {
    DxCommandQueue::DxCommandQueue(CommandType type, const wrl::ComPtr<ID3D12CommandQueue>& DxCommandQueue, rsl::unique_ptr<DxFence> fence)
      : CommandQueue(type)
      , m_command_queue(DxCommandQueue)
      , m_fence(rsl::move(fence))
      , m_fence_event()
    {
    }

    DxCommandQueue::~DxCommandQueue()
    {
      flush();
    }

    // Halt the cpu until the fence value is reached
    void DxCommandQueue::cpu_wait(u64 fenceValue)
    {
      // If our fence value if higher or equal to the given fence value
      // we don't need to do anything
      //if (is_fence_completed(fenceValue))
      //{
      //  return;
      //}

      // If the fence isn't completed yet, we halt this thread until it is
      wait_for_fence(fenceValue);
    }
    // Halt the gpu until the fence value is reached
    void DxCommandQueue::gpu_wait(SyncInfo& sync_info)
    {
      ID3D12Fence* fence = static_cast<ID3D12Fence*>(sync_info.fence_object());
      u64 fence_gpu_val = fence->GetCompletedValue();
      m_command_queue->Wait(fence, sync_info.fence_val());
    }

    bool DxCommandQueue::is_fence_completed(u64 fenceValue) const
    {
      u64 gpu_fence_value = m_fence->get()->GetCompletedValue();
      return fenceValue <= gpu_fence_value;
    }

    ScopedPoolObject<SyncInfo> DxCommandQueue::execute_context(GraphicsContext* ctx)
    {
      ID3D12GraphicsCommandList* cmdlist = cmdlist_from_ctx(ctx);

      cmdlist->Close();
      ID3D12CommandList* base_cmdlist = cmdlist;
      m_command_queue->ExecuteCommandLists(1, &base_cmdlist);
      m_command_queue->Signal(m_fence->get(), next_fence_value());

      u64 old_fence_val = inc_fence();
      wait_for_fence(old_fence_val);
      return create_sync_info(old_fence_val, m_fence->get());
    }

    u64 DxCommandQueue::gpu_fence_value() const
    {
      return m_fence->get()->GetCompletedValue();
    }

    ID3D12CommandQueue* DxCommandQueue::dx_object()
    {
      return m_command_queue.Get();
    }

    void DxCommandQueue::wait_for_fence(u64 fenceValue)
    {
      //if (m_fence->get()->GetCompletedValue() < fenceValue)
      //{
        DX_CALL(m_fence->get()->SetEventOnCompletion(fenceValue, m_fence_event.get()));
        m_fence_event.wait_for_me();
      //}
    }

    ID3D12GraphicsCommandList* DxCommandQueue::cmdlist_from_ctx(GraphicsContext* ctx) const
    {
      REX_ASSERT_X(ctx->type() == type(), "Trying to execute a context on a command queue that doesn't match its type. ctx: {} queue: {}", rsl::enum_refl::enum_name(ctx->type()), rsl::enum_refl::enum_name(type()));
      switch (type())
      {
      case CommandType::Render:   return static_cast<DxRenderContext*>(ctx)->dx_cmdlist();
      case CommandType::Copy:     return static_cast<DxCopyContext*>(ctx)->dx_cmdlist();
      case CommandType::Compute:  return static_cast<DxComputeContext*>(ctx)->dx_cmdlist();
      }

      REX_ASSERT("Unknown command type used for command queue. Cannot get command list of context");
      return nullptr;
    }

  } // namespace rhi
} // namespace rex