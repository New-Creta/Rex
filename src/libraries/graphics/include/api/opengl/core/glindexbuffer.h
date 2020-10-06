#pragma once

#include "core/indexbuffer.h"

namespace rex
{
    namespace opengl
    {
        class IndexBuffer : public graphics::IndexBuffer
        {
        public:
            IndexBuffer(uint32_t* indices, uint32_t count);
            virtual ~IndexBuffer();

            void bind() override;
            void unbind() override;
            bool isBound() const override;

            uint32_t getCount() const override { return m_count; }
        private:
            uint32_t m_buffer_id;
            uint32_t m_count;
            bool m_is_bound;
        };
    }
}