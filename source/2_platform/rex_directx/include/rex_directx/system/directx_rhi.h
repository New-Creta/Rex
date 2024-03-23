#pragma once

#include "rex_directx/dxgi/util.h"
#include "rex_directx/dxgi/factory.h"
#include "rex_directx/utility/directx_util.h"
#include "rex_renderer_core/resource_management/resource_slot.h"
#include "rex_renderer_core/system/gpu_description.h"
#include "rex_renderer_core/system/rhi.h"
#include "rex_renderer_core/resource_management/resource_pool.h"

#include "rex_directx/system/directx_device.h"
#include "rex_directx/system/directx_debug_interface.h"
#include "rex_directx/system/directx_resource_heap.h"
#include "rex_directx/system/directx_descriptor_heap.h"
#include "rex_directx/system/directx_commandlist.h"
#include "rex_directx/system/directx_pipeline_library.h"
#include "rex_directx/resources/upload_buffer.h"

#include "rex_std/string_view.h"
#include "rex_engine/engine/defines.h"
#include "rex_engine/engine/types.h"

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

    namespace d3d
    {
      wrl::ComPtr<ID3D12RootSignature> create_shader_root_signature(const rsl::vector<ConstantLayoutDescription>& constants);
    }
  }
}