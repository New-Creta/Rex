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
                ShaderType  type;

                void* byte_code;
                u32         byte_code_size;
            };
        }
    }
}