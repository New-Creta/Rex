#pragma once

#include "rex_graphics_global.h"

#include "core/buffer.h"

namespace rex 
{
    namespace graphics
    {
        class BufferLayout;

        class VertexBuffer : public Buffer
        {
        public:
            virtual ~VertexBuffer() {}

            virtual const BufferLayout& getLayout() const = 0;
            virtual void setLayout(const BufferLayout& layout) = 0;

            REX_GRAPHICS_EXPORT static std::shared_ptr<VertexBuffer> create(float* vertices, unsigned int size);
        };
    }
}