#pragma once

#include "rex_directx/system/dx_descriptor_heap.h"
#include "rex_directx/system/dx_resource.h"
#include "rex_directx/system/dx_resource_heap.h"
#include "rex_directx/utility/dx_util.h"
#include "rex_engine/engine/types.h"
#include "rex_renderer_core/resource_management/resource.h"

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

      void resize_buffers(s32 width, s32 height, DXGI_SWAP_CHAIN_FLAG flags);

      s32 buffer_count() const;

      void present();

      Resource2* get_buffer(s32 idx);
      IDXGISwapChain3* get();

    private:
      wrl::ComPtr<IDXGISwapChain3> m_swapchain;
      DXGI_FORMAT m_format;
      rsl::vector<Resource2> m_swapchain_buffers; // swapchain render target buffer indices
    };
  } // namespace rhi
} // namespace rex