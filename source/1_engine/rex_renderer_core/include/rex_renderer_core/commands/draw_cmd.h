#pragma once

#include "rex_engine/types.h"

#include "rex_renderer_core/primitive_topology.h"

namespace rex
{
    namespace renderer
    {
        namespace commands
        {
            struct Draw
            {
                Draw()
                    : vertex_count(0)
                    , start_vertex(REX_INVALID_INDEX)
                {}

                s32 vertex_count;
                s32 start_vertex;
            };
        }
    }
}