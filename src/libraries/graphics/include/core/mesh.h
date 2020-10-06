#pragma once

#include "rex_graphics_global.h"

namespace rex
{
    namespace graphics
    {
        class Shader;
        class VertexArray;
        class VertexBuffer;
        class IndexBuffer;

        class Mesh
        {
        public:
            Mesh(Shader* shader);

            inline Shader* getShader() const { return m_shader; }

            inline VertexArray* getVertexArray() const { return m_vertex_array.get(); }
            inline VertexBuffer* getVertexBuffer() const { return m_vertex_buffer.get(); }
            inline IndexBuffer* getIndexBuffer() const { return m_index_buffer.get(); }

        protected:
            REX_GRAPHICS_EXPORT void setVertexArray(const std::shared_ptr<VertexArray>& vertexArray);
            REX_GRAPHICS_EXPORT void setVertexBuffer(const std::shared_ptr<VertexBuffer>& vertexBuffer);
            REX_GRAPHICS_EXPORT void setIndexBuffer(const std::shared_ptr<IndexBuffer>& indexBuffer);

        private:
            std::shared_ptr<VertexArray> m_vertex_array;
            std::shared_ptr<VertexBuffer> m_vertex_buffer;
            std::shared_ptr<IndexBuffer> m_index_buffer;

            Shader* m_shader;
        };
    }
}