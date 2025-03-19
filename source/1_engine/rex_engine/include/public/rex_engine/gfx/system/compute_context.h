#pragma once

#include "rex_engine/gfx/system/graphics_context.h"
#include "rex_engine/gfx/system/compute_engine.h"

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