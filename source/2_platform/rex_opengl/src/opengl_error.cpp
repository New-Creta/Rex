#include "rex_opengl/opengl_error.h"

#ifdef REX_DEBUG

  #include "rex_engine/diagnostics/logging.h"

  #if REX_PLATFORM_X64
    #include <glad/gl.h>
  #else
    #error "Unsupported platform"
  #endif

namespace rex
{
  namespace opengl
  {
    //-------------------------------------------------------------------------
    rsl::string_view get_error_message(u32 error)
    {
      switch(error)
      {
        case GL_INVALID_OPERATION: return "Invalid Operation";
        case GL_INVALID_ENUM: return "Invalid Enum";
        case GL_INVALID_VALUE: return "Invalid Value";
        case GL_INVALID_INDEX: return "Invalid Index";
        case GL_INVALID_FRAMEBUFFER_OPERATION: return "Invalid Framebuffer Operation";
        default:
          // Nothing to implement
          break;
      }

      return "Unknown error";
    }

    //-------------------------------------------------------------------------
    u32 get_current_error()
    {
      // This wil also clear the error with GL_NO_ERROR when the value is retrieved.
      return glGetError();
    }
    //-------------------------------------------------------------------------
    bool log_call(const char* function, const char* file, int32 line)
    {
      u32 error = get_current_error();
      if(error == GL_NO_ERROR)
      {
        return true;
      }

      while(error != GL_NO_ERROR)
      {
        REX_ERROR("[OpenGL Error] ({0}, {1}): {2} {3}: {4}", error, get_error_message(error), function, file, line);

        error = get_current_error();
      }

      return false;
    }
  } // namespace opengl
} // namespace rex

#endif