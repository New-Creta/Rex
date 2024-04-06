#pragma once

#include "rex_engine/diagnostics/assert.h"
#include "rex_engine/engine/types.h"
#include "rex_renderer_core/resource_management/resource.h"
#include "rex_renderer_core/resource_management/resource_slot.h"
#include "rex_renderer_core/system/resource_hasher.h"
#include "rex_std/memory.h"
#include "rex_std/mutex.h"
#include "rex_std/unordered_map.h"

namespace rex
{
  namespace rhi
  {
    using ResourcePtr = rsl::unique_ptr<IResource>;

    struct ResourceWithSlot
    {
      ResourcePtr resource;
      ResourceSlot slot;
    };

    class ResourcePool
    {
    public:
      void clear();

      ResourceSlot insert(ResourcePtr&& resource);
      void remove(const ResourceSlot& slot);

      bool has_slot(const ResourceSlot& slot) const;
      bool has_resource(ResourceHash hash) const;

      template <typename U>
      bool is(const ResourceSlot& slot) const;

      IResource* at(const ResourceSlot& slot);
      const IResource* at(const ResourceSlot& slot) const;

      ResourceSlot at(ResourceHash slot) const;

      template <typename U>
      U* as(const ResourceSlot& slot);
      template <typename U>
      const U* as(const ResourceSlot& slot) const;

    private:
      using ResourceMap = rsl::unordered_map<ResourceHash, ResourceWithSlot>;

    private:
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
    U* ResourcePool::as(const ResourceSlot& slot)
    {
      REX_ASSERT_X(has_slot(slot), "Slot was not registered within resource pool ({})", slot.slot_id());
      REX_ASSERT_X(slot != globals::g_invalid_slot_id, "Invalid index given to retrieve resource from resource pool");
      REX_ASSERT_X(is<U>(slot), "Invalid type cast for given resource");

      return static_cast<U*>(at(slot));
    }
    //-----------------------------------------------------------------------
    template <typename U>
    const U* ResourcePool::as(const ResourceSlot& slot) const
    {
      REX_ASSERT_X(has_slot(slot), "Slot was not registered within resource pool ({})", slot.slot_id());
      REX_ASSERT_X(slot != globals::g_invalid_slot_id, "Invalid index given to retrieve resource from resource pool");
      REX_ASSERT_X(is<U>(slot), "Invalid type cast for given resource");

      return static_cast<const U*>(at(slot));
    }
  } // namespace rhi
} // namespace rex