#include "rex_directx/system/dx_resource_state_tracker.h"

namespace rex
{
  namespace rhi
  {
    ResourceStateTransition ResourceStateTracker::track_resource_transition(Resource2* resource, ResourceState state)
    {
      ResourceStateTransition transition{};
      transition.after = state;
      transition.before = current_resource_state(resource);

      if (!m_states_per_resource.contains(resource))
      {
        m_states_per_resource.emplace(resource, rsl::vector<ResourceState>{});
      }
      m_states_per_resource.at(resource).push_back(state);

      return transition;
    }

    ResourceState ResourceStateTracker::current_resource_state(Resource2* resource) const
    {
      ResourceState resource_state{};

      if (m_states_per_resource.contains(resource))
      {
        resource_state = m_states_per_resource.at(resource).back();
      }
      else if (m_parent_resource_state_tracker)
      {
        resource_state = m_parent_resource_state_tracker->current_resource_state(resource);
      }
      else
      {
        resource_state = ResourceState::Common;
      }

      return resource_state;
    }

    void ResourceStateTracker::update_parent()
    {
      if (m_parent_resource_state_tracker)
      {
        // Update parent so it knows the latest resource state of a resource
      }
    }
    void ResourceStateTracker::clear()
    {
      m_states_per_resource.clear();
    }
  }
}