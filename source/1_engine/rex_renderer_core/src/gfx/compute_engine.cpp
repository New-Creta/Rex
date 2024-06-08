#include "rex_renderer_core/gfx/compute_engine.h"

namespace rex
{
  namespace gfx
  {
    ComputeEngine::ComputeEngine()
      : GraphicsEngine(rhi::CommandType::Compute)
    {}
  }
}