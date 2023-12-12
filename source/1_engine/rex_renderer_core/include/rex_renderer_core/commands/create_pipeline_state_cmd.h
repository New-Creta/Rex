#pragma once

#include "rex_renderer_core/commands/render_cmd.h"
#include "rex_renderer_core/resource_slot.h"

namespace rex
{
  namespace renderer
  {
    namespace commands
    {
      struct CreatePipelineStateCommandDesc
      {
        ResourceSlot input_layout;
        ResourceSlot shader_program;
        ResourceSlot rasterizer_state;
        ResourceSlot blend_state;
        ResourceSlot depth_stencil_state;
        s32 num_render_targets;
      };

      class CreatePipelineState : public RenderCommand
      {
      public:
        CreatePipelineState(CreatePipelineStateCommandDesc&& desc, ResourceSlot slot)
            : RenderCommand()
            , m_desc(rsl::move(desc))
            , m_resource_slot(slot)
        {
        }

        ~CreatePipelineState() override = default;

        bool execute() override
        {
          return backend::create_pipeline_state_object(m_desc, m_resource_slot);
        }

      private:
        CreatePipelineStateCommandDesc m_desc;
        ResourceSlot m_resource_slot;
      };
    } // namespace commands
  }   // namespace renderer
} // namespace rex

namespace rsl
{
  inline namespace v1
  {
    template <>
    struct hash<rex::renderer::commands::CreatePipelineState>
    {
      rsl::hash_result operator()(const rex::renderer::commands::CreatePipelineState& createPipelineStateCommand) const
      {
        card64 seed = 0;

        seed = internal::hash_combine(seed, createPipelineStateCommand.input_layout.slot_id());
        seed = internal::hash_combine(seed, createPipelineStateCommand.shader_program.slot_id());
        seed = internal::hash_combine(seed, createPipelineStateCommand.rasterizer_state.slot_id());
        seed = internal::hash_combine(seed, createPipelineStateCommand.blend_state.slot_id());
        seed = internal::hash_combine(seed, createPipelineStateCommand.depth_stencil_state.slot_id());
        seed = internal::hash_combine(seed, createPipelineStateCommand.nurender_targets);

        return static_cast<rsl::hash_result>(seed);
      }
    };
  } // namespace v1
} // namespace rsl