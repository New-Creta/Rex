#pragma once

#include "rex_renderer_core/gfx/graphics_engine.h"
#include "rex_renderer_core/rhi/render_context.h"

namespace rex
{
  namespace gfx
  {
    // The base graphics engine type used for submitting render commands
    class RenderEngine : public GraphicsEngine
    {
    public:
      RenderEngine();

    private:

    };
  }
}