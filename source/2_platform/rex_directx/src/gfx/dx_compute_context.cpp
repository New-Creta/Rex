#include "rex_directx/gfx/dx_compute_context.h"

#include "rex_directx/system/dx_command_allocator.h"
#include "rex_directx/system/dx_descriptor_heap.h"

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

    void DxComputeContext::platform_reset(CommandAllocator* alloc, DescriptorHeap* descHeap)
    {
      DxCommandAllocator* dx_alloc = static_cast<DxCommandAllocator*>(alloc);

      REX_ASSERT_X(dx_alloc != nullptr, "The command allocator for a context cannot be null");

      dx_alloc->get()->Reset();
      m_cmd_list->Reset(dx_alloc->get(), nullptr);
      ID3D12DescriptorHeap* d3d_desc_heap = d3d::to_dx12(descHeap)->get();
      m_cmd_list->SetDescriptorHeaps(1, &d3d_desc_heap);
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