#pragma once

#include "rex_renderer_core/gfx/render_engine.h"
#include "rex_directx/system/dx_render_context.h"
#include "rex_directx/system/dx_rhi.h"

namespace rex
{
  namespace rhi
  {
    class DxRenderEngine : public gfx::RenderEngine
    {
    public:
      void post_init() override
      {

      }

    protected:
      rsl::unique_ptr<GraphicsContext> allocate_new_context(rhi::CommandAllocator* alloc) override
      {
        wrl::ComPtr<ID3D12GraphicsCommandList> cmdlist = rhi::create_commandlist(alloc, rhi::CommandType::Render);

        cmdlist->Close();
        return rsl::make_unique<rhi::DxRenderContext>(this, cmdlist, alloc);
      }

    private:

    };
  }
}