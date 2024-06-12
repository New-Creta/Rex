#include "rex_renderer_core/gfx/copy_engine.h"

namespace rex
{
  namespace gfx
  {
    CopyEngine::CopyEngine()
      : GraphicsEngine(rhi::GraphicsEngineType::Copy)
    {}
  }
}