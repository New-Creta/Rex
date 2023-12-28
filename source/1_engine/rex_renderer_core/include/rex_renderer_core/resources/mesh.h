#pragma once

#include "rex_engine/types.h"

#include "rex_renderer_core/renderer.h"
#include "rex_renderer_core/resource_slot.h"
#include "rex_renderer_core/index_buffer_format.h"

#include "rex_std/unordered_map.h"
#include "rex_std/bonus/string/stack_string.h"

namespace rex
{
    namespace renderer
    {
        /*
        * Defines a subrange of geometry in a MeshGeometry. This is for when multiple
        * geometries are stored in one vertex and index buffer. It provides the offsets
        * and data needed to draw a subset of geometry stores in the vertex and index
        * buffers so that we can implement the technique described in the following illustration.
        *
        *
        * +-------------------+   +----------------+   +----------------------+
        * | Sphere = {VB, IB} |   | Box = {VB, IB} |   | Cyclinder = {VB, IB} |
        * +-------------------+   +----------------+   +----------------------+
        * 
        * 
        *                 Global Vertex Buffer
        * +-----------------+--------------+--------------------+
        * | Sphere vertices | Box vertices | Cyclinder vertices |
        * +-----------------+--------------+--------------------+
        * |                 |              | 
        * |                 |              | 
        * |                 |              | 
        * 0                 |         first cylinder vertex pos
        *               first box vertex pos
        *               
        *               
        *                 Global Index Buffer
        * +----------------+-------------+-------------------+
        * | Sphere indices | Box indices | Cyclinder indices |
        * +----------------+-------------+-------------------+
        * |<-------------->|<----------->|<------------------> 
        * | num sphere     | num box     | num cylinder
        * |  indices       |  indices    |  indices
        * |                |             |        
        * |                |             |
        * 0                |          first cylinder index
        *               first box index
        * 
        * 
        * Concatination of several vertex buffers into one large vertex buffer
        * Concatination of several index buffers into one large index buffer
        * 
        */

        struct Submesh
        {
            s32 index_count = 0;
            s32 start_index_location = 0;
            s32 base_vertex_location = 0;
        };

        class Mesh
        {
        public:
            struct VertexBufferDesc
            {
                // Resource handles to the vertex buffer of this geometry
                ResourceSlot        slot;

                // Data about the allocated buffer.
                s32                 byte_stride;
                s32                 byte_size;
            };
            struct IndexBufferDesc
            {
                // Resource handles to the index buffer of this geometry
                ResourceSlot        slot;

                // Data about the allocated buffers.
                IndexBufferFormat   format;
                s32                 byte_size;
            };

        public:
            Mesh(const rsl::medium_stack_string& name, const VertexBufferDesc& vbd, const IndexBufferDesc& ibd);
            ~Mesh();

        public:
            void add_submesh(const rsl::small_stack_string& name, const Submesh& subMesh);
            void add_submesh(const rsl::small_stack_string& name, s32 indexCount, s32 startIndexLocation, s32 baseVertexLocation);

            const Submesh* submesh(const rsl::small_stack_string& name) const;

        public:
            const rsl::medium_stack_string& name() const;

            const ResourceSlot& vertex_buffer_slot() const;
            s32 vertex_buffer_byte_stride() const;
            s32 vertex_buffer_byte_size() const;
            
            const ResourceSlot& index_buffer_slot() const;
            IndexBufferFormat index_buffer_format() const;
            s32 index_buffer_byte_size() const;

        private:
            // Give it a name so we can look it up by name.
            rsl::medium_stack_string m_name;

            VertexBufferDesc m_vbd;
            IndexBufferDesc m_ibd;

            // A Mesh may store multiple geometries in one vertex/index buffer.
            // Use this container to define the Submesh geometries so we can draw
            // the Submeshes individually.
            rsl::unordered_map<rsl::small_stack_string, Submesh> m_submesh_map;
        };
    }
}