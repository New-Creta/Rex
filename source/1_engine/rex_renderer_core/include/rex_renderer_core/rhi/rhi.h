#pragma once

#include "rex_renderer_core/rhi/primitive_topology.h"
#include "rex_renderer_core/rendering/renderer_output_window_user_data.h"
#include "rex_renderer_core/resource_management/resource_slot.h"
#include "rex_renderer_core/resources/clear_state.h"
#include "rex_renderer_core/system/compile_shader.h"
#include "rex_renderer_core/resources/constant_buffer.h"
#include "rex_renderer_core/resources/index_buffer.h"
#include "rex_renderer_core/resources/input_layout.h"
#include "rex_renderer_core/system/link_shader.h"
#include "rex_renderer_core/resources/pipeline_state.h"
#include "rex_renderer_core/resources/raster_state.h"
#include "rex_renderer_core/resources/shader.h"
#include "rex_renderer_core/resources/vertex_buffer.h"


// new includes
#include "rex_renderer_core/rhi/texture_format.h"
#include "rex_renderer_core/rhi/index_buffer_format.h"
#include "rex_renderer_core/shaders/shader_platform.h"

#include "rex_renderer_core/resources/resource.h"
#include "rex_renderer_core/resources/render_target.h"
#include "rex_renderer_core/resources/texture_2d.h"
#include "rex_renderer_core/system/command_queue.h"
#include "rex_renderer_core/system/swapchain.h"
#include "rex_renderer_core/resources/vertex_buffer.h"
#include "rex_renderer_core/resources/index_buffer.h"
#include "rex_renderer_core/resources/root_signature.h"
#include "rex_renderer_core/resources/pipeline_state.h"
#include "rex_renderer_core/resources/raster_state.h"
#include "rex_renderer_core/resources/constant_buffer.h"
#include "rex_renderer_core/resources/input_layout.h"

namespace rex
{
  namespace gfx
  {
    class GpuEngine;
  }

  namespace rhi
  {
    // Some info about the rendering infrastructure
    struct Info
    {
      rsl::small_stack_string api;              // eg: DirectX
      rsl::small_stack_string api_version;      // eg: D3D_FEATURE_LEVEL_12_0
      rsl::small_stack_string shader_version;   // eg: D3D_SHADER_MODEL_6_5
      rsl::small_stack_string adaptor;          // eg: NVIDIA GeForce RTX 3060 Ti
      rsl::small_stack_string vendor;           // eg: NVIDIA
      rsl::small_stack_string driver_version;   // eg: 552.85
      rsl::memory_size available_memory;        // eg: 8 388 608
    };

    // All logic inside the "api" namespace is only declared
    // The definition of these functions and/or classes are found in the graphics API specific rhi code.
    // eg. rex::rhi::api::shader_platform for DirectX is defined in dx_rhi.cpp
    // The reason we've made a seperate namespace is just for readability.
    namespace api
    {
      // Initializes the render hardware infrastructure
      // For DirectX, creates the dxgi factory, d3d device, command buffers, heaps and swapchain
      // After this, the rhi is setup to start creating resources (textures, shaders, vertex buffers, ..)
      gfx::GpuEngine* init(const renderer::OutputWindowUserData& userData);

      // shutdown the internal rhi, all reference to the rhi are invalidated from here on out
      void shutdown();

      // Return basic info about the graphics hardware of the current machine
      const Info& info();

      // Configuration
      renderer::ShaderPlatform shader_platform();

      // Backend Systems
      //rsl::unique_ptr<CommandList> create_commandlist(CommandAllocator* alloc, CommandType type, ResourceStateTracker* resourceStateTracker = nullptr);
      rsl::unique_ptr<CommandQueue> create_command_queue(CommandType type);
      rsl::unique_ptr<Swapchain> create_swapchain(void* apiDevice, s32 bufferCount, void* primaryDisplayHandle);
      rsl::unique_ptr<CommandAllocator> create_command_allocator(rhi::CommandType type);

      // Resource creation
      rsl::unique_ptr<RenderTarget> create_render_target(Texture2D* texture);

      rsl::unique_ptr<VertexBuffer> create_vertex_buffer(s32 numVertices, s32 vertexSize);
      rsl::unique_ptr<IndexBuffer> create_index_buffer(s32 numIndices, renderer::IndexBufferFormat format);
      rsl::unique_ptr<RootSignature> create_root_signature(const RootSignatureDesc& desc);
      rsl::unique_ptr<PipelineState> create_pso(const rhi::PipelineStateDesc& desc);
      rsl::unique_ptr<Texture2D> create_texture2d(s32 width, s32 height, renderer::TextureFormat format, const void* data = nullptr);
      rsl::unique_ptr<RasterStateResource> create_raster_state(const RasterStateDesc& desc);
      rsl::unique_ptr<ConstantBuffer> create_constant_buffer(rsl::memory_size size);
      rsl::unique_ptr<InputLayout> create_input_layout(const InputLayoutDesc& desc);
      rsl::unique_ptr<Shader> create_vertex_shader(rsl::string_view sourceCode);
      rsl::unique_ptr<Shader> create_pixel_shader(rsl::string_view sourceCode);

      RenderTarget* get_render_target();
    }

    // the api namespace isn't needed to access these functions
    // It's only used for readablity to make clear what needs to be defined
    // in graphics API specific code and what can be made generic across all graphics APIs
    using namespace rex::rhi::api;

  } // namespace rhi
} // namespace rex
