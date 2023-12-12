#pragma once

#include "rex_engine/types.h"

#include "rex_renderer_core/renderer.h"
#include "rex_renderer_core/index_buffer_format.h"

#include <rex_std/unordered_map.h>
#include <rex_std/bonus/string/stack_string.h>

namespace rex
{
    namespace renderer
    {
        // Defines a subrange of geometry in a MeshGeometry.  This is for when multiple
        // geometries are stored in one vertex and index buffer.  It provides the offsets
        // and data needed to draw a subset of geometry stores in the vertex and index
        // buffers so that we can implement the technique described by Figure 6.3.
        struct Submesh
        {
            s32 index_count = 0;
            s32 start_index_location = 0;
            s32 base_vertex_location = 0;
        };

        struct Mesh
        {
            // Give it a name so we can look it up by name.
            rsl::medium_stack_string name;

            // System memory copies.  Use Blobs because the vertex/index format can be generic.
            // It is up to the client to cast appropriately.
            ResourceSlot vertex_buffer;
            ResourceSlot index_buffer;

            // Data about the buffers.
            s32 vertex_byte_stride = 0;
            s32 vertex_buffer_byte_size = 0;
            renderer::IndexBufferFormat index_format = renderer::IndexBufferFormat::R16_UINT;
            s32 index_buffer_byte_size = 0;

            // A MeshGeometry may store multiple geometries in one vertex/index buffer.
            // Use this container to define the Submesh geometries so we can draw
            // the Submeshes individually.
            rsl::unordered_map<rsl::small_stack_string, Submesh> draw_args;
        };
    }
}