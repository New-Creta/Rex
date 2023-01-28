#pragma once

#include "rex_engine/types.h"

namespace rex
{
    struct renderer_info
    {
        const c8* api_version;
        const c8* shader_version;
        const c8* renderer;
        const c8* vendor;
    };

    namespace renderer
    {
        bool initialize(void* userData, u32 maxCommands);
    }
}