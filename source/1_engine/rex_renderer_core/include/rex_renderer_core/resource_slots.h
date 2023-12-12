#pragma once

#include "rex_engine/types.h"

#include "rex_renderer_core/resource_slot.h"

#include <rex_std/vector.h>
#include <rex_std/atomic.h>

namespace rex
{
    namespace renderer
    {
        /**
         * @class ResourceSlots
         * @brief Manages resource slots within the engine.
         *
         * This class is meant to manage the various slots of resources that could exist within the engine.
         * It provides a mechanism to allocate and free slots for a variaty of resources.
         *
         * Resources are not directly exposed to the user of the API; instead, the user interacts with resource
         * indices provided by this class. The engine uses these indices to query the correct resource from
         * the resource pool using the RHI (Rendering Hardware Interface) layer.
         */
        class ResourceSlots
        {
        public:
            ~ResourceSlots();

            /**
             * Initialize the ResourceSlots class with an initial capacity
             *
             * @param num The initial number of resource slots to manage.
             */
            void initialize(s32 num);

            /**
             * Allocate the next available resource slot.
             *
             * @return The index of the allocated resource slot.
             */
            ResourceSlot next_slot();

            /**
             * Free a previously allocated resource slot.
             *
             * @param slot The index of the resource slot to be freed.
             * @return `true` if the slot was successfully freed, `false` if the slot is not in use or out of range.
             */
            bool free_slot(s32 slot);

        private:
            /**
             * Resize the internal array of atomic flags to accommodate a new number of resource slots.
             *
             * @param num The new number of resource slots to support.
             */
            void resize(s32 num);

        private:
            // An array of atomic flags to manage resource slot states.
            rsl::atomic_flag* m_flags;
            rsl::count_t m_flag_capacity;
        };
    }
}