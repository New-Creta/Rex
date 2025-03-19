#include "rex_directx/system/dx_compute_engine.h"

// #TODO: Remaining cleanup of development/Pokemon -> main merge. ID: GRAPHICS

namespace rex
{
  namespace gfx
  {
    DxComputeEngine::DxComputeEngine(GpuEngine* gpuEngine, ResourceStateTracker* globalResourceStateTracker)
      : gfx::ComputeEngine(gpuEngine, globalResourceStateTracker)
    {}

    // Initialize all the resources required for the compute engine
    void DxComputeEngine::init()
    {
      // Nothing to implement
    }

    // Allocate a copy context
    rsl::unique_ptr<GraphicsContext> DxComputeEngine::allocate_new_context(CommandAllocator* alloc)
    {
      wrl::ComPtr<ID3D12GraphicsCommandList> cmdlist = dx_gal()->create_commandlist(alloc, GraphicsEngineType::Compute);

      return rsl::make_unique<DxComputeContext>(this, cmdlist);
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