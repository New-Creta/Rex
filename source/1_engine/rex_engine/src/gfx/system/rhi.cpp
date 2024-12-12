#include "rex_engine/gfx/system/rhi.h"

namespace rex
{
	namespace gfx
	{
		namespace rhi
		{
      GALInterface* g_gal;

      // Initializes the render hardware infrastructure
      // For DirectX, creates the dxgi factory, d3d device, command buffers, heaps and swapchain
      // After this, the rhi is setup to start creating resources (textures, shaders, vertex buffers, ..)
      gfx::GpuEngine* init(const OutputWindowUserData& userData)
      {

      }

      // shutdown the internal rhi, all reference to the rhi are invalidated from here on out
      void shutdown()
      {

      }

      // Return basic info about the graphics hardware of the current machine
      const Info& info()
      {

      }

      // Configuration
      ShaderPlatform shader_platform()
      {

      }

      // Backend Systems
      rsl::unique_ptr<CommandQueue> create_command_queue(GraphicsEngineType type)
      {
      }
      rsl::unique_ptr<Swapchain> create_swapchain(void* apiDevice, s32 bufferCount, void* primaryDisplayHandle)
      {

      }
      rsl::unique_ptr<CommandAllocator> create_command_allocator(GraphicsEngineType type)
      {

      }

      // Resource creation
      rsl::unique_ptr<RenderTarget> create_render_target(s32 width, s32 height, TextureFormat format)
      {

      }
      rsl::unique_ptr<VertexBuffer> create_vertex_buffer(s32 numVertices, s32 vertexSize, const void* data)
      {

      }
      rsl::unique_ptr<IndexBuffer> create_index_buffer(s32 numIndices, IndexBufferFormat format, const void* data)
      {

      }
      rsl::unique_ptr<RootSignature> create_root_signature(const rsl::vector<ShaderParameterDeclaration>& parameters)
      {

      }
      rsl::unique_ptr<PipelineState> create_pso(const PipelineStateDesc& desc)
      {

      }
      rsl::unique_ptr<Texture2D> create_texture2d(s32 width, s32 height, TextureFormat format, const void* data)
      {

      }
      rsl::unique_ptr<DepthStencilBuffer> create_depth_stencil_buffer(s32 width, s32 height, TextureFormat format, const ClearStateDesc& clearStateDesc)
      {

      }
      rsl::unique_ptr<ConstantBuffer> create_constant_buffer(rsl::memory_size size, rsl::string_view debugName)
      {

      }
      rsl::unique_ptr<InputLayout> create_input_layout(const InputLayoutDesc& desc)
      {

      }
      rsl::unique_ptr<Shader> create_vertex_shader(rsl::string_view sourceCode, rsl::string_view shaderName)
      {

      }
      rsl::unique_ptr<Shader> create_vertex_shader(const memory::Blob& byteBlob)
      {

      }
      rsl::unique_ptr<Shader> create_pixel_shader(rsl::string_view sourceCode, rsl::string_view shaderName)
      {

      }
      rsl::unique_ptr<Shader> create_pixel_shader(const memory::Blob& byteBlob)
      {

      }
      rsl::unique_ptr<UploadBuffer> create_upload_buffer(rsl::memory_size size)
      {

      }
      rsl::unique_ptr<Material> create_material(const MaterialDesc& matDesc)
      {

      }
      rsl::unique_ptr<Sampler2D> create_sampler2d(const SamplerDesc& desc)
      {

      }
      rsl::unique_ptr<UnorderedAccessBuffer> create_unordered_access_buffer(rsl::memory_size size, const void* data)
      {

      }
		}
	}
}