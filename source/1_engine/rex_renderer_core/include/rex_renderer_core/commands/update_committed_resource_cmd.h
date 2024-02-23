#pragma once

#include "rex_engine/memory/blob.h"
#include "rex_renderer_core/commands/render_cmd.h"

namespace rex
{
  namespace renderer
  {
    namespace commands
    {
      struct UpdateCommittedResourceCommandDesc
      {
        UpdateCommittedResourceCommandDesc()
            : buffer_data()
            , element_index(0)
        {
        }

        memory::Blob buffer_data;
        s32 element_index;
      };

      class UpdateCommittedResource : public RenderCommand
      {
      public:
        UpdateCommittedResource(UpdateCommittedResourceCommandDesc&& desc, const ResourceSlot& slot)
            : RenderCommand()
            , m_desc(rsl::move(desc))
            , m_resource_slot(slot)
        {
        }

        ~UpdateCommittedResource() override = default;

        bool execute() override
        {
          backend::update_committed_resource(m_desc, m_resource_slot);

          return true;
        }

      private:
        UpdateCommittedResourceCommandDesc m_desc;
        ResourceSlot m_resource_slot;
      };
    } // namespace commands
  }   // namespace renderer
} // namespace rex