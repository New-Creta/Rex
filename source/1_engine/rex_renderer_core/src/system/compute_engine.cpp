#include "rex_renderer_core/system/compute_engine.h"

namespace rex
{
  namespace gfx
  {
    ComputeEngine::ComputeEngine(ResourceStateTracker* globalResourceStateTracker)
      : GraphicsEngine(GraphicsEngineType::Compute, globalResourceStateTracker)
    {}
  }
}