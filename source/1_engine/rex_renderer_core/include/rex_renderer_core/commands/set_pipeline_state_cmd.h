#pragma once

#include "rex_renderer_core/commands/render_cmd.h"
#include "rex_renderer_core/resource_slot.h"

namespace rex
{
  namespace renderer
  {
    namespace commands
    {
      struct SetPipelineStateCommandDesc
      {
      };

      class SetPipelineState : public RenderCommand
      {
      public:
        SetPipelineState(SetPipelineStateCommandDesc&& desc, ResourceSlot slot)
            : RenderCommand()
            , m_desc(rsl::move(desc))
            , m_resource_slot(slot)
        {
        }

        ~SetPipelineState() override = default;

        bool execute() override 
        {
          return backend::set_pipeline_state_object(m_resource_slot);  
        }

      private:
        SetPipelineStateCommandDesc m_desc;
        ResourceSlot m_resource_slot;
      };
    } // namespace commands
  }   // namespace renderer
} // namespace rex