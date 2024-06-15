#include "rex_renderer_core/gfx/copy_engine.h"

namespace rex
{
  namespace gfx
  {
    CopyEngine::CopyEngine(rhi::ResourceStateTracker* globalResourceStateTracker)
      : GraphicsEngine(rhi::GraphicsEngineType::Copy, globalResourceStateTracker)
    {}
  }
}