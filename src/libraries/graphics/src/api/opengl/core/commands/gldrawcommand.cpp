#include "rex_graphics_pch.h"

#include "core/vertexarray.h"
#include "core/indexbuffer.h"
#include "core/shader.h"

#include "api/opengl/core/commands/gldrawcommand.h"

//-------------------------------------------------------------------------
rex::opengl::VertexArrayDrawCommand::VertexArrayDrawCommand(graphics::Shader* shader, rex::graphics::VertexArray* vertexArray)
    :m_shader(shader)
    ,m_vertex_array(vertexArray)
{}
//-------------------------------------------------------------------------
rex::opengl::VertexArrayDrawCommand::~VertexArrayDrawCommand() = default;

//-------------------------------------------------------------------------
void rex::opengl::VertexArrayDrawCommand::execute()
{
    m_shader->bind();
    m_vertex_array->bind();

    glDrawElements(GL_TRIANGLES, m_vertex_array->getIndexBuffer()->getCount(), GL_UNSIGNED_INT, nullptr);
}

