#pragma once

/**
 *  @brief Tracks the known state of a (sub)resource within a command list.
 *
 *  The ResourceStateTracker tracks the known state of a (sub)resource within a command list.
 *  It is often difficult (or impossible) to know the current state of a (sub)resource if
 *  it is being used in multiple command lists. For example when doing shadow mapping,
 *  a depth buffer resource is being used as a depth-stencil view in a command list
 *  that is generating the shadow map for a light source, but needs to be used as
 *  a shader-resource view in a command list that is performing shadow mapping. If
 *  the command lists are being generated in separate threads, the exact state of the
 *  resource can't be guaranteed at the moment it is used in the command list.
 *  The ResourceStateTracker class is intended to be used within a command list
 *  to track the state of the resource as it is known within that command list.
 *
 *  @see https://youtu.be/nmB2XMasz2o
 *  @see https://msdn.microsoft.com/en-us/library/dn899226(v=vs.85).aspx#implicit_state_transitions
 */

#include "rex_engine/engine/types.h"

#include "rex_directx/directx_util.h"

#include "rex_std/mutex.h"
#include "rex_std/map.h"
#include "rex_std/unordered_map.h"
#include "rex_std/vector.h"

#include <mutex>

namespace rex
{
    namespace renderer
    {
        class CommandList;
        class Resource;

        class ResourceStateTracker
        {
        public:
            ResourceStateTracker();
            virtual ~ResourceStateTracker();

            /**
             * Push a resource barrier to the resource state tracker.
             *
             * @param barrier The resource barrier to push to the resource state tracker.
             */
            void resource_barrier(const D3D12_RESOURCE_BARRIER& barrier);

            /**
             * Push a transition resource barrier to the resource state tracker.
             *
             * @param resource The resource to transition.
             * @param stateAfter The state to transition the resource to.
             * @param subResource The subresource to transition. By default, this is D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES
             * which indicates that all subresources should be transitioned to the same state.
             */
            void transition_resource(ID3D12Resource* resource, D3D12_RESOURCE_STATES stateAfter, u32 subResource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES);
            void transition_resource(const Resource& resource, D3D12_RESOURCE_STATES stateAfter, u32 subResource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES);

            /**
             * Flush any pending resource barriers to the command list.
             *
             * @return The number of resource barriers that were flushed to the command list.
             */
            u32 flush_pending_resource_barriers(const rsl::shared_ptr<CommandList>& commandList);

            /**
             * Flush any (non-pending) resource barriers that have been pushed to the resource state
             * tracker.
             */
            void flush_resource_barriers(const rsl::shared_ptr<CommandList>& commandList);

            /**
             * Commit final resource states to the global resource state map.
             * This must be called when the command list is closed.
             */
            void commit_final_resource_states();

            /**
             * Reset state tracking. This must be done when the command list is reset.
             */
            void reset();

            /**
             * The global state must be locked before flushing pending resource barriers
             * and committing the final resource state to the global resource state.
             * This ensures consistency of the global resource state between command list
             * executions.
             */
            static void lock();

            /**
             * Unlocks the global resource state after the final states have been committed
             * to the global resource state array.
             */
            static void unlock();

            /**
             * Add a resource with a given state to the global resource state array (map).
             * This should be done when the resource is created for the first time.
             */
            static void add_global_resource_state(ID3D12Resource* resource, D3D12_RESOURCE_STATES state);

        private:
            // An array (vector) of resource barriers.
            using ResourceBarriers = rsl::vector<D3D12_RESOURCE_BARRIER>;

            // Pending resource transitions are committed before a command list
            // is executed on the command queue. This guarantees that resources will
            // be in the expected state at the beginning of a command list.
            ResourceBarriers m_pending_resource_barriers;

            // Resource barriers that need to be committed to the command list.
            ResourceBarriers m_resource_barriers;

            // Tracks the state of a particular resource and all of its subresources.
            struct ResourceState
            {
                // Initialize all of the subresources within a resource to the given state.
                explicit ResourceState(D3D12_RESOURCE_STATES state = D3D12_RESOURCE_STATE_COMMON)
                    : state(state)
                {}

                // Set a subresource to a particular state.
                void set_subresource_state(u32 inSubresource, D3D12_RESOURCE_STATES inState)
                {
                    if (inSubresource == D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES)
                    {
                        state = inState;
                        subresource_state_map.clear();
                    }
                    else
                    {
                        subresource_state_map[inSubresource] = inState;
                    }
                }

                // Get the state of a (sub)resource within the resource.
                // If the specified subresource is not found in the SubresourceState array (map)
                // then the state of the resource (D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES) is
                // returned.
                D3D12_RESOURCE_STATES subresource_state(u32 subresource) const
                {
                    D3D12_RESOURCE_STATES new_state = state;
                    const auto iter = subresource_state_map.find(subresource);
                    if(iter != subresource_state_map.end())
                    {
                        new_state = iter->value;
                    }
                    return new_state;
                }

                // If the SubresourceState array (map) is empty, then the State variable defines 
                // the state of all of the subresources.
                D3D12_RESOURCE_STATES state;
                rsl::map<u32, D3D12_RESOURCE_STATES> subresource_state_map;
            };

            using ResourceStateMap = rsl::unordered_map<ID3D12Resource*, ResourceState>;

            // The final (last known state) of the resources within a command list.
            // The final resource state is committed to the global resource state when the 
            // command list is closed but before it is executed on the command queue.
            ResourceStateMap m_final_resource_state;

            // The global resource state array (map) stores the state of a resource
            // between command list execution.
            static ResourceStateMap s_global_resource_state;

            // The mutex protects shared access to the GlobalResourceState map.
            static rsl::mutex s_global_mutex;
            static bool s_is_locked;
        };
    }
}