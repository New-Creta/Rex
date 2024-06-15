#pragma once

#include "rex_renderer_core/gfx/graphics_context.h"
#include "rex_renderer_core/system/compute_engine.h"

namespace rex
{
  namespace gfx
  {
    class ComputeContext : public GraphicsContext
    {
    public:
      ComputeContext(gfx::GraphicsEngine* owningEngine);

      // Nothing to implement yet.
    };
  }
}