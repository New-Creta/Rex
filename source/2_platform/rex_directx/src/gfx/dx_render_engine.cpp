#include "rex_directx/gfx/dx_render_engine.h"

namespace rex
{
  namespace rhi
  {
    void DxRenderEngine::post_init()
    {
      // Nothing to implement
    }

    rsl::unique_ptr<GraphicsContext> DxRenderEngine::allocate_new_context(rhi::CommandAllocator* alloc)
    {
      wrl::ComPtr<ID3D12GraphicsCommandList> cmdlist = rhi::create_commandlist(alloc, rhi::CommandType::Render);

      cmdlist->Close();
      return rsl::make_unique<rhi::DxRenderContext>(this, cmdlist, alloc);
    }
  }
}