#pragma once

#include "rex_renderer_core/commands/render_cmd.h"
#include "rex_renderer_core/resource_slot.h"

namespace rex
{
  namespace renderer
  {
    namespace commands
    {
      struct SetShaderCommandDesc
      {
      };

      class SetShader : public RenderCommand
      {
      public:
        SetShader(SetShaderCommandDesc&& desc, ResourceSlot slot)
            : RenderCommand()
            , m_desc(rsl::move(desc))
            , m_resource_slot(slot)
        {
        }

        ~SetShader() override = default;

        bool execute() override 
        {
          return backend::set_shader(m_resource_slot);
        }

      private:
        SetShaderCommandDesc m_desc;
        ResourceSlot m_resource_slot;
      };
    } // namespace commands
  }   // namespace renderer
} // namespace rex