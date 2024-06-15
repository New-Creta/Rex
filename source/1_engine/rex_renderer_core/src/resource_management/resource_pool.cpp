//#include "rex_renderer_core/resource_management/resource_pool.h"
//
//namespace rex
//{
//  namespace gfx
//  {
//    //-----------------------------------------------------------------------
//    void ResourcePool::clear()
//    {
//      rsl::unique_lock const sl(m_lock);
//      m_resource_map.clear();
//    }
//
//    //-----------------------------------------------------------------------
//    ResourceSlot ResourcePool::insert(ResourcePtr&& resource)
//    {
//      rsl::unique_lock const sl(m_lock);
//      ResourceID id = resource->id();
//      m_resource_map[id] = ResourceWithSlot{ rsl::move(resource), ResourceSlot(id) };
//
//      return m_resource_map[id].slot;
//    }
//
//    //-----------------------------------------------------------------------
//    void ResourcePool::remove(const ResourceSlot& slot)
//    {
//      REX_ASSERT_X(has_slot(slot), "Slot was not registered within resource pool ({})", slot.slot_id());
//
//      rsl::unique_lock const sl(m_lock);
//      m_resource_map.erase(slot.slot_id());
//    }
//
//    //-----------------------------------------------------------------------
//    bool ResourcePool::has_slot(const ResourceSlot& slot) const
//    {
//      return has_resource(slot.slot_id());
//    }
//    //-----------------------------------------------------------------------
//    bool ResourcePool::has_resource(ResourceID id) const
//    {
//      return m_resource_map.find(id) != m_resource_map.cend();
//    }
//
//    //-----------------------------------------------------------------------
//    IResource* ResourcePool::at(const ResourceSlot& slot)
//    {
//      REX_ASSERT_X(has_slot(slot), "Slot was not registered within resource pool ({})", slot.slot_id());
//      return m_resource_map.at(slot.slot_id()).resource.get();
//    }
//
//    //-----------------------------------------------------------------------
//    const IResource* ResourcePool::at(const ResourceSlot& slot) const
//    {
//      REX_ASSERT_X(has_slot(slot), "Slot was not registered within resource pool ({})", slot.slot_id());
//      return m_resource_map.at(slot.slot_id()).resource.get();
//    }
//
//    //-----------------------------------------------------------------------
//    ResourceSlot ResourcePool::at(ResourceID id) const
//    {
//      REX_ASSERT_X(has_resource(id), "Hash was not registered within resource pool ({})", id);
//      return m_resource_map.at(id).slot;
//    }
//  } // namespace gfx
//} // namespace rex