#pragma once

#include <glm/glm.hpp>

namespace rex
{
    namespace graphics
    {
        class VertexArray;
        class Shader;

        class RendererAPI
        {
        public:
            enum class Type
            {
                NONE = 0,
                OPENGL = 1
            };

            RendererAPI() = default;
            virtual ~RendererAPI() = default;

            virtual void clear() = 0;
            virtual void clear(const glm::vec4& clearColor, float clearDepth) = 0;

            virtual void drawIndexed(Shader* shader, graphics::VertexArray* vertexArray) = 0;
            virtual void flush() = 0;

            virtual RendererAPI::Type getAPI() const = 0;

            static std::unique_ptr<RendererAPI> create(RendererAPI::Type type);
        };
    }
}