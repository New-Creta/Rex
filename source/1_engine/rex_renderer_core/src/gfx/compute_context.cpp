#include "rex_renderer_core/gfx/compute_context.h"

namespace rex
{
  namespace gfx
  {
    ComputeContext::ComputeContext(gfx::GraphicsEngine* owningEngine)
      : GraphicsContext(owningEngine, GraphicsEngineType::Compute)
    {}
  }
}