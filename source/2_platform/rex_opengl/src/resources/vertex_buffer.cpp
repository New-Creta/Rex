#include "resources/vertex_buffer.h"

#define GLAD_GL_IMPLEMENTATION
#include <glad/glad.h>

namespace rex
{
    //-----------------------------------------------------------------------
    VertexBuffer::VertexBuffer()
        :Buffer(GL_ARRAY_BUFFER)
    {

    }
    //-----------------------------------------------------------------------
    VertexBuffer::~VertexBuffer()
    {

    }

    //-----------------------------------------------------------------------
    void VertexBuffer::set_data(u64 size, const void* data, BufferUsage usage)
    {
        glBufferData(get_resource_target(), size, data, get_gl_usage(usage));
    }

}