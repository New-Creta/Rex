#pragma once

#include "rex_renderer_core/gfx/gpu_engine.h"

#include "rex_directx/utility/dx_util.h"

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

      wrl::ComPtr<ID3D12Resource> allocate_buffer(rsl::memory_size size);

    private:
      void init_debug_layer();     
      void init_resource_heap();

    private:
      wrl::ComPtr<IDXGIInfoQueue> m_debug_info_queue;
      rsl::unique_ptr<rhi::DxDevice> m_device;
      rsl::unique_ptr<dxgi::AdapterManager> m_adapter_manager;
      rsl::unique_ptr<rhi::ResourceHeap> m_heap;
      rsl::unordered_map<D3D12_DESCRIPTOR_HEAP_TYPE, rhi::DescriptorHeap> m_descriptor_heap_pool;
    };
  }
}