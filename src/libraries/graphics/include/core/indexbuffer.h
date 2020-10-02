#pragma once

namespace rex 
{
    namespace graphics
    {
        class IndexBuffer
        {
        public:
            virtual ~IndexBuffer() {}

            virtual void bind() const = 0;
            virtual void unbind() const = 0;

            virtual unsigned int getCount() const = 0;

            static IndexBuffer* create(unsigned int* indices, unsigned int size);
        };
    }
}