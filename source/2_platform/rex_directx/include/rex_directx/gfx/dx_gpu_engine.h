#pragma once

#include "rex_renderer_core/gfx/gpu_engine.h"

#include "rex_directx/utility/dx_util.h"

#include "rex_engine/platform/win/win_com_ptr.h"

#include "rex_directx/system/dx_descriptor_heap.h"

struct IDXGIInfoQueue;

namespace rex
{
  namespace rhi 
  {
    class DxDevice;
    class DxCommandQueue;
    class ResourceHeap;
    class DescriptorHeap;
  }
  namespace dxgi
  {
    class Factory;
    class AdapterManager;
  }

  namespace gfx
  {
    class DxGpuEngine : public GpuEngine
    {
    public:
      DxGpuEngine(const renderer::OutputWindowUserData& userData, rsl::unique_ptr<rhi::DxDevice> device, rsl::unique_ptr<dxgi::AdapterManager> adapterManager);
      ~DxGpuEngine() override = default;

      wrl::ComPtr<ID3D12Resource> allocate_buffer(rsl::memory_size size);
      wrl::ComPtr<ID3D12Resource> allocate_texture2d(renderer::TextureFormat, s32 width, s32 height);

      rhi::DescriptorHandle create_rtv(const wrl::ComPtr<ID3D12Resource>& texture);
      rhi::DescriptorHandle create_texture2d_srv(const wrl::ComPtr<ID3D12Resource>& texture);
      rhi::DescriptorHandle create_cbv(const wrl::ComPtr<ID3D12Resource>& resource, rsl::memory_size size);

      rsl::vector<ID3D12DescriptorHeap*> desc_heaps();

    private:
      void init_debug_layer();     
      void init_resource_heap();
      void init_desc_heap_pool();
      void init_desc_heap(D3D12_DESCRIPTOR_HEAP_TYPE type);

    private:
      wrl::ComPtr<IDXGIInfoQueue> m_debug_info_queue;
      rsl::unique_ptr<rhi::DxDevice> m_device;
      rsl::unique_ptr<dxgi::AdapterManager> m_adapter_manager;
      rsl::unique_ptr<rhi::ResourceHeap> m_heap;
      rsl::unordered_map<D3D12_DESCRIPTOR_HEAP_TYPE, rsl::unique_ptr<rhi::DescriptorHeap>> m_descriptor_heap_pool;
    };
  }
}