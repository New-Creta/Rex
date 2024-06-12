#pragma once

#include "rex_renderer_core/rhi/graphics_context.h"
#include "rex_renderer_core/gfx/compute_engine.h"

namespace rex
{
  namespace rhi
  {
    class ComputeContext : public GraphicsContext
    {
    public:
      ComputeContext(gfx::GraphicsEngine* owningEngine);

      // Nothing to implement yet.
    };
  }
}