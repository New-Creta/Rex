#pragma once

#include "rex_renderer_core/commands/render_cmd.h"
#include "rex_renderer_resources/resource_slot.h"

#include <utility>

namespace rex
{
  namespace renderer
  {
    namespace commands
    {
      struct CreatePipelineStateCommandDesc
      {
        CreatePipelineStateCommandDesc()
            : input_layout(ResourceSlot::make_invalid())
            , shader_program(ResourceSlot::make_invalid())
            , rasterizer_state(ResourceSlot::make_invalid())
            , blend_state(ResourceSlot::make_invalid())
            , depth_stencil_state(ResourceSlot::make_invalid())
            , num_render_targets(0)
        {
        }

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
            , m_resource_slot(std::move(std::move(slot)))
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
    struct hash<rex::renderer::commands::CreatePipelineStateCommandDesc>
    {
      rsl::hash_result operator()(const rex::renderer::commands::CreatePipelineStateCommandDesc& createPipelineStateCommand) const
      {
        card64 seed = 0;

        seed = internal::hash_combine(seed, createPipelineStateCommand.input_layout.slot_id());
        seed = internal::hash_combine(seed, createPipelineStateCommand.shader_program.slot_id());
        seed = internal::hash_combine(seed, createPipelineStateCommand.rasterizer_state.slot_id());
        seed = internal::hash_combine(seed, createPipelineStateCommand.blend_state.slot_id());
        seed = internal::hash_combine(seed, createPipelineStateCommand.depth_stencil_state.slot_id());
        seed = internal::hash_combine(seed, createPipelineStateCommand.num_render_targets);

        return static_cast<rsl::hash_result>(seed);
      }
    };
  } // namespace v1
} // namespace rsl