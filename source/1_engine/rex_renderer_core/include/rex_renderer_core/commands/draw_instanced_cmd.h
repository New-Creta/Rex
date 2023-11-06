#pragma once

#include "rex_engine/types.h"

#include "rex_renderer_core/primitive_topology.h"

namespace rex
{
    namespace renderer
    {
        namespace commands
        {
            struct DrawInstanced
            {
                DrawInstanced()
                    :instance_count(0)
                    ,start_instance(REX_INVALID_INDEX)
                    ,vertex_count(0)
                    ,start_vertex(REX_INVALID_INDEX)
                    ,primitive_topology(PrimitiveTopology::NONE)
                {}

                s32 instance_count;
                s32 start_instance;
                s32 vertex_count;
                s32 start_vertex;
                PrimitiveTopology primitive_topology;
            };
        }
    }
}