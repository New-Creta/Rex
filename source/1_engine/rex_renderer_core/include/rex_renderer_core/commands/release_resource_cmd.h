#pragma once

#include "rex_renderer_core/commands/render_cmd.h"
#include "rex_renderer_core/resource_slot.h"

namespace rex
{
  namespace renderer
  {
    namespace commands
    {
      struct ReleaseResourceCommandDesc
      {
        RenderCommandDesc command;

        ResourceSlot resource_slot;
      };

      class ReleaseResource : public RenderCommand
      {
      public:
        ReleaseResource(ReleaseResourceCommandDesc&& desc)
            : RenderCommand(rsl::move(desc.command))
            , m_desc(rsl::move(desc))
        {
        }

        ~ReleaseResource() override = default;

        bool execute() override
        {
          result = backend::release_resource(cmd.release_resource.resource_index.slot_id());
          g_ctx.slot_resources.free_slot(cmd.release_resource.resource_index.slot_id());

          return result;
        }

      private:
        ReleaseResourceCommandDesc m_desc;
      };
    } // namespace commands
  }   // namespace renderer
} // namespace rex