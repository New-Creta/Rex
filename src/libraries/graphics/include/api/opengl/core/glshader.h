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

            void bind() override;
            void unbind() override;
            bool isBound() const override;

            void uploadMat4(const std::string& name, const glm::mat4& mat) override;

        private:
            unsigned int m_shader_id;
            bool m_is_bound;
        };
    }
}