#pragma once

#include "rex_engine/defines.h"
#include "rex_engine/diagnostics/logging.h"

#if REX_PLATFORM_X64
#include "glad/glad.h"
#else
#error "Unsupported platform"
#endif

#if defined _MSC_VER
    #pragma warning( push )
    #pragma warning( disable : 4505 )
#endif

namespace rex
{
#ifdef REX_DEBUG
    //-------------------------------------------------------------------------
    inline char8* gl_get_error_message(u32 error)
    {
        switch (error)
        {
        case GL_INVALID_OPERATION: return "Invalid Operation";
        case GL_INVALID_ENUM: return "Invalid Enum";
        case GL_INVALID_VALUE: return "Invalid Value";
        case GL_INVALID_INDEX: return "Invalid Index";
        case GL_INVALID_FRAMEBUFFER_OPERATION: return "Invalid Framebuffer Operation";
        }

        return "Unknown error";
    }

    //-------------------------------------------------------------------------
    inline u32 gl_check_error()
    {
        return glGetError();
    }
    //-------------------------------------------------------------------------
    inline bool gl_log_call(const char* function, const char* file, int32 line)
    {
        u32 error = gl_check_error();
        if (error == GL_NO_ERROR)
        {
            return true;
        }

        while (error != GL_NO_ERROR)
        {
            REX_ERROR("[OpenGL Error] ({0}, {1}): {2} {3}: {4}", error, gl_get_error_message(error), function, file, line);
            
            error = gl_check_error();
        }

        return false;
    }
#endif
}

#ifdef REX_DEBUG
    #define GL_CALL(x) rex::gl_check_error();\
		    x; \
		    if (!rex::gl_log_call(#x, __FILE__, __LINE__)) DEBUG_BREAK();
#else
    #define GL_CALL(x) x
#endif

#if defined _MSC_VER
    #pragma warning( pop )
#endif