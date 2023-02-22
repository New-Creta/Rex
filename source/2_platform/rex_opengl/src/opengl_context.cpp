#include "rex_renderer_core/context.h"

#include "rex_engine/diagnostics/logging.h"

#include <glad/gl.h>

// NOLINTBEGIN(modernize-use-nullptr,-warnings-as-errors)

namespace rex
{
  namespace context
  {
    namespace backend
    {
      //-------------------------------------------------------------------------
      bool create(void* )
      {
        if (gladLoaderLoadGL() == NULL)
        {
          REX_ERROR("Glad Loader failed!");
          return false;
        }

        return true;
      }
    }
  }
}

// NOLINTEND(modernize-use-nullptr,-warnings-as-errors)