#pragma once

namespace rex
{
    namespace graphics
    {
        class Shader
        {
        public:
            Shader();
            virtual ~Shader();

            virtual void bind() const = 0;
            virtual void unbind() const = 0;
        };
    }
}