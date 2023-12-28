#pragma once

#include "rex_engine/types.h"
#include "rex_engine/diagnostics/assert.h"

#include "rex_renderer_core/resource_slot.h"
#include "rex_renderer_core/resource.h"

#include "rex_std/memory.h"
#include "rex_std/unordered_map.h"
#include "rex_std/mutex.h"

namespace rex
{
    namespace renderer
    {
        using ResourcePtr = rsl::unique_ptr<IResource>;

        class ResourcePool
        {
        public:
            void                    initialize(s32 reservedCapacity);
            void                    clear();

            void                    insert(const ResourceSlot& slot, ResourcePtr&& resource);
            void                    remove(const ResourceSlot& slot);

            bool                    has_slot(const ResourceSlot& slot) const;

            ResourcePtr&            at(const ResourceSlot& slot);
            const ResourcePtr&      at(const ResourceSlot& slot) const;

        private:
            using ResourceMap = rsl::unordered_map<ResourceSlot, ResourcePtr>;

            void                    validate_and_grow_if_necessary(const ResourceSlot& minCapacity);

            ResourceMap             m_resource_map;
            rsl::recursive_mutex    m_lock;
        };

        //-----------------------------------------------------------------------
        template<typename U>
        U& get_resource_from_pool_as(ResourcePool& pool, const ResourceSlot& slot)
        {
            REX_ASSERT_X(pool.has_slot(slot), "Slot was not registered within resource pool ({})", slot.slot_id());
            REX_ASSERT_X(slot != globals::g_invalid_slot_id, "Invalid index given to retrieve resource from resource pool");
            REX_ASSERT_X(U::static_type() == pool.at(slot)->type(), "Invalid type cast for given resource");

            return static_cast<U&>(*pool.at(slot));
        }
        //-----------------------------------------------------------------------
        template<typename U>
        const U& get_resource_from_pool_as(const ResourcePool& pool, const ResourceSlot& slot)
        {
            REX_ASSERT_X(pool.has_slot(slot), "Slot was not registered within resource pool ({})", slot.slot_id());
            REX_ASSERT_X(slot != globals::g_invalid_slot_id, "Invalid index given to retrieve resource from resource pool");
            REX_ASSERT_X(U::static_type() == pool.at(slot)->type(), "Invalid type cast for given resource");

            return static_cast<const U&>(pool.at(slot));
        }
    }
}