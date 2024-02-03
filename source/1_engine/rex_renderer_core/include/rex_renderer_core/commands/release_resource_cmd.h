#pragma once

#include "rex_renderer_core/commands/render_cmd.h"
#include "rex_renderer_core/resource_slot.h"
#include "rex_renderer_core/resource_slots.h"

namespace rex
{
  namespace renderer
  {
    namespace commands
    {
      struct ReleaseResourceCommandDesc
      {
          ReleaseResourceCommandDesc()
              :slots(nullptr)
          {}

          explicit ReleaseResourceCommandDesc(ResourceSlots* resourceSlots)
              :slots(resourceSlots)
          {}

          ResourceSlots* slots;
      };

      class ReleaseResource : public RenderCommand
      {
      public:
        ReleaseResource(ReleaseResourceCommandDesc&& desc, const ResourceSlot& slot)
            : RenderCommand()
            , m_desc(rsl::move(desc))
            , m_resource_slot(slot)
        {
        }

        ~ReleaseResource() override = default;

        bool execute() override
        {
          bool result = backend::release_resource(m_resource_slot);

          result = m_desc.slots->free_slot(m_resource_slot.slot_id());

          return result;
        }

      private:
        ReleaseResourceCommandDesc m_desc;
        ResourceSlot m_resource_slot;
      };
    } // namespace commands
  }   // namespace renderer
} // namespace rex