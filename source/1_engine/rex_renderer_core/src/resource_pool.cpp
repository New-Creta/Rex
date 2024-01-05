#include "rex_renderer_core/resource_pool.h"

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
            rsl::unique_lock sl(m_lock);
            m_resource_map.clear();
        }

        //-----------------------------------------------------------------------
        void ResourcePool::insert(const ResourceSlot& slot, ResourcePtr&& resource)
        {
          // The slots are all in numerical order ( see ResourceSlots.cpp )
          // This means if there is a slot ID incoming with a larger value as the currently allocated buckets within the hashmap
          // We have to grow rehash and allocate a larger number of buckets
            validate_and_grow_if_necessary(slot.slot_id()); 

            rsl::unique_lock sl(m_lock);
            m_resource_map[slot] = rsl::move(resource);
        }

        //-----------------------------------------------------------------------
        void ResourcePool::remove(const ResourceSlot& slot)
        {
            REX_ASSERT_X(has_slot(slot), "Slot was not registered within resource pool ({})", slot.slot_id());

            rsl::unique_lock sl(m_lock);
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
        void ResourcePool::validate_and_grow_if_necessary(s32 minCapacity)
        {
            if (minCapacity >= m_resource_map.size())
            {
                s32 new_cap = (minCapacity * 2) + 1; // Grow to accommodate the slot

                rsl::unique_lock sl(m_lock);
                m_resource_map.reserve(new_cap);
            }
        }
    }
}