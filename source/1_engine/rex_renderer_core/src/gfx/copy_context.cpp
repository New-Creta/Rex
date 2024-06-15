#include "rex_renderer_core/gfx/copy_context.h"

#include "rex_renderer_core/system/copy_engine.h"

namespace rex
{
  namespace gfx
  {
    CopyContext::CopyContext(gfx::GraphicsEngine* owningEngine)
      : GraphicsContext(owningEngine, GraphicsEngineType::Copy)
    {}
  }
}