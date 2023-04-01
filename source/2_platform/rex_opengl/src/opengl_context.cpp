#include "rex_renderer_core/context.h"
#include "rex_opengl/log.h"

#include <glad/gl.h>

// NOLINTBEGIN(modernize-use-nullptr,-warnings-as-errors)

namespace rex
{
  namespace context
  {
    namespace backend
    {
      //-------------------------------------------------------------------------
      bool create(void* /*unused*/)
      {
        if(gladLoaderLoadGL() == NULL)
        {
          REX_ERROR(LogOpenGL, "Glad Loader failed!");
          return false;
        }

        return true;
      }
    } // namespace backend
  }   // namespace context
} // namespace rex

// NOLINTEND(modernize-use-nullptr,-warnings-as-errors)