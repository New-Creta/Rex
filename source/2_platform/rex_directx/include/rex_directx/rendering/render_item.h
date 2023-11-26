#pragma once

#include "rex_engine/types.h"

#include "rex_renderer_core/primitive_topology.h"
#include "rex_renderer_core/resources/mesh.h"

#include <DirectXMath.h>

namespace rex
{
    namespace renderer
    {
        struct RenderItem
        {
            DirectX::XMFLOAT4X4 world;

            u32 constant_buffer_index;

            Mesh* geometry;

            renderer::PrimitiveTopology topology;

            s32 index_count;
            s32 start_index_location;
            s32 base_vertex_location;

            s32 num_frames_dirty;
        };
    }
}