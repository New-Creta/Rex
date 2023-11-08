#pragma once

#include "rex_engine/types.h"

namespace rex
{
    namespace renderer
    {
        namespace commands
        {
            struct BeginDraw
            {
                BeginDraw()
                {}

                s32 shader_program;
            };
        }
    }
}