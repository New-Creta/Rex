#pragma once

#include "rex_engine/engine/types.h"

#include "rex_directx/directx_util.h"
#include "rex_directx/resources/buffer.h"

namespace rex
{
    namespace renderer
    {
        class VertexBuffer : public Buffer
        {
        public:
            RESOURCE_CLASS_TYPE(VertexBuffer);

            size_t num_vertices() const;

            size_t vertex_stride() const;

            /**
             * Get the vertex buffer view for binding to the Input Assembler stage.
             */
            D3D12_VERTEX_BUFFER_VIEW vertex_buffer_view() const;

        protected:
            VertexBuffer(Device& device, size_t numVertices, size_t vertexStride);
            VertexBuffer(Device& device, wrl::ComPtr<ID3D12Resource> resource, size_t numVertices, size_t vertexStride);
            ~VertexBuffer() override;

            void create_vertex_buffer_view();

        private:
            size_t m_num_vertices;
            size_t m_vertex_stride;

            D3D12_VERTEX_BUFFER_VIEW m_vertex_buffer_view;
        };
    }
}