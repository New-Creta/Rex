#include "rex_directx/resources/resource_state_tracker.h"
#include "rex_directx/resources/resource.h"
#include "rex_directx/command_list.h"

#include "rex_engine/diagnostics/assert.h"

namespace rex
{
  namespace renderer
  {
    // Static definitions.
    rsl::mutex                              ResourceStateTracker::s_global_mutex;
    bool                                    ResourceStateTracker::s_is_locked = false;
    ResourceStateTracker::ResourceStateMap  ResourceStateTracker::s_global_resource_state;

    ResourceStateTracker::ResourceStateTracker() = default;

    ResourceStateTracker::~ResourceStateTracker() = default;

    void ResourceStateTracker::resource_barrier(const D3D12_RESOURCE_BARRIER& barrier)
    {
      if(barrier.Type == D3D12_RESOURCE_BARRIER_TYPE_TRANSITION)
      {
        const D3D12_RESOURCE_TRANSITION_BARRIER& transition_barrier = barrier.Transition;

        // First check if there is already a known "final" state for the given resource.
        // If there is, the resource has been used on the command list before and
        // already has a known state within the command list execution.
        const auto iter = m_final_resource_state.find(transition_barrier.pResource);
        if(iter != m_final_resource_state.end())
        {
          auto& resource_state = iter->value;
          // If the known final state of the resource is different...
          if(transition_barrier.Subresource == D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES && !resource_state.subresource_state_map.empty())
          {
            // First transition all of the subresources if they are different than the StateAfter.
            for(auto subresource_state: resource_state.subresource_state_map)
            {
              if(transition_barrier.StateAfter != subresource_state.value)
              {
                D3D12_RESOURCE_BARRIER new_barrier = barrier;
                new_barrier.Transition.Subresource = subresource_state.key;
                new_barrier.Transition.StateBefore = subresource_state.value;
                m_resource_barriers.push_back(new_barrier);
              }
            }
          }
          else
          {
            auto final_state = resource_state.subresource_state(transition_barrier.Subresource);
            if(transition_barrier.StateAfter != final_state)
            {
              // Push a new transition barrier with the correct before state.
              D3D12_RESOURCE_BARRIER new_barrier = barrier;
              new_barrier.Transition.StateBefore = final_state;
              m_resource_barriers.push_back(new_barrier);
            }
          }
        }
        else // In this case, the resource is being used on the command list for the first time.
        {
          // Add a pending barrier. The pending barriers will be resolved
          // before the command list is executed on the command queue.
          m_pending_resource_barriers.push_back(barrier);
        }

        // Push the final known state (possibly replacing the previously known state for the subresource).
        m_final_resource_state[transition_barrier.pResource].set_subresource_state(transition_barrier.Subresource, transition_barrier.StateAfter);
      }
      else
      {
        // Just push non-transition barriers to the resource barriers array.
        m_resource_barriers.push_back(barrier);
      }
    }

    void ResourceStateTracker::transition_resource(ID3D12Resource* resource, D3D12_RESOURCE_STATES stateAfter, u32 subResource)
    {
      if(resource)
      {
        resource_barrier(CD3DX12_RESOURCE_BARRIER::Transition(resource, D3D12_RESOURCE_STATE_COMMON, stateAfter, subResource));
      }
    }

    void ResourceStateTracker::transition_resource(const Resource& resource, D3D12_RESOURCE_STATES stateAfter, u32 subResource)
    {
      transition_resource(resource.d3d_resource().Get(), stateAfter, subResource);
    }

    void ResourceStateTracker::flush_resource_barriers(const rsl::shared_ptr<CommandList>& commandList)
    {
      u32 num_barriers = static_cast<u32>(m_resource_barriers.size());
      if(num_barriers > 0)
      {
        auto d3d_command_list = commandList->graphics_command_list();
        d3d_command_list->ResourceBarrier(num_barriers, m_resource_barriers.data());
        m_resource_barriers.clear();
      }
    }

    uint32_t ResourceStateTracker::flush_pending_resource_barriers(const rsl::shared_ptr<CommandList>& commandList)
    {
      REX_ASSERT_X(s_is_locked, "Resource State Tracker is not locked");

      // Resolve the pending resource barriers by checking the global state of the
      // (sub)resources. Add barriers if the pending state and the global state do
      //  not match.
      ResourceBarriers resource_barriers;
      // Reserve enough space (worst-case, all pending barriers).
      resource_barriers.reserve(m_pending_resource_barriers.size());

      for(auto pending_barrier: m_pending_resource_barriers)
      {
        if(pending_barrier.Type == D3D12_RESOURCE_BARRIER_TYPE_TRANSITION) // Only transition barriers should be pending...
        {
          auto pending_transition = pending_barrier.Transition;

          const auto& iter = s_global_resource_state.find(pending_transition.pResource);
          if(iter != s_global_resource_state.end())
          {
            // If all subresources are being transitioned, and there are multiple
            // subresources of the resource that are in a different state...
            auto& resource_state = iter->value;
            if(pending_transition.Subresource == D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES && !resource_state.subresource_state_map.empty())
            {
              // Transition all subresources
              for(auto subresource_state: resource_state.subresource_state_map)
              {
                if(pending_transition.StateAfter != subresource_state.value)
                {
                  D3D12_RESOURCE_BARRIER new_barrier = pending_barrier;
                  new_barrier.Transition.Subresource = subresource_state.key;
                  new_barrier.Transition.StateBefore = subresource_state.value;
                  resource_barriers.push_back(new_barrier);
                }
              }
            }
            else
            {
              // No (sub)resources need to be transitioned. Just add a single transition barrier (if needed).
              auto global_state = (iter->value).subresource_state(pending_transition.Subresource);

              if(pending_transition.StateAfter != global_state)
              {
                // Fix-up the before state based on current global state of the resource.
                pending_barrier.Transition.StateBefore = global_state;
                resource_barriers.push_back(pending_barrier);
              }
            }
          }
        }
      }

      u32 num_barriers = static_cast<u32>(resource_barriers.size());
      if(num_barriers > 0)
      {
        auto d3d_command_list = commandList->graphics_command_list();
        d3d_command_list->ResourceBarrier(num_barriers, resource_barriers.data());
      }

      m_pending_resource_barriers.clear();

      return num_barriers;
    }

    void ResourceStateTracker::commit_final_resource_states()
    {
      REX_ASSERT_X(s_is_locked, "Resource State Tracker is not locked");

      // Commit final resource states to the global resource state array (map).
      for(const auto& resource_state: m_final_resource_state)
      {
        s_global_resource_state[resource_state.key] = resource_state.value;
      }

      m_final_resource_state.clear();
    }

    void ResourceStateTracker::reset()
    {
      // Reset the pending, current, and final resource states.
      m_pending_resource_barriers.clear();
      m_resource_barriers.clear();
      m_final_resource_state.clear();
    }

    void ResourceStateTracker::lock()
    {
      s_global_mutex.lock();
      s_is_locked = true;
    }

    void ResourceStateTracker::unlock()
    {
      s_global_mutex.unlock();
      s_is_locked = false;
    }

    void ResourceStateTracker::add_global_resource_state(ID3D12Resource* resource, D3D12_RESOURCE_STATES state)
    {
      if(resource != nullptr)
      {
        rsl::unique_lock<rsl::mutex> lock(s_global_mutex);
        s_global_resource_state[resource].set_subresource_state(D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES, state);
      }
    }
  } // namespace renderer
} // namespace rex