#include "rex_renderer_core/renderer.h"

#include "rex_engine/defines.h"

namespace rex
{
  namespace renderer
  {
    //-------------------------------------------------------------------------
    bool initialize(const RendererOutputWindowUserData& userData, u32 maxCommands)
    {
      UNUSED_PARAM(userData);
      UNUSED_PARAM(maxCommands);

      return backend::initialize(userData);
    }
    //-------------------------------------------------------------------------
    void shutdown()
    {
      backend::shutdown();
    }

  } // namespace renderer
} // namespace rex