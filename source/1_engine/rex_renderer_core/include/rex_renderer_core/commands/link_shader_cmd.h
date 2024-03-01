#pragma once

#include "rex_renderer_core/commands/create_constant_layout_description_cmd.h"
#include "rex_renderer_core/commands/render_cmd.h"
#include "rex_renderer_resources/resource_slot.h"

namespace rex
{
  namespace renderer
  {
    namespace commands
    {
      struct LinkShaderCommandDesc
      {
        ResourceSlot vertex_shader;
        ResourceSlot pixel_shader;
        rsl::vector<ConstantLayoutDescription> constants;
      };

      class LinkShader : public RenderCommand
      {
      public:
        LinkShader(LinkShaderCommandDesc&& desc, const ResourceSlot& slot)
            : RenderCommand()
            , m_desc(rsl::move(desc))
            , m_resource_slot(slot)
        {
        }

        ~LinkShader() override = default;

        bool execute() override
        {
          return backend::link_shader(m_desc, m_resource_slot);
        }

      private:
        LinkShaderCommandDesc m_desc;
        ResourceSlot m_resource_slot;
      };
    } // namespace commands
  }   // namespace renderer
} // namespace rex