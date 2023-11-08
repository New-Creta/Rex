#pragma once

#include "rex_engine/types.h"
#include "rex_renderer_core/parameters/constant_layout_description_params.h"

namespace rex
{
    namespace renderer
    {
        namespace parameters
        {
            struct LinkShader
            {
                s32 vertex_shader;
                s32 pixel_shader;

                ConstantLayoutDescription* constants;
                s32 num_constants;
            };
        }
    }
}