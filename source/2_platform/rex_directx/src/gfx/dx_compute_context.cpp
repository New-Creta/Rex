#include "rex_directx/gfx/dx_compute_context.h"

#include "rex_directx/system/dx_command_allocator.h"
#include "rex_directx/system/dx_descriptor_heap.h"

#include "rex_directx/resources/dx_pipeline_state.h"

#include "WinPixEventRuntime/pix3.h"

namespace rex
{
  namespace gfx
  {
    DxComputeContext::DxComputeContext(gfx::GraphicsEngine* owningEngine, const wrl::ComPtr<ID3D12GraphicsCommandList> cmdList)
      : ComputeContext(owningEngine)
      , m_cmd_list(cmdList)
    {

    }

    ID3D12GraphicsCommandList* DxComputeContext::dx_cmdlist()
    {
      return m_cmd_list.Get();
    }

    void DxComputeContext::platform_reset(CommandAllocator* alloc, const ContextResetData& resetData)
    {
      DxCommandAllocator* dx_alloc = static_cast<DxCommandAllocator*>(alloc);

      REX_ASSERT_X(dx_alloc != nullptr, "The command allocator for a context cannot be null");

      dx_alloc->dx_object()->Reset();
      m_cmd_list->Reset(dx_alloc->dx_object(), d3d::dx12_pso(resetData.pso));

      rsl::array<ID3D12DescriptorHeap*, 2> d3d_desc_heaps{};
      d3d_desc_heaps[0] = d3d::to_dx12(resetData.shader_visible_srv_desc_heap)->dx_object();
      d3d_desc_heaps[1] = d3d::to_dx12(resetData.shader_visible_sampler_desc_heap)->dx_object();
      m_cmd_list->SetDescriptorHeaps(d3d_desc_heaps.size(), d3d_desc_heaps.data());
    }

    // Profiling events
    void DxComputeContext::begin_profile_event(rsl::string_view eventName)
    {
      ::PIXBeginEvent(m_cmd_list.Get(), 0, eventName.data());
    }
    void DxComputeContext::end_profile_event()
    {
      ::PIXEndEvent(m_cmd_list.Get());
    }
  }
}