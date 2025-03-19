#include "rex_engine/gfx/system/render_context.h"

#include "rex_engine/gfx/graphics.h"

namespace rex
{
  namespace gfx
  {
    RenderContext::RenderContext(gfx::GraphicsEngine* owningEngine)
      : GraphicsContext(owningEngine, GraphicsEngineType::Render)
    {
    }
    void RenderContext::set_render_target(RenderTarget* colorRenderTarget)
    {
      set_render_target(colorRenderTarget, nullptr);
    }
    void RenderContext::clear_render_target(RenderTarget* renderTarget)
    {
      clear_render_target(renderTarget, nullptr);
    }
    void RenderContext::set_vertex_buffer(VertexBuffer* vb)
    {
      set_vertex_buffer(vb, 0);
    }

    // Reset the engine specific context
    void RenderContext::type_specific_reset(const ContextResetData& resetData)
    {
      // By default we set the current backbuffer as render target
      set_render_target(resetData.current_backbuffer_rt);
    }
  }
}