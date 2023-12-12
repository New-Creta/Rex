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
        RenderCommandDesc command;

        ResourceSlot pipeline_state;
      };

      class SetPipelineState : public RenderCommand
      {
      public:
        SetPipelineState(SetPipelineStateCommandDesc&& desc)
            : RenderCommand(rsl::move(desc.command))
            , m_desc(rsl::move(desc))
        {
        }

        ~SetPipelineState() override = default;

        bool execute() override 
        {
          return backend::set_pipeline_state_object(cmd.set_pipeline_state.pipeline_state.slot_id());  
        }

      private:
        SetPipelineStateCommandDesc m_desc;
      };
    } // namespace commands
  }   // namespace renderer
} // namespace rex