#pragma once

#include "rex_engine/types.h"
#include "rex_engine/defines.h"

#include "rex_renderer_core/clear_bits.h"

#include "rex_std_extra/math/color.h"
#include "rex_std/functional.h"

namespace rex
{
    namespace renderer
    {
        namespace parameters
        {
            struct CreatePipelineState
            {
                CreatePipelineState()
                    :input_layout(REX_INVALID_INDEX)
                    ,shader_program(REX_INVALID_INDEX)
                    ,rasterizer_state(REX_INVALID_INDEX)
                    ,blend_state(REX_INVALID_INDEX)
                    ,depth_stencil_state(REX_INVALID_INDEX)
                    ,num_render_targets(REX_INVALID_INDEX)
                {}

                s32 input_layout;
                s32 shader_program;
                s32 rasterizer_state;
                s32 blend_state;
                s32 depth_stencil_state;
                s32 num_render_targets;
            };
        }
    }
}

namespace rsl
{
    inline namespace v1
    {
        template<>
        struct hash<rex::renderer::parameters::CreatePipelineState>
        {
            rsl::hash_result operator()(const rex::renderer::parameters::CreatePipelineState& params) const
            {
                card64 seed = 0;
                
                seed = internal::hash_combine(seed, params.input_layout);
                seed = internal::hash_combine(seed, params.root_signature);
                seed = internal::hash_combine(seed, params.vertex_shader);
                seed = internal::hash_combine(seed, params.pixel_shader);
                seed = internal::hash_combine(seed, params.rasterizer_state);
                seed = internal::hash_combine(seed, params.blend_state);
                seed = internal::hash_combine(seed, params.depth_stencil_state);
                seed = internal::hash_combine(seed, params.num_render_targets);

                return static_cast<rsl::hash_result>(seed);
            }
        };
    }
}