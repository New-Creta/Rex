#include "rex_directx/system/dx_gpu_engine.h"

#include "rex_engine/diagnostics/assert.h"
#include "rex_engine/diagnostics/log.h"

#include "rex_directx/diagnostics/dx_call.h"
#include "rex_directx/utility/dx_util.h"
#include "rex_directx/utility/d3dx12.h"
#include "rex_directx/dxgi/includes.h"
#include "rex_directx/dxgi/factory.h"
#include "rex_directx/dxgi/dx_adapter.h"
#include "rex_directx/dxgi/dx_adapter_manager.h"
#include "rex_directx/system/dx_device.h"
#include "rex_directx/system/dx_feature_level.h"
#include "rex_directx/system/dx_resource_heap.h"
#include "rex_directx/system/dx_view_heap.h"
#include "rex_directx/system/dx_command_queue.h"
#include "rex_directx/resources/dx_texture_2d.h"
#include "rex_directx/resources/dx_sampler_2d.h"

#include "rex_directx/system/dx_render_engine.h"
#include "rex_directx/system/dx_compute_engine.h"
#include "rex_directx/system/dx_copy_engine.h"

#include "rex_directx/imgui/dx_imgui_window.h"

namespace rex
{
  namespace gfx
  {
    DEFINE_LOG_CATEGORY(LogDxGpuEngine);

    DxGpuEngine::DxGpuEngine(const OutputWindowUserData& userData, rsl::unique_ptr<DxDevice> device, dxgi::AdapterManager* adapterManager)
      : GpuEngine(userData)
      , m_device(rsl::move(device))
      , m_adapter_manager(adapterManager)
      , m_heap(rhi::create_resource_heap())
      , m_shader_compiler()
    {
    }

    // Allocate a 1D buffer on the gpu, returning a DirectX resource
    wrl::ComPtr<ID3D12Resource> DxGpuEngine::allocate_buffer(rsl::memory_size size)
    {
      return m_heap->create_buffer(size);
    }
    // Allocate a 2D buffer on the gpu, returning a DirectX resource
    wrl::ComPtr<ID3D12Resource> DxGpuEngine::allocate_texture2d(s32 width, s32 height, TextureFormat format)
    {
      DXGI_FORMAT d3d_format = d3d::to_dx12(format);
      return m_heap->create_texture2d(d3d_format, width, height);
    }

    // Create a render target view for a given resource
    DxResourceView DxGpuEngine::create_rtv(const wrl::ComPtr<ID3D12Resource>& texture)
    {
      return d3d::to_dx12(cpu_desc_heap(ViewHeapType::RenderTarget))->create_rtv(texture.Get());
    }
    // Create a shader resource view pointing to a 2D texture
    DxResourceView DxGpuEngine::create_texture2d_srv(const wrl::ComPtr<ID3D12Resource>& texture)
    {
      return d3d::to_dx12(cpu_desc_heap(ViewHeapType::Texture2D))->create_texture2d_srv(texture.Get());
    }
    // Create a constant buffer view pointing for a given resource
    DxResourceView DxGpuEngine::create_cbv(const wrl::ComPtr<ID3D12Resource>& resource, rsl::memory_size size)
    {
      return d3d::to_dx12(cpu_desc_heap(ViewHeapType::ConstantBuffer))->create_cbv(resource.Get(), size);
    }
    rsl::unique_ptr<DxSampler2D> DxGpuEngine::create_sampler2d(const SamplerDesc& desc)
    {
      return d3d::to_dx12(cpu_desc_heap(ViewHeapType::Sampler))->create_sampler2d(desc);
    }

    // Compile a shader written in HLSL
    wrl::ComPtr<ID3DBlob> DxGpuEngine::compile_shader(const CompileShaderDesc& desc)
    {
      return m_shader_compiler.compile_shader(desc);
    }

    // Initialize the various sub engines
    rsl::unique_ptr<RenderEngine> DxGpuEngine::init_render_engine(ResourceStateTracker* resourceStateTracker)
    {
      return rsl::make_unique<DxRenderEngine>(resourceStateTracker);
    }
    rsl::unique_ptr<CopyEngine> DxGpuEngine::init_copy_engine(ResourceStateTracker* resourceStateTracker)
    {
      return rsl::make_unique<DxCopyEngine>(resourceStateTracker);
    }
    rsl::unique_ptr<ComputeEngine> DxGpuEngine::init_compute_engine(ResourceStateTracker* resourceStateTracker)
    {
      return rsl::make_unique<DxComputeEngine>(resourceStateTracker);
    }

    // Initialize the resource heap which keeps track of all gpu resources
    void DxGpuEngine::init_resource_heap()
    {
      rsl::memory_size resource_heap_size = 100_mib; // hardcoded for now, will become a setting in the future
      CD3DX12_HEAP_DESC desc(resource_heap_size, D3D12_HEAP_TYPE_DEFAULT);

      wrl::ComPtr<ID3D12Heap> d3d_heap;
      if (DX_FAILED(m_device->dx_object()->CreateHeap(&desc, IID_PPV_ARGS(&d3d_heap))))
      {
        REX_ERROR(LogDxGpuEngine, "Failed to create global resource heap");
      }

      m_heap = rsl::make_unique<ResourceHeap>(d3d_heap, m_device->dx_object());
    }

    // Allocate a new view heap of a given type
    rsl::unique_ptr<ViewHeap> DxGpuEngine::allocate_view_heap(ViewHeapType viewHeapType, IsShaderVisible isShaderVisible)
    {
      return rhi::create_view_heap(d3d::to_dx12(viewHeapType), isShaderVisible);
    }
  }
}