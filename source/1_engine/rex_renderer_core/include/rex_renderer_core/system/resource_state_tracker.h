#pragma once

#include "rex_std/unordered_map.h"
#include "rex_std/vector.h"

#include "rex_renderer_core/rhi/resource_state.h"
#include "rex_renderer_core/resources/resource.h"

#include "rex_engine/engine/types.h"

namespace rex
{
  namespace rhi
  {
    class Resource;

    // Describes the resource transition taking place, with the before and after state
    struct ResourceStateTransition
    {
      ResourceState before;
      ResourceState after;
    };

    // Each context has a resource tracker to track the transitions of resources that are changed by the context
    class ResourceStateTracker
    {
    public:
      explicit ResourceStateTracker(ResourceStateTracker* parentResourceStateTracker = nullptr);

      // Track a resource state internally
      ResourceStateTransition track_resource_transition(Resource* resource, ResourceState state);

      // Get the current resource state of a resource
      ResourceState current_resource_state(Resource* resource) const;

      // Update the parent with out resource states
      void update_parent();
      // Clear our resource states, all trackings are cleared
      void clear();

    private:
      rsl::unordered_map<Resource*, rsl::vector<ResourceState>> m_states_per_resource;
      ResourceStateTracker* m_parent_resource_state_tracker;
    };
  }
}