#include "rex_graphics_pch.h"

#include "api/opengl/core/glrendererapi.h"

#include "api/opengl/core/commands/glclearcommand.h"
#include "api/opengl/core/commands/gldrawcommand.h"

//-------------------------------------------------------------------------
rex::opengl::RendererAPI::RendererAPI() = default;
//-------------------------------------------------------------------------
rex::opengl::RendererAPI::~RendererAPI() = default;

//-------------------------------------------------------------------------
void rex::opengl::RendererAPI::clear()
{
    clear(glm::vec4(0.1f, 0.1f, 0.1f, 1.0f), 0.0f);
}
//-------------------------------------------------------------------------
void rex::opengl::RendererAPI::clear(const glm::vec4& color, float depth)
{
    m_commands.push_back(std::make_unique<opengl::ClearCommand>(color, depth));
}

//-------------------------------------------------------------------------
void rex::opengl::RendererAPI::drawIndexed(graphics::Shader* shader, graphics::VertexArray* vertexArray)
{
    m_commands.push_back(std::make_unique<VertexArrayDrawCommand>(shader, vertexArray));
}

//-------------------------------------------------------------------------
void rex::opengl::RendererAPI::flush()
{
    for (auto& command : m_commands)
        command->execute();

    m_commands.clear();
}

//-------------------------------------------------------------------------
rex::opengl::RendererAPI::Type rex::opengl::RendererAPI::getAPI() const
{
    return RendererAPI::Type::OPENGL;
}