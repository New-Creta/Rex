#pragma once

namespace rex 
{
    namespace graphics
    {
        class BufferLayout;

        class VertexBuffer
        {
        public:
            virtual ~VertexBuffer() {}

            virtual void bind() const = 0;
            virtual void unbind() const = 0;

            virtual const BufferLayout& getLayout() const = 0;
            virtual void setLayout(const BufferLayout& layout) = 0;

            static VertexBuffer* create(float* vertices, unsigned int size);
        };
    }
}