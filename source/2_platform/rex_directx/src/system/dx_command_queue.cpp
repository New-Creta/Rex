#include "rex_directx/system/dx_command_queue.h"

namespace rex
{
  namespace rhi
  {
    CommandQueue::CommandQueue(const wrl::ComPtr<ID3D12CommandQueue>& commandQueue, const wrl::ComPtr<ID3D12Fence>& fence)
        : m_command_queue(commandQueue)
        , m_fence(fence)
    {
    }

    CommandQueue::~CommandQueue()
    {
      flush();
    }

    void CommandQueue::inc_fence()
    {
      m_fence.inc(m_command_queue.Get());
    }

    void CommandQueue::flush()
    {
      inc_fence();
      m_fence.wait_for_val();
    }

    void CommandQueue::execute(ID3D12CommandList* commandlist)
    {
      m_command_queue->ExecuteCommandLists(1, &commandlist);
    }

    void CommandQueue::wait()
    {
      m_command_queue->Wait(m_fence.get(), m_fence.target_value());
    }

    bool CommandQueue::has_reached_latest_fence() const
    {
      return fence_value() < m_fence.target_value();
    }

    s32 CommandQueue::fence_value() const
    {
      return m_fence.get()->GetCompletedValue();
    }

    ID3D12CommandQueue* CommandQueue::get()
    {
      return m_command_queue.Get();
    }

  } // namespace rhi
} // namespace rex