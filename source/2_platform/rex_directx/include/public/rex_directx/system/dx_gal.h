#pragma once

#include "rex_engine/gfx/graphics.h"

#include "rex_directx/dxgi/includes.h"
#include "rex_directx/dxgi/factory.h"
#include "rex_directx/utility/dx_util.h"
#include "rex_engine/gfx/core/gpu_description.h"

#include "rex_directx/system/dx_device.h"
#include "rex_directx/system/dx_debug_interface.h"
#include "rex_directx/system/dx_resource_heap.h"
#include "rex_directx/system/dx_view_heap.h"
#include "rex_directx/system/dx_command_queue.h"
#include "rex_directx/system/dx_swapchain.h"

#include "rex_directx/dxgi/factory.h"
#include "rex_directx/dxgi/dx_adapter_manager.h"
#include "rex_directx/dxgi/dx_adapter.h"

#include "rex_directx/resources/dx_upload_buffer.h"

#include "rex_std/string_view.h"
#include "rex_engine/engine/defines.h"
#include "rex_engine/engine/types.h"

#include "rex_directx/utility/dx_util.h"

#include "rex_engine/platform/win/win_com_ptr.h"

#include "rex_directx/system/dx_view_heap.h"
#include "rex_directx/system/dx_shader_compiler.h"

#include "rex_std/bonus/math/color.h"



struct ID3D12GraphicsCommandList;

namespace rex
{
  namespace gfx
  {
		class DxDevice;
		class DxCommandQueue;
		class DxResourceHeap;
		class ViewHeap;
		struct CompileShaderDesc;
		class DxSampler2D;

		namespace dxgi
		{
			class Factory;
			class AdapterManager;
		}

    class IsShaderVisible;

    class DirectXInterface : public GALBase
    {
    public:
      DirectXInterface(const OutputWindowUserData& userData);
      DirectXInterface(const DirectXInterface&) = delete;
      DirectXInterface(DirectXInterface&&) = delete;
      ~DirectXInterface();

      DirectXInterface& operator=(const DirectXInterface&) = delete;
      DirectXInterface& operator=(DirectXInterface&&) = delete;


      // Return basic info about the graphics hardware of the current machine
      const Info& info() const override;

      // Configuration
      ShaderPlatform shader_platform() const override;

      // Backend Systems
      rsl::unique_ptr<CommandQueue>           create_command_queue(GraphicsEngineType type) override;
      rsl::unique_ptr<Swapchain>              create_swapchain(void* apiDevice, s32 bufferCount, void* primaryDisplayHandle) override;
      rsl::unique_ptr<CommandAllocator>       create_command_allocator(GraphicsEngineType type) override;

      // Create a DirectX commandlist
      wrl::ComPtr<ID3D12GraphicsCommandList>  create_commandlist(CommandAllocator* alloc, GraphicsEngineType type);
      // Create a DirectX descriptor heap
      rsl::unique_ptr<ViewHeap>               create_view_heap(D3D12_DESCRIPTOR_HEAP_TYPE type, IsShaderVisible isShaderVisible);
      // Create a DirectX resource heap
      rsl::unique_ptr<DxResourceHeap>           create_resource_heap();
      // Create a DirectX fence object
      rsl::unique_ptr<DxFence>                create_fence();

      // Resource creation
      rsl::unique_ptr<RenderTarget>           create_render_target(s32 width, s32 height, TextureFormat format)                                               override;
      rsl::unique_ptr<VertexBuffer>           create_vertex_buffer(s32 numVertices, s32 vertexSize, const void* data = nullptr)                               override;
      rsl::unique_ptr<IndexBuffer>            create_index_buffer(s32 numIndices, IndexBufferFormat format, const void* data = nullptr)                       override;
      rsl::unique_ptr<RootSignature>          create_root_signature(const rsl::vector<ShaderParameterDeclaration>& parameters)                                override;
      rsl::unique_ptr<PipelineState>          create_pso(const PipelineStateDesc& desc)                                                                       override;
      rsl::unique_ptr<Texture2D>              create_texture2d(s32 width, s32 height, TextureFormat format, const void* data = nullptr)                       override;
      rsl::unique_ptr<DepthStencilBuffer>     create_depth_stencil_buffer(s32 width, s32 height, TextureFormat format, const ClearStateDesc& clearStateDesc)  override;
      rsl::unique_ptr<ConstantBuffer>         create_constant_buffer(rsl::memory_size size, rsl::string_view debugName = "Constant Buffer")                   override;
      rsl::unique_ptr<InputLayout>            create_input_layout(const InputLayoutDesc& desc)                                                                override;
      rsl::unique_ptr<Shader>                 create_vertex_shader(rsl::string_view sourceCode, rsl::string_view shaderName = "")                             override;
      rsl::unique_ptr<Shader>                 create_vertex_shader(const memory::Blob& byteBlob)                                                              override;
      rsl::unique_ptr<Shader>                 create_pixel_shader(rsl::string_view sourceCode, rsl::string_view shaderName = "")                              override;
      rsl::unique_ptr<Shader>                 create_pixel_shader(const memory::Blob& byteBlob)                                                               override;
      rsl::unique_ptr<UploadBuffer>           create_upload_buffer(rsl::memory_size size)                                                                     override;
      rsl::unique_ptr<Material>               create_material(const MaterialDesc& matDesc)                                                                    override;
      rsl::unique_ptr<Sampler2D>              create_sampler2d(const SamplerDesc& desc)                                                                       override;
      rsl::unique_ptr<UnorderedAccessBuffer>  create_unordered_access_buffer(rsl::memory_size size, const void* data = nullptr)                               override;
      rsl::unique_ptr<StructuredBuffer>       create_structured_buffer(rsl::memory_size stride, s32 numElements, const void* data = nullptr)                  override;

