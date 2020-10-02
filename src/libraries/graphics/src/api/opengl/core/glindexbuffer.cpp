#include "rex_graphics_pch.h"
#include "api/opengl/core/glindexbuffer.h"

//-------------------------------------------------------------------------
rex::opengl::IndexBuffer::IndexBuffer(uint32_t* indices, uint32_t count)
    : m_count(count)
{
    glCreateBuffers(1, &m_buffer_id);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_buffer_id);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, count * sizeof(uint32_t), indices, GL_STATIC_DRAW);
}

//-------------------------------------------------------------------------
rex::opengl::IndexBuffer::~IndexBuffer()
{
    glDeleteBuffers(1, &m_buffer_id);
}

//-------------------------------------------------------------------------
void rex::opengl::IndexBuffer::bind() const
{
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_buffer_id);
}

//-------------------------------------------------------------------------
void rex::opengl::IndexBuffer::unbind() const
{
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}
