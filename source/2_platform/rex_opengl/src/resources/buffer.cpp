#include "rex_opengl/resources/buffer.h"

#include "rex_engine/diagnostics/logging.h"

#if REX_PLATFORM_X64
#include "glad/glad.h"
#else
#error "Unsupported platform"
#endif

namespace rex
{
    //-----------------------------------------------------------------------
    Buffer::Buffer(u32 target)
        :m_resource_id(0)
        ,m_resource_target(target)
    {
        glGenBuffers(1, &m_resource_id);
    }
    //-----------------------------------------------------------------------
    Buffer::~Buffer()
    {
        release();
    }

    //-----------------------------------------------------------------------
    u32 Buffer::get_resource_id() const
    {
        return m_resource_id;
    }
    //-----------------------------------------------------------------------
    u32 Buffer::get_resource_target() const
    {
        return m_resource_target;
    }

    //-----------------------------------------------------------------------
    void Buffer::bind() const
    {
        glBindBuffer((GLenum)m_resource_target, m_resource_id);
    }
    //-----------------------------------------------------------------------
    void Buffer::unbind() const
    {
        glBindBuffer((GLenum)m_resource_target, 0);
    }

    //-----------------------------------------------------------------------
    void Buffer::release()
    {
        glDeleteBuffers(1, &m_resource_id);

        m_resource_id = 0;
    }

    //-----------------------------------------------------------------------
    u32 get_gl_usage(const BufferUsage& usage)
    {
        switch (usage)
        {
        case BufferUsage::STATIC_DRAW:
            return GL_STATIC_DRAW;
        case BufferUsage::DYNAMIC_DRAW:
            return GL_DYNAMIC_DRAW;
        }

        REX_ERROR("Unknown buffer usage: {0}, returing \"Static Draw\"", (s32)usage);
        return GL_STATIC_DRAW;
    }

}