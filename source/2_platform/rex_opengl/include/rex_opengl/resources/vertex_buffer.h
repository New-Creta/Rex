#pragma once

#include "resources/buffer.h"

namespace rex
{
    class VertexBuffer : public Buffer
    {
    public:
        VertexBuffer();
        ~VertexBuffer() override;

        void set_data(u64 size, const void* data, BufferUsage usage = BufferUsage::STATIC_DRAW);
    };
}