#include "resources/element_buffer.h"

#define GLAD_GL_IMPLEMENTATION
#include <glad/glad.h>

namespace rex
{
    //-----------------------------------------------------------------------
    ElementBuffer::ElementBuffer()
        :Buffer(GL_ELEMENT_ARRAY_BUFFER)
    {

    }
    //-----------------------------------------------------------------------
    ElementBuffer::~ElementBuffer()
    {

    }

    //-----------------------------------------------------------------------
    void ElementBuffer::set_data(u64 size, const void* data, BufferUsage usage)
    {
        glBufferData(get_resource_target(), size, data, get_gl_usage(usage));
    }

}