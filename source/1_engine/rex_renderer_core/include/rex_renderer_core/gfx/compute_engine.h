#pragma once

#include "rex_renderer_core/gfx/graphics_engine.h"
#include "rex_renderer_core/rhi/compute_context.h"

namespace rex
{
  namespace gfx
  {
    class ComputeEngine : public GraphicsEngine
    {
    public:
      ComputeEngine()
        : GraphicsEngine(rhi::CommandType::Compute)
      {}
    };
  }
}