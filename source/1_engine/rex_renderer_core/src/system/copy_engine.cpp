#include "rex_renderer_core/system/copy_engine.h"

namespace rex
{
  namespace gfx
  {
    CopyEngine::CopyEngine(ResourceStateTracker* globalResourceStateTracker)
      : GraphicsEngine(GraphicsEngineType::Copy, globalResourceStateTracker)
    {}
  }
}