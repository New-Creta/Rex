#include "rex_graphics_pch.h"

#include "core/shaderdatatype.h"
#include "core/bufferlayout.h"
#include "core/indexbuffer.h"
#include "core/vertexbuffer.h"

#include "api/opengl/core/glvertexarray.h"

namespace
{
    //-------------------------------------------------------------------------
    GLenum shaderDataTypeToOpenGLBaseType(rex::graphics::ShaderDataType type)
    {
        switch (type)
        {
        case rex::graphics::ShaderDataType::FLOAT:    return GL_FLOAT;
        case rex::graphics::ShaderDataType::FLOAT2:   return GL_FLOAT;
        case rex::graphics::ShaderDataType::FLOAT3:   return GL_FLOAT;
        case rex::graphics::ShaderDataType::FLOAT4:   return GL_FLOAT;
        case rex::graphics::ShaderDataType::MAT3:     return GL_FLOAT;
        case rex::graphics::ShaderDataType::MAT4:     return GL_FLOAT;
        case rex::graphics::ShaderDataType::INT:      return GL_INT;
        case rex::graphics::ShaderDataType::INT2:     return GL_INT;
        case rex::graphics::ShaderDataType::INT3:     return GL_INT;
        case rex::graphics::ShaderDataType::INT4:     return GL_INT;
        case rex::graphics::ShaderDataType::BOOL:     return GL_BOOL;
        }

        RX_ASSERT_X(false, "Unknown ShaderDataType!");
        return 0;
    }

    //-------------------------------------------------------------------------
    unsigned int getComponentCount(rex::graphics::ShaderDataType type)
    {
        switch (type)
        {
        case rex::graphics::ShaderDataType::FLOAT:   return 1;
        case rex::graphics::ShaderDataType::FLOAT2:  return 2;
        case rex::graphics::ShaderDataType::FLOAT3:  return 3;
        case rex::graphics::ShaderDataType::FLOAT4:  return 4;
        case rex::graphics::ShaderDataType::MAT3:    return 3 * 3;
        case rex::graphics::ShaderDataType::MAT4:    return 4 * 4;
        case rex::graphics::ShaderDataType::INT:     return 1;
        case rex::graphics::ShaderDataType::INT2:    return 2;
        case rex::graphics::ShaderDataType::INT3:    return 3;
        case rex::graphics::ShaderDataType::INT4:    return 4;
        case rex::graphics::ShaderDataType::BOOL:    return 1;
        }

        RX_ASSERT_X(false, "Unknown ShaderDataType!");
        return 0;
    }
}

//-------------------------------------------------------------------------
rex::opengl::VertexArray::VertexArray()
    :m_is_bound(false)
{
    glCreateVertexArrays(1, &m_array_id);
}

//-------------------------------------------------------------------------
rex::opengl::VertexArray::~VertexArray()
{
    glDeleteVertexArrays(1, &m_array_id);
}

//-------------------------------------------------------------------------
void rex::opengl::VertexArray::bind()
{
    glBindVertexArray(m_array_id);
    m_is_bound = true;
}

//-------------------------------------------------------------------------
void rex::opengl::VertexArray::unbind()
{
    glBindVertexArray(0);
    m_is_bound = false;
}

//-------------------------------------------------------------------------
bool rex::opengl::VertexArray::isBound() const
{
    return m_is_bound;
}

//-------------------------------------------------------------------------
void rex::opengl::VertexArray::addVertexBuffer(const std::shared_ptr<graphics::VertexBuffer>& vertexBuffer)
{
    RX_ASSERT_X(vertexBuffer->getLayout().getElements().size(), "Vertex Buffer has no layout!");

    glBindVertexArray(m_array_id);
    vertexBuffer->bind();

    uint32_t index = 0;
    const auto& layout = vertexBuffer->getLayout();
    for (const auto& element : layout)
    {
        glEnableVertexAttribArray(index);
        glVertexAttribPointer(index,
            getComponentCount(element.type),
            shaderDataTypeToOpenGLBaseType(element.type),
            element.normalized ? GL_TRUE : GL_FALSE,
            layout.getStride(),
#pragma warning( push )
#pragma warning( disable : 4312 )
            (const void*)element.offset);
#pragma warning( pop )
        index++;
    }

    m_vertex_buffers.push_back(vertexBuffer);
}

//-------------------------------------------------------------------------
void rex::opengl::VertexArray::setIndexBuffer(const std::shared_ptr<graphics::IndexBuffer>& indexBuffer)
{
    glBindVertexArray(m_array_id);
    indexBuffer->bind();

    m_index_buffer = indexBuffer;
}

//-------------------------------------------------------------------------
const std::vector<std::shared_ptr<rex::graphics::VertexBuffer>>& rex::opengl::VertexArray::getVertexBuffers() const
{
    return m_vertex_buffers;
}

//-------------------------------------------------------------------------
const std::shared_ptr<rex::graphics::IndexBuffer>& rex::opengl::VertexArray::getIndexBuffer() const
{
    return m_index_buffer;
}
