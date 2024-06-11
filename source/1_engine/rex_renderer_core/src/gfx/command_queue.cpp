#include "rex_renderer_core/system/command_queue.h"

namespace rex
{
  namespace rhi
  {
    CommandQueue::CommandQueue(CommandType type)
      : m_next_fence_value(1)
      , m_type(type)
    {

    }

    u64 CommandQueue::next_fence_value() const
    {
      return m_next_fence_value;
    }

    u64 CommandQueue::inc_fence()
    {
      return m_next_fence_value++;
    }

    u64 CommandQueue::last_completed_fence() const
    {
      return gpu_fence_value();
    }

    CommandType CommandQueue::type() const
    {
      return m_type;
    }

    void CommandQueue::flush()
    {
      cpu_wait(next_fence_value() - 1);
    }

    ScopedPoolObject<SyncInfo> CommandQueue::create_sync_info(u64 fenceValue, void* fenceObject)
    {
      return m_sync_info_pool.request(fenceValue, fenceObject);
    }

  }
}