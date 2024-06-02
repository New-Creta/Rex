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
      return m_next_fence_value - 1;
    }

    CommandType CommandQueue::type() const
    {
      return m_type;
    }

  }
}