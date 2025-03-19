#pragma once

#include "rex_engine/gfx/core/primitive_topology.h"
#include "rex_engine/gfx/core/renderer_output_window_user_data.h"
#include "rex_engine/gfx/resources/clear_state.h"
#include "rex_engine/gfx/system/compile_shader.h"
#include "rex_engine/gfx/resources/constant_buffer.h"
#include "rex_engine/gfx/resources/index_buffer.h"
#include "rex_engine/gfx/resources/input_layout.h"

#include "rex_engine/gfx/resources/pipeline_state.h"
#include "rex_engine/gfx/resources/raster_state.h"
#include "rex_engine/gfx/resources/shader.h"
#include "rex_engine/gfx/resources/vertex_buffer.h"


// new includes
#include "rex_engine/gfx/core/texture_format.h"
#include "rex_engine/gfx/core/index_buffer_format.h"
#include "rex_engine/gfx/system/shader_platform.h"

#include "rex_engine/gfx/resources/resource.h"
#include "rex_engine/gfx/resources/render_target.h"
#include "rex_engine/gfx/resources/texture_2d.h"
#include "rex_engine/gfx/resources/sampler_2d.h"
#include "rex_engine/gfx/system/command_queue.h"
#include "rex_engine/gfx/system/command_allocator.h"
#include "rex_engine/gfx/system/swapchain.h"
#include "rex_engine/gfx/resources/vertex_buffer.h"
#include "rex_engine/gfx/resources/index_buffer.h"
#include "rex_engine/gfx/resources/root_signature.h"
#include "rex_engine/gfx/resources/pipeline_state.h"
#include "rex_engine/gfx/resources/raster_state.h"
#include "rex_engine/gfx/resources/constant_buffer.h"
#include "rex_engine/gfx/resources/input_layout.h"
#include "rex_engine/gfx/resources/upload_buffer.h"
#include "rex_engine/gfx/resources/depth_stencil_buffer.h"
#include "rex_engine/gfx/resources/unordered_access_buffer.h"
#include "rex_engine/gfx/materials/material.h"
#include "rex_engine/gfx/system/shader_pipeline.h"
#include "rex_engine/gfx/shader_reflection/shader_signature.h"
#include "rex_engine/gfx/shader_reflection/shader_param_reflection.h"
#include "rex_engine/gfx/system/shader_param_declaration.h"

  // #TODO: Remaining cleanup of development/Pokemon -> main merge. ID: GRAPHICS
namespace rex
{
  namespace gfx
  {
    class GpuEngine;
    class Material;
  }

  namespace gfx
  {
    struct Info;
    struct MaterialDesc;

    // GAL stands for Graphics Abstraction Layer
    // It is the layer between the program and the graphics systems
    class GALInterface
    {
    public:
      virtual ~GALInterface() = default;

      // Initialize the graphics systesm and create a gpu engine
      virtual gfx::GpuEngine* create_gpu_engine(const OutputWindowUserData& userData) = 0;

      // Return basic info about the graphics hardware of the current machine
      virtual const Info& info() const = 0;

      // Configuration
      virtual ShaderPlatform shader_platform() const = 0;

      // Backend Systems
      virtual rsl::unique_ptr<CommandQueue> create_command_queue(GraphicsEngineType type) = 0;
      virtual rsl::unique_ptr<Swapchain> create_swapchain(void* apiDevice, s32 bufferCount, void* primaryDisplayHandle) = 0;
      virtual rsl::unique_ptr<CommandAllocator> create_command_allocator(GraphicsEngineType type) = 0;

      // Resource creation
      virtual rsl::unique_ptr<RenderTarget> create_render_target(s32 width, s32 height, TextureFormat format) = 0;
      virtual rsl::unique_ptr<VertexBuffer> create_vertex_buffer(s32 numVertices, s32 vertexSize, const void* data = nullptr) = 0;
      virtual rsl::unique_ptr<IndexBuffer> create_index_buffer(s32 numIndices, IndexBufferFormat format, const void* data = nullptr) = 0;
      virtual rsl::unique_ptr<RootSignature> create_root_signature(const rsl::vector<ShaderParameterDeclaration>& parameters) = 0;
      virtual rsl::unique_ptr<PipelineState> create_pso(const PipelineStateDesc& desc) = 0;
      virtual rsl::unique_ptr<Texture2D> create_texture2d(s32 width, s32 height, TextureFormat format, const void* data = nullptr) = 0;
      virtual rsl::unique_ptr<DepthStencilBuffer> create_depth_stencil_buffer(s32 width, s32 height, TextureFormat format, const ClearStateDesc& clearStateDesc) = 0;
      virtual rsl::unique_ptr<ConstantBuffer> create_constant_buffer(rsl::memory_size size, rsl::string_view debugName = "Constant Buffer") = 0;
      virtual rsl::unique_ptr<InputLayout> create_input_layout(const InputLayoutDesc& desc) = 0;
      virtual rsl::unique_ptr<Shader> create_vertex_shader(rsl::string_view sourceCode, rsl::string_view shaderName = "") = 0;
      virtual rsl::unique_ptr<Shader> create_vertex_shader(const memory::Blob& byteBlob) = 0;
      virtual rsl::unique_ptr<Shader> create_pixel_shader(rsl::string_view sourceCode, rsl::string_view shaderName = "") = 0;
      virtual rsl::unique_ptr<Shader> create_pixel_shader(const memory::Blob& byteBlob) = 0;
      virtual rsl::unique_ptr<UploadBuffer> create_upload_buffer(rsl::memory_size size) = 0;
      virtual rsl::unique_ptr<Material> create_material(const MaterialDesc& matDesc) = 0;
      virtual rsl::unique_ptr<Sampler2D> create_sampler2d(const SamplerDesc& desc) = 0;
      virtual rsl::unique_ptr<UnorderedAccessBuffer> create_unordered_access_buffer(rsl::memory_size size, const void* data = nullptr) = 0;

      virtual ShaderSignature reflect_shader(const gfx::Shader* shader) = 0;

    private:

    };

    GALInterface* gal();

		gfx::GpuEngine* init_gal(rsl::unique_ptr<GALInterface> galInterface, const OutputWindowUserData& userData);
		void shutdown_gal();
  } // namespace gfx
} // namespace rex
