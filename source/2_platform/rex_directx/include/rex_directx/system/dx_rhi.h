#pragma once

#include "rex_renderer_core/rhi/rhi.h"

#include "rex_directx/dxgi/includes.h"
#include "rex_directx/dxgi/factory.h"
#include "rex_directx/utility/dx_util.h"
#include "rex_renderer_core/resource_management/resource_slot.h"
#include "rex_renderer_core/system/gpu_description.h"
#include "rex_renderer_core/resource_management/resource_pool.h"

#include "rex_directx/system/dx_device.h"
#include "rex_directx/system/dx_debug_interface.h"
#include "rex_directx/system/dx_resource_heap.h"
#include "rex_directx/system/dx_descriptor_heap.h"

#include "rex_directx/system/dx_command_queue.h"
#include "rex_directx/system/dx_swapchain.h"

#include "rex_directx/resources/dx_upload_buffer.h"

#include "rex_std/string_view.h"
#include "rex_engine/engine/defines.h"
#include "rex_engine/engine/types.h"

struct ID3D12GraphicsCommandList;

namespace rex
{
  namespace rhi
  {
    REX_STATIC_WARNING("The following func doesn't belong here and is more a utility/debug functionality");
    // Useful function that assigns a name to a resource.
    // This is useful for debugging
    template<typename TResourceType>
    void set_debug_name_for(TResourceType* resource, rsl::string_view name)
    {
#ifdef REX_ENABLE_DEBUG_RESOURCE_NAMES
      resource->SetPrivateData(WKPDID_D3DDebugObjectName, name.length(), name.data());
#else
      REX_UNUSED_PARAM(resource);
      REX_UNUSED_PARAM(name);
#endif
    }

    namespace api
    {
      rsl::unique_ptr<RenderTarget> create_render_target(wrl::ComPtr<ID3D12Resource>& resource);

      wrl::ComPtr<ID3DBlob> compile_shader(const CompileShaderDesc& desc);
      wrl::ComPtr<ID3D12GraphicsCommandList> create_commandlist(rhi::CommandAllocator* alloc, rhi::GraphicsEngineType type);
      rsl::unique_ptr<DescriptorHeap> create_descriptor_heap(D3D12_DESCRIPTOR_HEAP_TYPE type);
      rsl::unique_ptr<Texture2D> create_texture2d(const wrl::ComPtr<ID3D12Resource>& resource);
      rsl::unique_ptr<ResourceHeap> create_resource_heap();
      rsl::vector<ID3D12DescriptorHeap*> get_desc_heaps();
      rsl::unique_ptr<UploadBuffer> create_upload_buffer(rsl::memory_size size);

      rsl::unique_ptr<RenderTarget> create_render_target(wrl::ComPtr<ID3D12Resource>& resource);

    }
  }
}