#pragma once

#include "rex_engine/defines.h"
#include "rex_engine/diagnostics/logging.h"

#if REX_PLATFORM_X64
#include <glad/gl.h>
#else
#error "Unsupported platform"
#endif

#if defined REX_COMPILER_MSVC
    #pragma warning( push )
    #pragma warning( disable : 4505 ) // 'function' : unreferenced local function has been removed
#endif

namespace rex
{
    namespace opengl
    {
#ifdef REX_DEBUG
        //-------------------------------------------------------------------------
        inline char8* get_error_message(u32 error)
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
        inline u32 get_current_error()
        {
            // This wil also clear the error with GL_NO_ERROR when the value is retrieved.
            return glGetError();
        }
        //-------------------------------------------------------------------------
        inline bool log_call(const char* function, const char* file, int32 line)
        {
            u32 error = get_current_error();
            if (error == GL_NO_ERROR)
            {
                return true;
            }

            while (error != GL_NO_ERROR)
            {
                REX_ERROR("[OpenGL Error] ({0}, {1}): {2} {3}: {4}", error, get_error_message(error), function, file, line);
                
                error = get_current_error();
            }

            return false;
        }
#endif
    }
}

#ifdef REX_DEBUG
    #define GL_CALL(x) rex::opengl::get_current_error();\
		    x; \
		    if (!rex::opengl::log_call(#x, __FILE__, __LINE__)) DEBUG_BREAK();
#else
    #define GL_CALL(x) x
#endif

#if defined REX_COMPILER_MSVC
    #pragma warning( pop )
#endif