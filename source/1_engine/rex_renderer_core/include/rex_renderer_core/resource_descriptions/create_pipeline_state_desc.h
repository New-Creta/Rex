#pragma once

#include "rex_renderer_core/resource_slot.h"

namespace rex
{
  namespace renderer
  {
      struct CreatePipelineStateDesc
      {
        CreatePipelineStateDesc()
            : rasterizer_state(ResourceSlot::make_invalid())
            , blend_state(ResourceSlot::make_invalid())
            , depth_stencil_state(ResourceSlot::make_invalid())
            , num_render_targets(0)
        {
        }

        ResourceSlot rasterizer_state;
        ResourceSlot blend_state;
        ResourceSlot depth_stencil_state;
        s32 num_render_targets;
      };
  }   // namespace renderer
} // namespace rex

namespace rsl
{
  inline namespace v1
  {
    template <>
    struct hash<rex::renderer::CreatePipelineStateDesc>
    {
      rsl::hash_result operator()(const rex::renderer::CreatePipelineStateDesc& createPipelineStateCommand) const
      {
        card64 seed = 0;

        seed = internal::hash_combine(seed, createPipelineStateCommand.rasterizer_state.slot_id());
        seed = internal::hash_combine(seed, createPipelineStateCommand.blend_state.slot_id());
        seed = internal::hash_combine(seed, createPipelineStateCommand.depth_stencil_state.slot_id());
        seed = internal::hash_combine(seed, createPipelineStateCommand.num_render_targets);

        return static_cast<rsl::hash_result>(seed);
      }
    };
  } // namespace v1
} // namespace rsl