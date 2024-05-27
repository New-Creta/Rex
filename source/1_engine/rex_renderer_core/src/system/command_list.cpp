#include "rex_renderer_core/system/command_list.h"

namespace rex
{
  namespace rhi
  {
    CommandList::CommandList(rhi::CommandType type, rhi::ResourceStateTracker* resourceStateTracker)
      : m_resource_state_tracker(resourceStateTracker)
      , m_type(type)
    {}

    rhi::CommandType CommandList::type() const
    {
      return m_type;
    }
  }
}