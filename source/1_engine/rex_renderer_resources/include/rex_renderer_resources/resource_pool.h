#pragma once

#include "rex_engine/diagnostics/assert.h"
#include "rex_engine/engine/types.h"
#include "rex_renderer_resources/resource.h"
#include "rex_renderer_resources/resource_slot.h"
#include "rex_std/memory.h"
#include "rex_std/mutex.h"
#include "rex_std/unordered_map.h"

namespace rex
{
  namespace renderer
  {
    using ResourcePtr = rsl::unique_ptr<IResource>;

    class ResourcePool
    {
    public:
      void initialize(s32 reservedCapacity);
      void clear();

      void insert(const ResourceSlot& slot, ResourcePtr&& resource);
      void remove(const ResourceSlot& slot);

      bool has_slot(const ResourceSlot& slot) const;

      template <typename U>
      bool is(const ResourceSlot& slot) const;

      ResourcePtr& at(const ResourceSlot& slot);
      const ResourcePtr& at(const ResourceSlot& slot) const;

      template <typename U>
      U& as(const ResourceSlot& slot);
      template <typename U>
      const U& as(const ResourceSlot& slot) const;

      rsl::count_t size() const;

    private:
      using ResourceMap = rsl::unordered_map<ResourceSlot, ResourcePtr>;

      void validate_and_grow_if_necessary(s32 minCapacity);

      ResourceMap m_resource_map {};
      rsl::recursive_mutex m_lock;
    };

    //-----------------------------------------------------------------------
    template <typename U>
    bool ResourcePool::is(const ResourceSlot& slot) const
    {
      return U::static_type() == at(slot)->type();
    }

    //-----------------------------------------------------------------------
    template <typename U>
    U& ResourcePool::as(const ResourceSlot& slot)
    {
      REX_ASSERT_X(has_slot(slot), "Slot was not registered within resource pool ({})", slot.slot_id());
      REX_ASSERT_X(slot != globals::g_invalid_slot_id, "Invalid index given to retrieve resource from resource pool");
      REX_ASSERT_X(is<U>(slot), "Invalid type cast for given resource");

      return static_cast<U&>(*at(slot));
    }
    //-----------------------------------------------------------------------
    template <typename U>
    const U& ResourcePool::as(const ResourceSlot& slot) const
    {
      REX_ASSERT_X(has_slot(slot), "Slot was not registered within resource pool ({})", slot.slot_id());
      REX_ASSERT_X(slot != globals::g_invalid_slot_id, "Invalid index given to retrieve resource from resource pool");
      REX_ASSERT_X(is<U>(slot), "Invalid type cast for given resource");

      return static_cast<const U&>(*at(slot));
    }
  } // namespace renderer
} // namespace rex