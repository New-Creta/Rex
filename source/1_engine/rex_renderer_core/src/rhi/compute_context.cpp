#include "rex_renderer_core/rhi/compute_context.h"

namespace rex
{
  namespace rhi
  {
    ComputeContext::ComputeContext(gfx::GraphicsEngine* owningEngine)
      : GraphicsContext(owningEngine, CommandType::Compute)
    {}
  }
}