#pragma once

#include "rex_renderer_core/commands/render_cmd.h"
#include "rex_renderer_core/parameters/create_constant_layout_description_cmd.h"
#include "rex_renderer_core/resource_slot.h"

namespace rex
{
  namespace renderer
  {
    namespace commands
    {
      struct LinkShaderCommandDesc
      {
        RenderCommandDesc command;

        ResourceSlot vertex_shader;
        ResourceSlot pixel_shader;
        ConstantLayoutDescription* constants;
        s32 nuconstants;
      };

      class LinkShader : public RenderCommand
      {
      public:
        LinkShader(LinkShaderCommandDesc&& desc)
            : RenderCommand(rsl::move(desc.command))
            , m_desc(rsl::move(desc))
        {
        }

        ~LinkShader() override = default;

        bool execute() override
        {
          result = backend::link_shader(cmd.link_shader_params, cmd.resource_slot);
          for(s32 i = 0; i < cmd.link_shader_params.num_constants; ++i)
            memory_free(cmd.link_shader_params.constants[i].name);
          memory_free(cmd.link_shader_params.constants);

          return result;
        }

      private:
        LinkShaderCommandDesc m_desc;
      };
    } // namespace commands
  }   // namespace renderer
} // namespace rex