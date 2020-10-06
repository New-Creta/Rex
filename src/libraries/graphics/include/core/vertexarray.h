#pragma once

#include "rex_graphics_global.h"

#include "core/buffer.h"

namespace rex
{
    namespace graphics
    {
        class VertexBuffer;
        class IndexBuffer;

        class VertexArray : public Buffer
        {
        public:
            REX_GRAPHICS_EXPORT VertexArray();
            REX_GRAPHICS_EXPORT virtual ~VertexArray();

            virtual void addVertexBuffer(const std::shared_ptr<graphics::VertexBuffer>& vertexBuffer) = 0;
            virtual void setIndexBuffer(const std::shared_ptr<graphics::IndexBuffer>& indexBuffer) = 0;

            virtual const std::vector<std::shared_ptr<VertexBuffer>>& getVertexBuffers() const = 0;
            virtual const std::shared_ptr<IndexBuffer>& getIndexBuffer() const = 0;

            REX_GRAPHICS_EXPORT static std::shared_ptr<VertexArray> create();
        };
    }
}