#pragma once

#include "rex_directx/system/dx_descriptor_heap.h"

#include "rex_directx/system/dx_resource_heap.h"
#include "rex_directx/utility/dx_util.h"
#include "rex_engine/engine/types.h"


#include "rex_renderer_core/system/swapchain.h"

#include <dxgi1_4.h>

namespace rex
{
  namespace gfx
  {
    struct OutputWindowUserData;
    struct MsaaSettings;

    class DxSwapchain : public Swapchain
    {
    public:
      DxSwapchain(const wrl::ComPtr<IDXGISwapChain3>& swapchain, s32 width, s32 height, DXGI_FORMAT format, s32 bufferCount);

      // Resize the internal buffers of the swapchain to a new width and height
      void resize_buffers(s32 width, s32 height, DXGI_SWAP_CHAIN_FLAG flags);
      // Swap the current back buffer with the front buffer
      void present();

      // Return the internal wrapped swapchain object
      IDXGISwapChain3* get();

      // Return the index of the current back buffer
      s32 current_buffer_idx() const override;

    private:
      // Query the buffers from the swapped swapchain object and cache them
      void query_buffers(s32 bufferCount);

    private:
      wrl::ComPtr<IDXGISwapChain3> m_swapchain;
      DXGI_FORMAT m_format;
    };
  } // namespace gfx
} // namespace rex