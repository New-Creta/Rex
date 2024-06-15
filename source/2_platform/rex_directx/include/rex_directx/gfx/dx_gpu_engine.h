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
    // A gpu engine for a system running on DirectX 12
    class DxGpuEngine : public GpuEngine
    {
    public:
      DxGpuEngine(const renderer::OutputWindowUserData& userData, rsl::unique_ptr<rhi::DxDevice> device, dxgi::AdapterManager* adapterManager);

      // Allocate a 1D buffer on the gpu, returning a DirectX resource
      wrl::ComPtr<ID3D12Resource> allocate_buffer(rsl::memory_size size);
      // Allocate a 2D buffer on the gpu, returning a DirectX resource
      wrl::ComPtr<ID3D12Resource> allocate_texture2d(s32 width, s32 height, renderer::TextureFormat format);

      // Create a render target view for a given resource
      rhi::DescriptorHandle create_rtv(const wrl::ComPtr<ID3D12Resource>& texture);
      // Create a shader resource view pointing to a 2D texture
      rhi::DescriptorHandle create_texture2d_srv(const wrl::ComPtr<ID3D12Resource>& texture);
      // Create a constant buffer view pointing for a given resource
      rhi::DescriptorHandle create_cbv(const wrl::ComPtr<ID3D12Resource>& resource, rsl::memory_size size);

      // Compile a shader written in HLSL
      wrl::ComPtr<ID3DBlob> compile_shader(const rhi::CompileShaderDesc& desc);

    protected:
      // Initialize the various sub engines
      rsl::unique_ptr<RenderEngine> init_render_engine(rhi::ResourceStateTracker* resourceStateTracker) override;
      rsl::unique_ptr<CopyEngine> init_copy_engine(rhi::ResourceStateTracker* resourceStateTracker) override;
      rsl::unique_ptr<ComputeEngine> init_compute_engine(rhi::ResourceStateTracker* resourceStateTracker) override;

      // Initialize the resource heap which keeps track of all gpu resources
      void init_resource_heap() override;
      // Allocate a new descriptor heap of a given type
      rsl::unique_ptr<rhi::DescriptorHeap> allocate_desc_heap(rhi::DescriptorHeapType descHeapType) override;

    private:
      rsl::unique_ptr<rhi::DxDevice> m_device;    // The DirectX device
      dxgi::AdapterManager* m_adapter_manager;    // The list of adapters (aka gpus)
      rsl::unique_ptr<rhi::ResourceHeap> m_heap;  // The heap we use to allocate gpu resources
      rhi::ShaderCompiler m_shader_compiler;      // A shader compiler with internal caching
    };
  }
}