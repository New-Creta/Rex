#pragma once

#include "rex_renderer_core/system/gpu_engine.h"

#include "rex_directx/utility/dx_util.h"

#include "rex_engine/platform/win/win_com_ptr.h"

#include "rex_directx/system/dx_view_heap.h"
#include "rex_directx/system/dx_shader_compiler.h"

struct IDXGIInfoQueue;

namespace rex
{
  namespace gfx 
  {
    class DxDevice;
    class DxCommandQueue;
    class ResourceHeap;
    class ViewHeap;
    struct CompileShaderDesc;
    class DxSampler2D;
    
    namespace dxgi
    {
      class Factory;
      class AdapterManager;
    }
  }

  namespace gfx
  {
    // A gpu engine for a system running on DirectX 12
    class DxGpuEngine : public GpuEngine
    {
    public:
      DxGpuEngine(const OutputWindowUserData& userData, rsl::unique_ptr<DxDevice> device, dxgi::AdapterManager* adapterManager);

      // Allocate a 1D buffer on the gpu, returning a DirectX resource
      wrl::ComPtr<ID3D12Resource> allocate_buffer(rsl::memory_size size);
      // Allocate a 2D buffer on the gpu, returning a DirectX resource
      wrl::ComPtr<ID3D12Resource> allocate_texture2d(s32 width, s32 height, TextureFormat format);
      wrl::ComPtr<ID3D12Resource> allocate_depth_stencil(s32 width, s32 height, TextureFormat format);

      // Create a render target view for a given resource
      DxResourceView create_rtv(const wrl::ComPtr<ID3D12Resource>& texture);
      // Create a shader resource view pointing to a 2D texture
      DxResourceView create_texture2d_srv(const wrl::ComPtr<ID3D12Resource>& texture);
      // Create a constant buffer view pointing for a given resource
      DxResourceView create_cbv(const wrl::ComPtr<ID3D12Resource>& resource, rsl::memory_size size);
      // Create a render target view for a given resource
      DxResourceView create_dsv(const wrl::ComPtr<ID3D12Resource>& texture);
      // Create a sampler2D and store it on the gpu
      rsl::unique_ptr<DxSampler2D> create_sampler2d(const SamplerDesc& desc);

      // Compile a shader written in HLSL
      wrl::ComPtr<ID3DBlob> compile_shader(const CompileShaderDesc& desc);

    protected:
      // Initialize the various sub engines
      rsl::unique_ptr<RenderEngine> init_render_engine(ResourceStateTracker* resourceStateTracker) override;
      rsl::unique_ptr<CopyEngine> init_copy_engine(ResourceStateTracker* resourceStateTracker) override;
      rsl::unique_ptr<ComputeEngine> init_compute_engine(ResourceStateTracker* resourceStateTracker) override;

      // Initialize the resource heap which keeps track of all gpu resources
      void init_resource_heap() override;
      // Allocate a new view heap of a given type
      rsl::unique_ptr<ViewHeap> allocate_view_heap(ViewHeapType viewHeapType, IsShaderVisible isShaderVisible) override;

    private:
      rsl::unique_ptr<DxDevice> m_device;    // The DirectX device
      dxgi::AdapterManager* m_adapter_manager;    // The list of adapters (aka gpus)
      rsl::unique_ptr<ResourceHeap> m_heap;  // The heap we use to allocate gpu resources
      ShaderCompiler m_shader_compiler;      // A shader compiler with internal caching
    };
  }
}