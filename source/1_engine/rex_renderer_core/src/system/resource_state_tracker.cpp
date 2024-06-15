#include "rex_renderer_core/system/resource_state_tracker.h"

namespace rex
{
  namespace gfx
  {
    ResourceStateTracker::ResourceStateTracker(ResourceStateTracker* parentResourceStateTracker)
      : m_parent_resource_state_tracker(parentResourceStateTracker)
    {
    }

    // Track a resource state internally
    ResourceStateTransition ResourceStateTracker::track_resource_transition(Resource* resource, ResourceState state)
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

    // Get the current resource state of a resource
    ResourceState ResourceStateTracker::current_resource_state(Resource* resource) const
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

    // Update the parent with out resource states
    void ResourceStateTracker::update_parent()
    {
      if (m_parent_resource_state_tracker)
      {
        for (auto& transition : m_states_per_resource)
        {
          Resource* resource = transition.key;
          ResourceState resource_state = transition.value.back();

          m_parent_resource_state_tracker->track_resource_transition(resource, resource_state);
        }

        m_parent_resource_state_tracker->update_parent();
      }
    }
    // Clear our resource states, all trackings are cleared
    void ResourceStateTracker::clear()
    {
      m_states_per_resource.clear();
    }
  }
}