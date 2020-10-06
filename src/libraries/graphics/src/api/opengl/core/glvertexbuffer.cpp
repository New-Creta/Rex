#include "rex_graphics_pch.h"
#include "api/opengl/core/glvertexbuffer.h"

//-------------------------------------------------------------------------
rex::opengl::VertexBuffer::VertexBuffer(float* vertices, uint32_t size)
    :m_is_bound(false)
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
void rex::opengl::VertexBuffer::bind()
{
    glBindBuffer(GL_ARRAY_BUFFER, m_buffer_id);
    m_is_bound = true;
}

//-------------------------------------------------------------------------
void rex::opengl::VertexBuffer::unbind()
{
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    m_is_bound = false;
}

//-------------------------------------------------------------------------
bool rex::opengl::VertexBuffer::isBound() const
{
    return m_is_bound;
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
