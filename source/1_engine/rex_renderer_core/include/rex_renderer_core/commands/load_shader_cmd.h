#pragma once

#include "rex_engine/memory/blob.h"
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
        LoadShaderCommandDesc()
            : shader_type(ShaderType::None)
            , shader_byte_code()
            , constant_buffer_count(0)
        {
        }

        ShaderType shader_type;
        memory::Blob shader_byte_code;
        s32 constant_buffer_count;
      };

      class LoadShader : public RenderCommand
      {
      public:
        LoadShader(LoadShaderCommandDesc&& desc, const ResourceSlot& slot)
            : RenderCommand()
            , m_desc(rsl::move(desc))
            , m_resource_slot(slot)
        {
        }

        ~LoadShader() override = default;

        bool execute() override
        {
          return backend::load_shader(m_desc, m_resource_slot);
        }

      private:
        LoadShaderCommandDesc m_desc;
        ResourceSlot m_resource_slot;
      };
    } // namespace commands
  }   // namespace renderer
} // namespace rex