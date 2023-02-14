#include "rex_opengl/resources/element_buffer.h"

#if REX_PLATFORM_X64
#include <glad/gl.h>
#else
#error "Unsupported platform"
#endif

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