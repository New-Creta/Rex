#pragma once

#include "core/buffer.h"

#include <glm/glm.hpp>

namespace rex
{
    namespace graphics
    {
        class Shader : public Buffer
        {
        public:
            Shader();
            ~Shader() override;

            virtual void uploadMat4(const std::string& name, const glm::mat4& mat) = 0;
        };
    }
}