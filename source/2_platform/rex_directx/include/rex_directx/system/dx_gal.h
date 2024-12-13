#pragma once

#include "rex_engine/gfx/system/gal.h"

#include "rex_directx/dxgi/includes.h"
#include "rex_directx/dxgi/factory.h"
#include "rex_directx/utility/dx_util.h"
#include "rex_engine/gfx/core/gpu_description.h"

#include "rex_directx/system/dx_device.h"
#include "rex_directx/system/dx_debug_interface.h"
#include "rex_directx/system/dx_resource_heap.h"
#include "rex_directx/system/dx_view_heap.h"
#include "rex_directx/system/dx_gpu_engine.h"

#include "rex_directx/system/dx_command_queue.h"
#include "rex_directx/system/dx_swapchain.h"

#include "rex_directx/dxgi/factory.h"
#include "rex_directx/dxgi/dx_adapter_manager.h"
#include "rex_directx/dxgi/dx_adapter.h"

#include "rex_directx/resources/dx_upload_buffer.h"

#include "rex_std/string_view.h"
#include "rex_engine/engine/defines.h"
#include "rex_engine/engine/types.h"

struct ID3D12GraphicsCommandList;

namespace rex
{
  namespace gfx
  {
    class IsShaderVisible;

    class DirectXInterface : public GALInterface
    {
    public:
      DirectXInterface();

      // Initialize the graphics systesm and create a gpu engine
      gfx::GpuEngine* create_gpu_engine(const OutputWindowUserData& userData) override;

      // Return basic info about the graphics hardware of the current machine
      const Info& info() const override;

      // Configuration
      ShaderPlatform shader_platform() const override;

      // Backend Systems
      rsl::unique_ptr<CommandQueue> create_command_queue(GraphicsEngineType type) override;
      rsl::unique_ptr<Swapchain> create_swapchain(void* apiDevice, s32 bufferCount, void* primaryDisplayHandle) override;
      rsl::unique_ptr<CommandAllocator> create_command_allocator(GraphicsEngineType type) override;

      // Resource creation
      rsl::unique_ptr<RenderTarget> create_render_target(s32 width, s32 height, TextureFormat format) override;
      rsl::unique_ptr<VertexBuffer> create_vertex_buffer(s32 numVertices, s32 vertexSize, const void* data = nullptr) override;
      rsl::unique_ptr<IndexBuffer> create_index_buffer(s32 numIndices, IndexBufferFormat format, const void* data = nullptr) override;
      rsl::unique_ptr<RootSignature> create_root_signature(const rsl::vector<ShaderParameterDeclaration>& parameters) override;
      rsl::unique_ptr<PipelineState> create_pso(const PipelineStateDesc& desc) override;
      rsl::unique_ptr<Texture2D> create_texture2d(s32 width, s32 height, TextureFormat format, const void* data = nullptr) override;
      rsl::unique_ptr<DepthStencilBuffer> create_depth_stencil_buffer(s32 width, s32 height, TextureFormat format, const ClearStateDesc& clearStateDesc) override;
      rsl::unique_ptr<ConstantBuffer> create_constant_buffer(rsl::memory_size size, rsl::string_view debugName = "Constant Buffer") override;
      rsl::unique_ptr<InputLayout> create_input_layout(const InputLayoutDesc& desc) override;
      rsl::unique_ptr<Shader> create_vertex_shader(rsl::string_view sourceCode, rsl::string_view shaderName = "") override;
      rsl::unique_ptr<Shader> create_vertex_shader(const memory::Blob& byteBlob) override;
      rsl::unique_ptr<Shader> create_pixel_shader(rsl::string_view sourceCode, rsl::string_view shaderName = "") override;
      rsl::unique_ptr<Shader> create_pixel_shader(const memory::Blob& byteBlob) override;
      rsl::unique_ptr<UploadBuffer> create_upload_buffer(rsl::memory_size size) override;
      rsl::unique_ptr<Material> create_material(const MaterialDesc& matDesc) override;
      rsl::unique_ptr<Sampler2D> create_sampler2d(const SamplerDesc& desc) override;
      rsl::unique_ptr<UnorderedAccessBuffer> create_unordered_access_buffer(rsl::memory_size size, const void* data = nullptr) override;

      ShaderSignature reflect_shader(const gfx::Shader* shader) override;

      // Return a new render target constructed from a given gpu resource (usefull for swapchains)
      rsl::unique_ptr<RenderTarget> create_render_target(wrl::ComPtr<ID3D12Resource>& resource);

      // Compile a shader and return its binary blob
      wrl::ComPtr<ID3DBlob> compile_shader(const CompileShaderDesc& desc);
      // Create a DirectX commandlist
      wrl::ComPtr<ID3D12GraphicsCommandList> create_commandlist(CommandAllocator* alloc, GraphicsEngineType type);
      // Create a DirectX descriptor heap
      rsl::unique_ptr<ViewHeap> create_view_heap(D3D12_DESCRIPTOR_HEAP_TYPE type, IsShaderVisible isShaderVisible);
      // Create a DirectX resource heap
      rsl::unique_ptr<ResourceHeap> create_resource_heap();
      // Create a DirectX fence object
      rsl::unique_ptr<DxFence>          create_fence();

      // Log live gpu objects using DirectX api
      void report_live_objects();

      RenderTarget* current_backbuffer_rt();

    private:
      // Resources needed to create objects
			rsl::unique_ptr<DebugInterface> m_debug_interface;        // Used to determine if we have any leaking resource on shutdown
			rsl::unique_ptr<DxDevice> m_device = nullptr;             // Used as the factory object to create gpu resources
			DxCommandQueue* m_render_command_queue = nullptr;         // Used as the object the swapchain speaks to queue a present command
			rsl::unique_ptr<dxgi::Factory> m_factory;
			rsl::unique_ptr<dxgi::AdapterManager> m_adapter_manager;  // The manager holding all the adapters on this machine

      // The gpu engine which owns all graphic resources
      rsl::unique_ptr<gfx::DxGpuEngine> m_gpu_engine;
    };

    DirectXInterface* dx_gal();
  }
}