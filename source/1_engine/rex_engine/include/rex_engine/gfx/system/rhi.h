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
#include "rex_engine/gfx/materials/material.h"
#include "rex_engine/gfx/system/shader_pipeline.h"
#include "rex_engine/gfx/shader_reflection/shader_signature.h"
#include "rex_engine/gfx/shader_reflection/shader_param_reflection.h"
#include "rex_engine/gfx/system/shader_param_declaration.h"

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

    // All logic inside the "api" namespace is only declared
    // The definition of these functions and/or classes are found in the graphics API specific rhi code.
    // eg. rhi::shader_platform for DirectX is defined in dx_rhi.cpp
    // The reason we've made a seperate namespace is just for readability.

    namespace rhi
    {
      // Initializes the render hardware infrastructure
      // For DirectX, creates the dxgi factory, d3d device, command buffers, heaps and swapchain
      // After this, the rhi is setup to start creating resources (textures, shaders, vertex buffers, ..)
      gfx::GpuEngine* init(const OutputWindowUserData& userData);

      // shutdown the internal rhi, all reference to the rhi are invalidated from here on out
      void shutdown();

      // Return basic info about the graphics hardware of the current machine
      const Info& info();

      // Configuration
      ShaderPlatform shader_platform();

      // Backend Systems
      rsl::unique_ptr<CommandQueue> create_command_queue(GraphicsEngineType type);
      rsl::unique_ptr<Swapchain> create_swapchain(void* apiDevice, s32 bufferCount, void* primaryDisplayHandle);
      rsl::unique_ptr<CommandAllocator> create_command_allocator(GraphicsEngineType type);

      // Resource creation
      rsl::unique_ptr<RenderTarget> create_render_target(s32 width, s32 height, TextureFormat format);
      rsl::unique_ptr<VertexBuffer> create_vertex_buffer(s32 numVertices, s32 vertexSize, const void* data = nullptr);
      rsl::unique_ptr<IndexBuffer> create_index_buffer(s32 numIndices, IndexBufferFormat format, const void* data = nullptr);
      rsl::unique_ptr<RootSignature> create_root_signature(const rsl::vector<ShaderParameterDeclaration>& parameters);
      rsl::unique_ptr<PipelineState> create_pso(const PipelineStateDesc& desc);
      rsl::unique_ptr<Texture2D> create_texture2d(s32 width, s32 height, TextureFormat format, const void* data = nullptr);
      rsl::unique_ptr<DepthStencilBuffer> create_depth_stencil_buffer(s32 width, s32 height, TextureFormat format, const ClearStateDesc& clearStateDesc);
      rsl::unique_ptr<ConstantBuffer> create_constant_buffer(rsl::memory_size size, rsl::string_view debugName = "Constant Buffer");
      rsl::unique_ptr<InputLayout> create_input_layout(const InputLayoutDesc& desc);
      rsl::unique_ptr<Shader> create_vertex_shader(rsl::string_view sourceCode, rsl::string_view shaderName = "");
      rsl::unique_ptr<Shader> create_vertex_shader(const memory::Blob& byteBlob);
      rsl::unique_ptr<Shader> create_pixel_shader(rsl::string_view sourceCode, rsl::string_view shaderName = "");
      rsl::unique_ptr<Shader> create_pixel_shader(const memory::Blob& byteBlob);
      rsl::unique_ptr<UploadBuffer> create_upload_buffer(rsl::memory_size size);
      rsl::unique_ptr<Material> create_material(const MaterialDesc& matDesc);
      rsl::unique_ptr<Sampler2D> create_sampler2d(const SamplerDesc& desc);
    }
  } // namespace gfx
} // namespace rex
