#include "resources/vertex_array.h"

#define GLAD_GL_IMPLEMENTATION
#include <glad/glad.h>

namespace rex
{
    //-----------------------------------------------------------------------
    VertexArray::VertexArray()
        :m_resource_id(0)
    {
        glGenVertexArrays(1, &m_resource_id);
    }
    //-----------------------------------------------------------------------
    VertexArray::~VertexArray()
    {
        release();
    }

    //-----------------------------------------------------------------------
    u32 VertexArray::get_resource_id() const
    {
        return m_resource_id;
    }

    //-----------------------------------------------------------------------
    void VertexArray::bind() const
    {
        glBindVertexArray(m_resource_id);
    }
    //-----------------------------------------------------------------------
    void VertexArray::unbind() const
    {
        glBindVertexArray(0);
    }

    //-----------------------------------------------------------------------
    void VertexArray::release()
    {
        glDeleteVertexArrays(1, &m_resource_id);

        m_resource_id = 0;
    }
}