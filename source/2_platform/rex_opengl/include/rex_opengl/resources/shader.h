#pragma once

#include "rex_engine/types.h"

namespace rex
{
    u32 create_vertex_shader(u64 shaderElementCount, const char** shaderElements, s32* shaderElementLength = nullptr);
    u32 create_fragment_shader(u64 shaderElementCount, const char** shaderElements, s32* shaderElementLength = nullptr);

    void destroy_vertex_shader(u32 shader);
    void destroy_fragment_shader(u32 shader);
}