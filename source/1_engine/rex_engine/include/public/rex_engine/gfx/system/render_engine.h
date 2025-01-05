#pragma once

#include "rex_engine/gfx/system/graphics_engine.h"
#include "rex_engine/gfx/system/render_context.h"

namespace rex
{
  namespace gfx
  {
    // The base graphics engine type used for submitting render commands
    class RenderEngine : public GraphicsEngine
    {
    public:
      RenderEngine(ResourceStateTracker* globalResourceStateTracker);

    private:

    };
  }
}