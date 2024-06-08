#include "rex_directx/gfx/dx_compute_engine.h"

namespace rex
{
  namespace rhi
  {
    void DxComputeEngine::init()
    {
      // Nothing to implement
    }

    rsl::unique_ptr<GraphicsContext> DxComputeEngine::allocate_new_context(rhi::CommandAllocator* alloc)
    {
      wrl::ComPtr<ID3D12GraphicsCommandList> cmdlist = rhi::create_commandlist(alloc, rhi::CommandType::Compute);

      return rsl::make_unique<rhi::DxComputeContext>(this, cmdlist, alloc);
    }

  }
}