#pragma once

#include "rex_renderer_core/gfx/graphics.h"
#include "rex_renderer_core/gfx/compute_context.h"
#include "rex_renderer_core/system/graphics_engine.h"

#include "rex_directx/utility/dx_util.h"

namespace rex
{
  namespace gfx
  {
    class DxComputeContext : public ComputeContext
    {
    public:
      DxComputeContext(gfx::GraphicsEngine* owningEngine, const wrl::ComPtr<ID3D12GraphicsCommandList> cmdList);

      ID3D12GraphicsCommandList* dx_cmdlist();

    protected:
      void platform_reset(CommandAllocator* alloc, DescriptorHeap* descHeap) override;
      // profiling events
      void begin_profile_event(rsl::string_view eventName) override;
      void end_profile_event() override;

    private:
      wrl::ComPtr<ID3D12GraphicsCommandList> m_cmd_list;
    };
  }
}