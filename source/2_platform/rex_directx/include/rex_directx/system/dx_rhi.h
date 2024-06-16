#pragma once

#include "rex_renderer_core/gfx/rhi.h"

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
  namespace gfx
  {
    namespace rhi
    {
      // All functions here are DirectX specific

      // Return a new render target constructed from a given gpu resource (usefull for swapchains)
      rsl::unique_ptr<RenderTarget> create_render_target(wrl::ComPtr<ID3D12Resource>& resource);

      // Compile a shader and return its binary blob
      wrl::ComPtr<ID3DBlob> compile_shader(const CompileShaderDesc& desc);
      // Create a DirectX commandlist
      wrl::ComPtr<ID3D12GraphicsCommandList> create_commandlist(CommandAllocator* alloc, GraphicsEngineType type);
      // Create a DirectX descriptor heap
      rsl::unique_ptr<DescriptorHeap> create_descriptor_heap(D3D12_DESCRIPTOR_HEAP_TYPE type);
      // Create a DirectX resource heap
      rsl::unique_ptr<ResourceHeap> create_resource_heap();

      // Log live gpu objects using DirectX api
      void report_live_objects();
    }
  }
}