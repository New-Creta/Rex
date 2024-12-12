#include "rex_engine/gfx/system/rhi.h"

namespace rex
{
	namespace gfx
	{
    rsl::unique_ptr<GALInterface> g_gal;

    GALInterface* gal()
    {
      return g_gal.get();
    }

    // Initializes the render hardware infrastructure
		// For DirectX, creates the dxgi factory, d3d device, command buffers, heaps and swapchain
		// After this, the rhi is setup to start creating resources (textures, shaders, vertex buffers, ..)
		gfx::GpuEngine* init_gal(rsl::unique_ptr<GALInterface> galInterface, const OutputWindowUserData& userData)
		{
			REX_ASSERT_X(g_gal == nullptr, "The graphics abstraction layer has already been assigned, this is not allowed");
			g_gal = rsl::move(galInterface);
			return g_gal->create_gpu_engine(userData);
		}

		// shutdown the internal rhi, all reference to the rhi are invalidated from here on out
		void shutdown_gal()
		{
			g_gal.reset();
		}

		//namespace gal
		//{


  //    //// Return basic info about the graphics hardware of the current machine
  //    //const Info& info()
  //    //{
  //    //  return g_gal->info();
  //    //}

  //    //// Configuration
  //    //ShaderPlatform shader_platform()
  //    //{
  //    //  return g_gal->shader_platform();
  //    //}

  //    //// Backend Systems
  //    //rsl::unique_ptr<CommandQueue> create_command_queue(GraphicsEngineType type)
  //    //{
  //    //  return g_gal->create_command_queue(type);
  //    //}
  //    //rsl::unique_ptr<Swapchain> create_swapchain(void* apiDevice, s32 bufferCount, void* primaryDisplayHandle)
  //    //{
  //    //  return g_gal->create_swapchain(apiDevice, bufferCount, primaryDisplayHandle);
  //    //}
  //    //rsl::unique_ptr<CommandAllocator> create_command_allocator(GraphicsEngineType type)
  //    //{
  //    //  return g_gal->create_command_allocator(type);
  //    //}

  //    //// Resource creation
  //    //rsl::unique_ptr<RenderTarget> create_render_target(s32 width, s32 height, TextureFormat format)
  //    //{
  //    //  return g_gal->create_render_target(width, height, format);
  //    //}
  //    //rsl::unique_ptr<VertexBuffer> create_vertex_buffer(s32 numVertices, s32 vertexSize, const void* data)
  //    //{
  //    //  return g_gal->create_vertex_buffer(numVertices, vertexSize, data);
  //    //}
  //    //rsl::unique_ptr<IndexBuffer> create_index_buffer(s32 numIndices, IndexBufferFormat format, const void* data)
  //    //{
  //    //  return g_gal->create_index_buffer(numIndices, format, data);
  //    //}
  //    //rsl::unique_ptr<RootSignature> create_root_signature(const rsl::vector<ShaderParameterDeclaration>& parameters)
  //    //{
  //    //  return g_gal->create_root_signature(parameters);
  //    //}
  //    //rsl::unique_ptr<PipelineState> create_pso(const PipelineStateDesc& desc)
  //    //{
  //    //  return g_gal->create_pso(desc);
  //    //}
  //    //rsl::unique_ptr<Texture2D> create_texture2d(s32 width, s32 height, TextureFormat format, const void* data)
  //    //{
  //    //  return g_gal->create_texture2d(width, height, format, data);
  //    //}
  //    //rsl::unique_ptr<DepthStencilBuffer> create_depth_stencil_buffer(s32 width, s32 height, TextureFormat format, const ClearStateDesc& clearStateDesc)
  //    //{
  //    //  return g_gal->create_depth_stencil_buffer(width, height, format, clearStateDesc);
  //    //}
  //    //rsl::unique_ptr<ConstantBuffer> create_constant_buffer(rsl::memory_size size, rsl::string_view debugName)
  //    //{
  //    //  return g_gal->create_constant_buffer(size, debugName);
  //    //}
  //    //rsl::unique_ptr<InputLayout> create_input_layout(const InputLayoutDesc& desc)
  //    //{
  //    //  return g_gal->create_input_layout(desc);
  //    //}
  //    //rsl::unique_ptr<Shader> create_vertex_shader(rsl::string_view sourceCode, rsl::string_view shaderName)
  //    //{
  //    //  return g_gal->create_vertex_shader(sourceCode, shaderName);
  //    //}
  //    //rsl::unique_ptr<Shader> create_vertex_shader(const memory::Blob& byteBlob)
  //    //{
  //    //  return g_gal->create_vertex_shader(byteBlob);
  //    //}
  //    //rsl::unique_ptr<Shader> create_pixel_shader(rsl::string_view sourceCode, rsl::string_view shaderName)
  //    //{
  //    //  return g_gal->create_pixel_shader(sourceCode, shaderName);
  //    //}
  //    //rsl::unique_ptr<Shader> create_pixel_shader(const memory::Blob& byteBlob)
  //    //{
  //    //  return g_gal->create_pixel_shader(byteBlob);
  //    //}
  //    //rsl::unique_ptr<UploadBuffer> create_upload_buffer(rsl::memory_size size)
  //    //{
  //    //  return g_gal->create_upload_buffer(size);
  //    //}
  //    //rsl::unique_ptr<Material> create_material(const MaterialDesc& matDesc)
  //    //{
  //    //  return g_gal->create_material(matDesc);
  //    //}
  //    //rsl::unique_ptr<Sampler2D> create_sampler2d(const SamplerDesc& desc)
  //    //{
  //    //  return g_gal->create_sampler2d(desc);
  //    //}
  //    //rsl::unique_ptr<UnorderedAccessBuffer> create_unordered_access_buffer(rsl::memory_size size, const void* data)
  //    //{
  //    //  return g_gal->create_unordered_access_buffer(size, data);
  //    //}
		//}
	}
}