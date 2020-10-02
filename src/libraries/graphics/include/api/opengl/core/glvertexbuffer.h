#pragma once

#include "core/vertexbuffer.h"
#include "core/bufferlayout.h"

namespace rex
{
    namespace opengl
    {
        class VertexBuffer : public graphics::VertexBuffer
        {
        public:
            VertexBuffer(float* vertices, uint32_t size);
            virtual ~VertexBuffer();

            void bind() const override;
            void unbind() const override;

            const graphics::BufferLayout& getLayout() const override;
            void setLayout(const graphics::BufferLayout& layout) override;
        private:
            uint32_t m_buffer_id;
            graphics::BufferLayout m_layout;
        };
    }
}