#pragma once

#include "rex_renderer_core/commands/render_cmd.h"
#include "rex_renderer_core/shader_type.h"

namespace rex
{
  namespace renderer
  {
    namespace commands
    {
      struct LoadShaderCommandDesc
      {
        RenderCommandDesc command;

        ShaderType shader_type;
        void* byte_code;
        s32 byte_code_size;
        s32 constant_buffer_count;
      };

      class LoadShader : public RenderCommand
      {
      public:
        LoadShader(LoadShaderCommandDesc&& desc)
            : RenderCommand(rsl::move(desc.command))
            , m_desc(rsl::move(desc))
        {
        }

        ~LoadShader() override = default;

        bool execute() override
        {
          result = backend::load_shader(cmd.load_shader_params, cmd.resource_slot);
          memory_free(cmd.load_shader_params.byte_code);

          return result;
        }

      private:
        LoadShaderCommandDesc m_desc;
      };
    } // namespace commands
  }   // namespace renderer
} // namespace rex