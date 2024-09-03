#include "rex_engine/gfx/system/command_queue.h"

namespace rex
{
  namespace gfx
  {
    CommandQueue::CommandQueue(GraphicsEngineType type)
      : m_type(type)
    {

    }

    bool CommandQueue::is_fence_completed(u64 fenceValue) const
    {
      u64 fence_value_on_gpu = gpu_fence_value();
      return fence_value_on_gpu >= fenceValue;
    }

    u64 CommandQueue::last_completed_fence() const
    {
      return gpu_fence_value();
    }

    GraphicsEngineType CommandQueue::type() const
    {
      return m_type;
    }

    void CommandQueue::flush()
    {
      cpu_wait();
    }

    ScopedPoolObject<SyncInfo> CommandQueue::create_sync_info(u64 fenceValue, Fence* fenceObject)
    {
      return m_sync_info_pool.request(fenceValue, fenceObject);
    }

  }
}