#pragma once

#include "rex_engine/types.h"

#include "rex_renderer_core/shader_type.h"

namespace rex
{
    namespace renderer
    {
        namespace commands
        {
            struct SetShader
            {
                SetShader()
                    :shader_index(REX_INVALID_INDEX)
                    ,shader_type(ShaderType::NONE)
                {}

                s32 shader_index;
                ShaderType shader_type;
            };
        }
    }
}