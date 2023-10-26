#pragma once

#include "rex_engine/types.h"

namespace rex
{
    namespace renderer
    {
        namespace parameters
        {
            enum class ConstantType
            {
                SAMPLER_2D = 0,
                SAMPLER_3D,
                SAMPLER_CUBE,
                SAMPLER_2DMS,
                SAMPLER_2D_ARRAY,
                SAMPLER_2D_DEPTH,
                SAMPLER_2D_DEPTH_ARRAY,
                CBUFFER,
                CONSTANT
            };

            struct ConstantLayoutDescription
            {
                ConstantType type;

                char8* name;
                u32 location;
            };

            struct LinkShader
            {
                u32 vertex_shader;
                u32 pixel_shader;
                u32 input_layout;
                u32 compute_shader;

                ConstantLayoutDescription* constants;
                u32 num_constants;
            };
        }
    }
}