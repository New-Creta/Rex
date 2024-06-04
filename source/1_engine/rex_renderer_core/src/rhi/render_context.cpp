#include "rex_renderer_core/rhi/render_context.h"

#include "rex_renderer_core/gfx/graphics.h"

namespace rex
{
  namespace rhi
  {
    RenderContext::RenderContext(gfx::GraphicsEngine* owningEngine, rhi::CommandAllocator* alloc)
      : GraphicsContext(owningEngine, CommandType::Render)
    {
      reset(alloc);
    }

    RenderContext::~RenderContext()
    {
    }

  }
}