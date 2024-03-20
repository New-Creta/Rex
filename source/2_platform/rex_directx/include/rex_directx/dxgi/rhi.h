#pragma once

#include "rex_directx/dxgi/util.h"
#include "rex_directx/utility/directx_util.h"

#include "rex_std/string_view.h"
#include "rex_engine/engine/defines.h"

namespace rex
{
  namespace renderer
  {
    struct OutputWindowUserData;
  }

  namespace rhi
  {
    // Useful function that assigns a name to a resource.
    // This is useful for debugging
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

    // Initializes the render hardware infrastructure
    // Creates the dxgi factory, d3d device, command buffers, heaps and swapchain
    // After this, the rhi is setup to start creating resources (textures, shaders, vertex buffers, ..)
    bool init(const renderer::OutputWindowUserData& userData);

    void shutdown();
  }
}