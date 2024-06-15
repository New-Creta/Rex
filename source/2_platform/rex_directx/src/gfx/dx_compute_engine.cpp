#include "rex_directx/gfx/dx_compute_engine.h"

namespace rex
{
  namespace rhi
  {
    DxComputeEngine::DxComputeEngine(ResourceStateTracker* globalResourceStateTracker)
      : gfx::ComputeEngine(globalResourceStateTracker)
    {}

    // Initialize all the resources required for the compute engine
    void DxComputeEngine::init()
    {
      // Nothing to implement
    }

    // Allocate a copy context
    rsl::unique_ptr<GraphicsContext> DxComputeEngine::allocate_new_context(CommandAllocator* alloc)
    {
      wrl::ComPtr<ID3D12GraphicsCommandList> cmdlist = rhi::create_commandlist(alloc, rhi::GraphicsEngineType::Compute);

      return rsl::make_unique<rhi::DxComputeContext>(this, cmdlist);
    }

    // Prepare a new frame
    void DxComputeEngine::new_frame()
    {
      // Nothing to implement
    }
    // Finalise the frame
    void DxComputeEngine::end_frame()
    {
      flush();
    }
  }
}