#pragma once

#include "core/vertexarray.h"

namespace rex
{
    namespace graphics
    {
        class VertexBuffer;
        class IndexBuffer;
    }

    namespace opengl
    {
        class VertexArray : public graphics::VertexArray
        {
        public:
            VertexArray();
            ~VertexArray() override;

            void bind() override;
            void unbind() override;
            bool isBound() const override;

            virtual void addVertexBuffer(const std::shared_ptr<graphics::VertexBuffer>& vertexBuffer) override;
            virtual void setIndexBuffer(const std::shared_ptr<graphics::IndexBuffer>& indexBuffer) override;

            virtual const std::vector<std::shared_ptr<graphics::VertexBuffer>>& getVertexBuffers() const override;
            virtual const std::shared_ptr<graphics::IndexBuffer>& getIndexBuffer() const override;

        private:
            unsigned int m_array_id;
            bool m_is_bound;

            std::vector<std::shared_ptr<graphics::VertexBuffer>> m_vertex_buffers;
            std::shared_ptr<graphics::IndexBuffer> m_index_buffer;
        };
    }
}