#pragma once

namespace rex
{
    namespace graphics
    {
        class Buffer
        {
        public:
            Buffer() = default;
            virtual ~Buffer() = default;

            virtual void bind() = 0;
            virtual void unbind() = 0;

            virtual bool isBound() const = 0;
        };
    }
}