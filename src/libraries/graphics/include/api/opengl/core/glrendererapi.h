#pragma once

#include "core/rendererapi.h"

namespace rex
{
    namespace graphics
    {
        class RenderCommand;

        class VertexArray;
        class Shader;
    }
    namespace opengl
    {
        class RendererAPI : public graphics::RendererAPI
        { 
        public:
            RendererAPI();
            ~RendererAPI() override;

            void clear() override;
            void clear(const glm::vec4& clearColor, float clearDepth) override;

            void drawIndexed(graphics::Shader* shader, graphics::VertexArray* vertexArray) override;
            void flush() override;

            RendererAPI::Type getAPI() const override;

        private:
            std::vector<std::unique_ptr<graphics::RenderCommand>> m_commands;
        };
    }
}