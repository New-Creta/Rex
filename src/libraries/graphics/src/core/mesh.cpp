#include "rex_graphics_pch.h"

#include "core/vertexarray.h"
#include "core/mesh.h"

//-------------------------------------------------------------------------
rex::graphics::Mesh::Mesh(Shader* shader)
    :m_shader(shader)
{}

//-------------------------------------------------------------------------
void rex::graphics::Mesh::setVertexArray(const std::shared_ptr<VertexArray>& vertexArray)
{
    m_vertex_array = vertexArray;
}

//-------------------------------------------------------------------------
void rex::graphics::Mesh::setVertexBuffer(const std::shared_ptr<VertexBuffer>& vertexBuffer)
{
    m_vertex_buffer = vertexBuffer;
    m_vertex_array->addVertexBuffer(m_vertex_buffer);
}

//-------------------------------------------------------------------------
void rex::graphics::Mesh::setIndexBuffer(const std::shared_ptr<IndexBuffer>& indexBuffer)
{
    m_index_buffer = indexBuffer;
    m_vertex_array->setIndexBuffer(m_index_buffer);
}
