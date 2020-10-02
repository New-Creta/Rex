#include "rex_graphics_pch.h"
#include "api/opengl/core/glvertexbuffer.h"

//-------------------------------------------------------------------------
rex::opengl::VertexBuffer::VertexBuffer(float* vertices, uint32_t size)
{
    glCreateBuffers(1, &m_buffer_id);
    glBindBuffer(GL_ARRAY_BUFFER, m_buffer_id);
    glBufferData(GL_ARRAY_BUFFER, size, vertices, GL_STATIC_DRAW);
}

//-------------------------------------------------------------------------
rex::opengl::VertexBuffer::~VertexBuffer()
{
    glDeleteBuffers(1, &m_buffer_id);
}

//-------------------------------------------------------------------------
void rex::opengl::VertexBuffer::bind() const
{
    glBindBuffer(GL_ARRAY_BUFFER, m_buffer_id);
}

//-------------------------------------------------------------------------
void rex::opengl::VertexBuffer::unbind() const
{
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

//-------------------------------------------------------------------------
const rex::graphics::BufferLayout& rex::opengl::VertexBuffer::getLayout() const
{
    return m_layout;
}
//-------------------------------------------------------------------------
void rex::opengl::VertexBuffer::setLayout(const graphics::BufferLayout& layout)
{
    m_layout = layout;
}
