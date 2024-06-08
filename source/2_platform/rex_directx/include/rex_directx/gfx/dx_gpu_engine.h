#pragma once

#include "rex_renderer_core/gfx/gpu_engine.h"

#include "rex_directx/utility/dx_util.h"

#include "rex_engine/platform/win/win_com_ptr.h"

#include "rex_directx/system/dx_descriptor_heap.h"
#include "rex_directx/system/dx_shader_compiler.h"

struct IDXGIInfoQueue;

namespace rex
{
  namespace rhi 
  {
    class DxDevice;
    class DxCommandQueue;
    class ResourceHeap;
    class DescriptorHeap;
    struct CompileShaderDesc;
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
      DxGpuEngine(const renderer::OutputWindowUserData& userData, rsl::unique_ptr<rhi::DxDevice> device, dxgi::AdapterManager* adapterManager);

      wrl::ComPtr<ID3D12Resource> allocate_buffer(rsl::memory_size size);
      wrl::ComPtr<ID3D12Resource> allocate_texture2d(s32 width, s32 height, renderer::TextureFormat format);

      rhi::DescriptorHandle create_rtv(const wrl::ComPtr<ID3D12Resource>& texture);
      rhi::DescriptorHandle create_texture2d_srv(const wrl::ComPtr<ID3D12Resource>& texture);
      rhi::DescriptorHandle create_cbv(const wrl::ComPtr<ID3D12Resource>& resource, rsl::memory_size size);

      wrl::ComPtr<ID3DBlob> compile_shader(const rhi::CompileShaderDesc& desc);

      rsl::vector<ID3D12DescriptorHeap*> desc_heaps();

    protected:
      // Initialize the resource heap which keeps track of all gpu resources
      void init_resource_heap() override;
      // Initialize the descriptor heaps which keep track of all descriptors to various resources
      void init_descriptor_heaps() override;

    private:
      void init_desc_heap(D3D12_DESCRIPTOR_HEAP_TYPE type);

    private:
      wrl::ComPtr<IDXGIInfoQueue> m_debug_info_queue;
      rsl::unique_ptr<rhi::DxDevice> m_device;
      dxgi::AdapterManager* m_adapter_manager;
      rsl::unique_ptr<rhi::ResourceHeap> m_heap;
      rsl::unordered_map<D3D12_DESCRIPTOR_HEAP_TYPE, rsl::unique_ptr<rhi::DescriptorHeap>> m_descriptor_heap_pool;
      rhi::ShaderCompiler m_shader_compiler;
    };
  }
}