#include "rex_renderer_core/rhi/render_context.h"

#include "rex_renderer_core/gfx/graphics.h"

namespace rex
{
  namespace rhi
  {
    RenderContext::RenderContext(gfx::GraphicsEngine<RenderContext>* owningEngine, rhi::CommandAllocator* alloc)
      : GraphicsContext(CommandType::Render)
      , m_owning_engine(owningEngine)
      , m_cmd_allocator(alloc)
    {
            
    }

    RenderContext::~RenderContext()
    {
    }
  }
}