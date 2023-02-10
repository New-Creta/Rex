#include "rex_opengl/resources/vertex_buffer.h"

#if REX_PLATFORM_X64
#define GLEW_STATIC
#include "GL/glew.h"
#include "GL/wglew.h"
#else
#error "Unsupported platform"
#endif

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