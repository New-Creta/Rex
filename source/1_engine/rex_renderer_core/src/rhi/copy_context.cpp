#include "rex_renderer_core/rhi/copy_context.h"

#include "rex_renderer_core/gfx/copy_engine.h"

namespace rex
{
  namespace rhi
  {
    CopyContext::CopyContext(gfx::GraphicsEngine* owningEngine)
      : GraphicsContext(owningEngine, GraphicsEngineType::Copy)
    {}
  }
}