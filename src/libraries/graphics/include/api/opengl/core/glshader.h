#pragma once

#include "core/shader.h"

namespace rex
{
    namespace opengl
    {
        class Shader : public graphics::Shader
        {
        public:
            Shader(const std::string& vertexSource, const std::string& fragmentSource);
            ~Shader();

            void bind() const override;
            void unbind() const override;

        private:
            unsigned int m_shader_id;
        };
    }
}