      // View creation
      ResourceView* create_srv(const RenderTarget* rt) override;

      // -------------------------
      // Resource creation from Direct X
      // -------------------------

      // Return a new render target constructed from a given gpu resource (usefull for swapchains)
      rsl::unique_ptr<RenderTarget> create_render_target(wrl::ComPtr<ID3D12Resource>& resource, DXGI_FORMAT format);

      // Repoint an existing render target view to a new buffer and return this as a new render target
      rsl::unique_ptr<RenderTarget> retarget_render_target(wrl::ComPtr<ID3D12Resource>& resource, DXGI_FORMAT format, DxResourceView view);

      // Log live gpu objects using DirectX api
      void report_live_objects();


    protected:
      void api_init() override;

      // Initialize the various sub engines
      rsl::unique_ptr<RenderEngine>   init_render_engine(ResourceStateTracker* resourceStateTracker) override;
      rsl::unique_ptr<ComputeEngine>  init_compute_engine(ResourceStateTracker* resourceStateTracker) override;

      // Allocates a debug interface using DirectX debug interface API
      rsl::unique_ptr<DebugInterface> allocate_debug_interface() override;

      // Initialize the resource heap which keeps track of all gpu resources
      void init_resource_heap() override;
      // Allocate a new view heap of a given type
      rsl::unique_ptr<ViewHeap> allocate_view_heap(ResourceViewType viewHeapType, IsShaderVisible isShaderVisible) override;

    private:
      // Allocate a 1D buffer on the gpu, returning a DirectX resource
      wrl::ComPtr<ID3D12Resource> allocate_buffer(rsl::memory_size size);
      // Allocate a 1D buffer on the gpu that allows for unordered access, returning a DirectX resource
      wrl::ComPtr<ID3D12Resource> allocate_unordered_access_buffer(rsl::memory_size size);
      // Allocate a 1D buffer on the gpu for a structured buffer, returning a DirectX resource
      wrl::ComPtr<ID3D12Resource> allocate_structured_buffer(rsl::memory_size size);
      // Allocate a 2D buffer on the gpu, returning a DirectX resource
      wrl::ComPtr<ID3D12Resource> allocate_texture2d(s32 width, s32 height, TextureFormat format);
      // Allocate a 2D buffer on the gpu, returning a DirectX resource
      wrl::ComPtr<ID3D12Resource> allocate_render_target(s32 width, s32 height, TextureFormat format, const ClearStateDesc& clearColor);
      // Allocate a 2D buffer on the gpu, used for depth stencil testing
      wrl::ComPtr<ID3D12Resource> allocate_depth_stencil(s32 width, s32 height, TextureFormat format, const ClearStateDesc& clearStateDesc);

      // Create a render target view for a given resource
      DxResourceView create_rtv(const wrl::ComPtr<ID3D12Resource>& texture, DXGI_FORMAT format);
      // Create a shader resource view pointing to a 2D texture
      DxResourceView create_texture2d_srv(const wrl::ComPtr<ID3D12Resource>& texture);
      // Create a shader resource view pointing to a structured buffer
      DxResourceView create_structured_buffer_srv(const wrl::ComPtr<ID3D12Resource>& resource, rsl::memory_size stride);
      // Create a constant buffer view pointing for a given resource
      DxResourceView create_cbv(const wrl::ComPtr<ID3D12Resource>& resource, rsl::memory_size size);
      // Create a depth stencil view for a given resource
      DxResourceView create_dsv(const wrl::ComPtr<ID3D12Resource>& texture);
      // Create a unordered access view for a given resource
      DxResourceView create_uav(const wrl::ComPtr<ID3D12Resource>& resource, rsl::memory_size size);

      // Create a sampler2D and store it on the gpu
      rsl::unique_ptr<DxSampler2D> allocate_sampler2d(const SamplerDesc& desc);

    private:
      // Resources needed to create objects
      rsl::unique_ptr<DxDevice> m_device = nullptr;             // Used as the factory object to create gpu resources
      DxCommandQueue* m_render_command_queue = nullptr;         // Used as the object the swapchain speaks to queue a present command
      rsl::unique_ptr<dxgi::Factory> m_factory;
      rsl::unique_ptr<dxgi::AdapterManager> m_adapter_manager;  // The manager holding all the adapters on this machine

      rsl::unique_ptr<DxResourceHeap> m_heap;  // The heap we use to allocate gpu resources
      rsl::Color4f m_rtv_clear_color;
    };
  }
}