#include "rex_directx/gfx/dx_gpu_engine.h"

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
#include "rex_directx/system/dx_descriptor_heap.h"
#include "rex_directx/system/dx_command_queue.h"
#include "rex_directx/resources/dx_texture_2d.h"

#include "rex_directx/gfx/dx_render_engine.h"
#include "rex_directx/gfx/dx_compute_engine.h"
#include "rex_directx/gfx/dx_copy_engine.h"


#include "rex_directx/rendering/dx_imgui_window.h"

namespace rex
{
  namespace gfx
  {
    DEFINE_LOG_CATEGORY(LogDxGpuEngine);

    DxGpuEngine::DxGpuEngine(const renderer::OutputWindowUserData& userData, rsl::unique_ptr<rhi::DxDevice> device, dxgi::AdapterManager* adapterManager)
      : GpuEngine(rsl::make_unique<rhi::DxRenderEngine>(), rsl::make_unique<rhi::DxComputeEngine>(), rsl::make_unique<rhi::DxCopyEngine>(), userData)
      , m_device(rsl::move(device))
      , m_heap(rhi::create_resource_heap())
      , m_descriptor_heap_pool()
      , m_adapter_manager(adapterManager)
    {
    }

    wrl::ComPtr<ID3D12Resource> DxGpuEngine::allocate_buffer(rsl::memory_size size)
    {
      return m_heap->create_buffer(size);
    }
    wrl::ComPtr<ID3D12Resource> DxGpuEngine::allocate_texture2d(s32 width, s32 height, renderer::TextureFormat format)
    {
      DXGI_FORMAT d3d_format = d3d::to_dx12(format);
      return m_heap->create_texture2d(d3d_format, width, height);
    }

    rhi::DescriptorHandle DxGpuEngine::create_rtv(const wrl::ComPtr<ID3D12Resource>& texture)
    {
      return m_descriptor_heap_pool.at(D3D12_DESCRIPTOR_HEAP_TYPE_RTV)->create_rtv(texture.Get());
    }
    rhi::DescriptorHandle DxGpuEngine::create_texture2d_srv(const wrl::ComPtr<ID3D12Resource>& texture)
    {
      return m_descriptor_heap_pool.at(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV)->create_texture2d_srv(texture.Get());
    }
    rhi::DescriptorHandle DxGpuEngine::create_cbv(const wrl::ComPtr<ID3D12Resource>& resource, rsl::memory_size size)
    {
      return m_descriptor_heap_pool.at(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV)->create_cbv(resource.Get(), size);
    }
    wrl::ComPtr<ID3DBlob> DxGpuEngine::compile_shader(const rhi::CompileShaderDesc& desc)
    {
      return m_shader_compiler.compile_shader(desc);
    }

    rsl::vector<ID3D12DescriptorHeap*> DxGpuEngine::desc_heaps()
    {
      rsl::vector<ID3D12DescriptorHeap*> heaps;
      heaps.emplace_back(m_descriptor_heap_pool.at(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV)->get());

      return heaps;
    }

    void DxGpuEngine::init_resource_heap()
    {
      rsl::memory_size resource_heap_size = 100_mib;
      CD3DX12_HEAP_DESC desc(resource_heap_size, D3D12_HEAP_TYPE_DEFAULT);

      wrl::ComPtr<ID3D12Heap> d3d_heap;
      if (DX_FAILED(m_device->get()->CreateHeap(&desc, IID_PPV_ARGS(&d3d_heap))))
      {
        REX_ERROR(LogDxGpuEngine, "Failed to create global resource heap");
      }

      m_heap = rsl::make_unique<rhi::ResourceHeap>(d3d_heap, m_device->get());
    }
    void DxGpuEngine::init_descriptor_heaps()
    {
      init_desc_heap(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
      init_desc_heap(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
      init_desc_heap(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
      init_desc_heap(D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER);
    }

    void DxGpuEngine::init_desc_heap(D3D12_DESCRIPTOR_HEAP_TYPE type)
    {
      m_descriptor_heap_pool.emplace(type, rhi::create_descriptor_heap(type));
    }
  }
}