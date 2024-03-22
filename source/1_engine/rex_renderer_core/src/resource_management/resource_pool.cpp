#include "rex_renderer_core/resource_management/resource_pool.h"

namespace rex
{
  namespace rhi
  {
    //-----------------------------------------------------------------------
    void ResourcePool::clear()
    {
      rsl::unique_lock const sl(m_lock);
      m_resource_map.clear();
    }

    //-----------------------------------------------------------------------
    ResourceSlot ResourcePool::insert(ResourcePtr&& resource)
    {
      ResourceSlot slot = new_slot();

      rsl::unique_lock const sl(m_lock);
      m_resource_map[slot] = rsl::move(resource);

      return slot;
    }

    //-----------------------------------------------------------------------
    void ResourcePool::remove(const ResourceSlot& slot)
    {
      REX_ASSERT_X(has_slot(slot), "Slot was not registered within resource pool ({})", slot.slot_id());

      rsl::unique_lock const sl(m_lock);
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
  } // namespace renderer
} // namespace rex