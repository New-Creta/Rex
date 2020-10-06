#pragma once

#include "rex_graphics_global.h"

#include "core/buffer.h"

namespace rex 
{
    namespace graphics
    {
        class IndexBuffer : public Buffer
        {
        public:
            virtual ~IndexBuffer() {}

            virtual unsigned int getCount() const = 0;

            REX_GRAPHICS_EXPORT static std::shared_ptr<IndexBuffer> create(unsigned int* indices, unsigned int size);
        };
    }
}