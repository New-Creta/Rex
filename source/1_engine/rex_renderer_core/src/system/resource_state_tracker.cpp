#include "rex_renderer_core/system/resource_state_tracker.h"

namespace rex
{
  namespace rhi
  {
    rsl::unique_ptr<ResourceStateTracker> ResourceStateTracker::s_global_resource_state_tracker = nullptr;

    ResourceStateTracker::ResourceStateTracker(ResourceStateTracker* parentResourceStateTracker)
      : m_parent_resource_state_tracker(parentResourceStateTracker)
    {
      if (!m_parent_resource_state_tracker)
      {
        if (!s_global_resource_state_tracker)
        {
          init_global();
        }

        m_parent_resource_state_tracker = s_global_resource_state_tracker.get();
      }
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
      }
    }
    // Clear our resource states, all trackings are cleared
    void ResourceStateTracker::clear()
    {
      m_states_per_resource.clear();
    }

    // The resource state tracker accepting the private handle
    // This means it doesn't have a parent tracker
    ResourceStateTracker::ResourceStateTracker(const PrivateHandle&)
      : m_parent_resource_state_tracker()
    {}

    // init the global resource state tracker who holds final resource tracking states
    void ResourceStateTracker::init_global()
    {
      PrivateHandle h{};
      s_global_resource_state_tracker = rsl::make_unique<ResourceStateTracker>(h);
    }

  }
}