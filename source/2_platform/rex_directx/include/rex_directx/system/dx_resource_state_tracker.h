//#pragma once
//
//#include "rex_std/unordered_map.h"
//#include "rex_std/vector.h"
//
//#include "rex_renderer_core/rhi/resource_state.h"
//#include "rex_directx/system/dx_resource.h"
//
//namespace rex
//{
//  namespace rhi
//  {
//    struct ResourceStateTransition
//    {
//      ResourceState before;
//      ResourceState after;
//    };
//
//    class ResourceStateTracker
//    {
//    public:
//      explicit ResourceStateTracker(ResourceStateTracker* parentResourceStateTracker = nullptr);
//      ResourceStateTransition track_resource_transition(Resource2* resource, ResourceState state);
//
//      ResourceState current_resource_state(Resource2* resource) const;
//
//      void update_parent();
//      void clear();
//
//    private:
//      rsl::unordered_map<Resource2*, rsl::vector<ResourceState>> m_states_per_resource;
//      ResourceStateTracker* m_parent_resource_state_tracker;
//    };
//  }
//}