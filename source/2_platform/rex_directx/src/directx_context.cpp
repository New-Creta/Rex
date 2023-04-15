#include "rex_directx/log.h"
#include "rex_renderer_core/context.h"

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
        return true;
      }
    } // namespace backend
  }   // namespace context
} // namespace rex

// NOLINTEND(modernize-use-nullptr,-warnings-as-errors)