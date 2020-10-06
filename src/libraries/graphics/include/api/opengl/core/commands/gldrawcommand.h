#pragma once

#include "core/commands/drawcommand.h"
#include "core/vertexarray.h"

namespace rex
{
    namespace graphics
    {
        class VertexArray;
        class Shader;
    }

    namespace opengl
    {
        class VertexArrayDrawCommand : public graphics::DrawCommand
        {
        public:
            VertexArrayDrawCommand(graphics::Shader* shader, graphics::VertexArray* vertexArray);
            ~VertexArrayDrawCommand() override;

            void execute() override;

        private:
            graphics::Shader* m_shader;
            graphics::VertexArray* m_vertex_array;
        };
    }
}