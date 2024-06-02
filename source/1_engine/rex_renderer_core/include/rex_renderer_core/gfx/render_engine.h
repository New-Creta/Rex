#pragma once

#include "rex_renderer_core/gfx/graphics_engine.h"
#include "rex_renderer_core/rhi/render_context.h"

namespace rex
{
  namespace rhi
  {
    class Swapchain;
  }

  namespace gfx
  {
    class RenderEngine : public GraphicsEngine
    {
    public:
      RenderEngine()
        : GraphicsEngine(rhi::CommandType::Render)
      {}
    };
  }
}