#pragma once

#include "rex_engine/types.h"

#include "rex_renderer_core/shader_type.h"

namespace rex
{
    namespace renderer
    {
        namespace parameters
        {
            struct LoadShader
            {
                ShaderType shader_type;

                void* byte_code;
                s32 byte_code_size;

                s32 constant_buffer_count;
            };
        }
    }
}