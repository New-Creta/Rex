#include "rex_engine/gfx/system/compute_context.h"

namespace rex
{
  namespace gfx
  {
    ComputeContext::ComputeContext(gfx::GraphicsEngine* owningEngine)
      : GraphicsContext(owningEngine, GraphicsEngineType::Compute)
    {}
  }
}