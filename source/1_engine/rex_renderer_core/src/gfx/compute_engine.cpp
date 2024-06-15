#include "rex_renderer_core/gfx/compute_engine.h"

namespace rex
{
  namespace gfx
  {
    ComputeEngine::ComputeEngine(rhi::ResourceStateTracker* globalResourceStateTracker)
      : GraphicsEngine(rhi::GraphicsEngineType::Compute, globalResourceStateTracker)
    {}
  }
}