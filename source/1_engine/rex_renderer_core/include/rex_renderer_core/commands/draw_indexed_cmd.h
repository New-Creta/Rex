#pragma once

#include "rex_engine/types.h"

#include "rex_renderer_core/primitive_topology.h"

namespace rex
{
    namespace renderer
    {
        namespace commands
        {
            struct DrawIndexed
            {
                DrawIndexed()
                    : index_count(0)
                    , start_index(REX_INVALID_INDEX)
                    , base_vertex(REX_INVALID_INDEX)
                    , primitive_topology(PrimitiveTopology::NONE)
                {}

                u32 index_count;
                u32 start_index;
                u32 base_vertex;
                PrimitiveTopology primitive_topology;
            };
        }
    }
}