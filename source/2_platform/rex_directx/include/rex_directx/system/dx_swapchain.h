#pragma once

#include "rex_directx/utility/dx_util.h"
#include "rex_renderer_core/resource_management/resource.h"
#include "rex_engine/engine/types.h"

#include <dxgi1_4.h>

namespace rex
{
  namespace rhi
  {
    struct OutputWindowUserData;
    struct MsaaSettings;

    class Swapchain
    {
    public:
      Swapchain(const wrl::ComPtr<IDXGISwapChain3>& swapchain, DXGI_FORMAT format, s32 bufferCount);

      HRESULT resize_buffers(s32 width, s32 height, DXGI_SWAP_CHAIN_FLAG flags);

      s32 buffer_count() const;

      void present();

      wrl::ComPtr<ID3D12Resource> get_buffer(s32 idx);

    private:
      wrl::ComPtr<IDXGISwapChain3> m_swapchain;
      DXGI_FORMAT m_format;
      s32 m_buffer_count;
    };
  }
}