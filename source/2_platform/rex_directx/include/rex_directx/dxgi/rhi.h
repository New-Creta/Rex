#pragma once

#include "rex_directx/dxgi/util.h"

namespace rex
{
  namespace renderer
  {
    struct OutputWindowUserData;
  }

  namespace rhi
  {
    bool init(const renderer::OutputWindowUserData& userData);

    rex::wrl::ComPtr<IDXGISwapChain> create_swapchain(ID3D12CommandList* commandlist, DXGI_SWAP_CHAIN_DESC& desc);

    template<typename TResourceType>
    void set_debug_name_for(TResourceType* resource, rsl::string_view name)
    {
#ifdef REX_ENABLE_DEBUG_RESOURCE_NAMES
      resource->SetPrivateData(WKPDID_D3DDebugObjectName, name.length(), name.data());
#else
      UNUSED_PARAM(resource);
      UNUSED_PARAM(name);
#endif
    }
  }
}