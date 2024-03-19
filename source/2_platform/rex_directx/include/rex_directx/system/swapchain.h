#pragma once

#include "rex_directx/utility/directx_util.h"
#include "rex_renderer_core/resource_management/resource.h"
#include "rex_engine/engine/types.h"

namespace rex
{
  namespace renderer
  {
    struct OutputWindowUserData;
    struct MsaaSettings;

    class Swapchain : public BaseResource<IDXGISwapChain>
    {
    public:
      RESOURCE_CLASS_TYPE(Swapchain);

      Swapchain(wrl::ComPtr<IDXGISwapChain> swapchain);

    private:
      wrl::ComPtr<IDXGISwapChain> m_swapchain;
    };
  }
}