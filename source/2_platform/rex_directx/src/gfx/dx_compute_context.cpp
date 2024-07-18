#include "rex_directx/gfx/dx_compute_context.h"

#include "rex_directx/system/dx_command_allocator.h"
#include "rex_directx/system/dx_view_heap.h"

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
      d3d::reset_cmdlist(m_cmd_list.Get(), dx_alloc, resetData);
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