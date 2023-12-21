#pragma once

#include "rex_renderer_core/commands/render_cmd.h"

#include "rex_engine/memory/blob.h"

namespace rex
{
  namespace renderer
  {
    namespace commands
    {
      struct UpdateCommitedResourceCommandDesc
      {
          UpdateCommitedResourceCommandDesc()
            :buffer_data()
            ,element_index(0)
        {}

        memory::Blob buffer_data;
        s32 element_index;
      };

      class UpdateCommitedResource : public RenderCommand
      {
      public:
          UpdateCommitedResource(UpdateCommitedResourceCommandDesc&& desc, ResourceSlot slot)
            : RenderCommand()
            , m_desc(rsl::move(desc))
            , m_resource_slot(slot)
        {
        }

        ~UpdateCommitedResource() override = default;

        bool execute() override 
        {
          backend::update_commited_resource(m_desc, m_resource_slot);

          return true;
        }

      private:
        UpdateCommitedResourceCommandDesc m_desc;
        ResourceSlot m_resource_slot;
      };
    } // namespace commands
  }   // namespace renderer
} // namespace rex