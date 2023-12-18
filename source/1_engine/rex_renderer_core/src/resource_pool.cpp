#include "rex_renderer_core/resource_pool.h"

#include "rex_engine/threading/scoped_lock.h"

namespace rex
{
    namespace renderer
    {
        //-----------------------------------------------------------------------
        void ResourcePool::initialize(s32 reservedCapacity)
        {
            m_resource_map.reserve(reservedCapacity);
        }

        //-----------------------------------------------------------------------
        void ResourcePool::clear()
        {
            ScopedLock sl(m_lock);
            m_resource_map.clear();
        }

        //-----------------------------------------------------------------------
        void ResourcePool::insert(const ResourceSlot& slot, ResourcePtr&& resource)
        {
            validate_and_grow_if_necessary(slot);

            ScopedLock sl(m_lock);
            m_resource_map[slot] = rsl::move(resource);
        }

        //-----------------------------------------------------------------------
        void ResourcePool::remove(const ResourceSlot& slot)
        {
            REX_ASSERT_X(has_slot(slot), "Slot was not registered within resource pool ({})", slot.slot_id());

            ScopedLock sl(m_lock);
            m_resource_map.erase(slot);
        }

        //-----------------------------------------------------------------------
        bool ResourcePool::has_slot(const ResourceSlot& slot) const
        {
            return m_resource_map.find(slot) != m_resource_map.cend();
        }

        //-----------------------------------------------------------------------
        ResourcePtr& ResourcePool::at(const ResourceSlot& slot)
        {
            REX_ASSERT_X(has_slot(slot), "Slot was not registered within resource pool ({})", slot.slot_id());
            return m_resource_map.at(slot);
        }

        //-----------------------------------------------------------------------
        const ResourcePtr& ResourcePool::at(const ResourceSlot& slot) const
        {
            REX_ASSERT_X(has_slot(slot), "Slot was not registered within resource pool ({})", slot.slot_id());
            return m_resource_map.at(slot);
        }

        //-----------------------------------------------------------------------
        void ResourcePool::validate_and_grow_if_necessary(const ResourceSlot& slot)
        {
            // The slots are all in numerical order ( see ResourceSlots.cpp )
            // This means if there is a slot ID incoming with a larger value as the currently allocated buckets within the hashmap
            // We have to grow rehash and allocate a larger number of buckets
            s32 min_capacity = slot.slot_id();

            if (min_capacity >= m_resource_map.size())
            {
                s32 new_cap = (min_capacity * 2) + 1; // Grow to accommodate the slot

                ScopedLock sl(m_lock);
                m_resource_map.reserve(new_cap);
            }
        }
    }
}