#include "rex_directx/system/dx_render_engine.h"

namespace rex
{
  namespace gfx
  {
    DxRenderEngine::DxRenderEngine(ResourceStateTracker* globalResourceStateTracker)
      : gfx::RenderEngine(globalResourceStateTracker)
    {}

    // Initialize all the resources required for the compute engine
    void DxRenderEngine::init()
    {
      // Nothing to implement
    }

    // Prepare a new frame
    void DxRenderEngine::new_frame()
    {
      // Nothing to implement
    }
    // Finalise the frame
    void DxRenderEngine::end_frame()
    {
      flush();
    }

    // Allocate a render context
    rsl::unique_ptr<GraphicsContext> DxRenderEngine::allocate_new_context(CommandAllocator* alloc)
    {
      wrl::ComPtr<ID3D12GraphicsCommandList> cmdlist = dx_gal()->create_commandlist(alloc, GraphicsEngineType::Render);

      cmdlist->Close();
      return rsl::make_unique<DxRenderContext>(this, cmdlist);
    }
  }
}