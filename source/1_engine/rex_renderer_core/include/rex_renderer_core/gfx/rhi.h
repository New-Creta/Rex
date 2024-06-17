#pragma once

#include "rex_renderer_core/gfx/primitive_topology.h"
#include "rex_renderer_core/gfx/renderer_output_window_user_data.h"
#include "rex_renderer_core/resource_management/resource_slot.h"
#include "rex_renderer_core/resources/clear_state.h"
#include "rex_renderer_core/system/compile_shader.h"
#include "rex_renderer_core/resources/constant_buffer.h"
#include "rex_renderer_core/resources/index_buffer.h"
#include "rex_renderer_core/resources/input_layout.h"
#include "rex_renderer_core/system/shader_elements.h"
#include "rex_renderer_core/resources/pipeline_state.h"
#include "rex_renderer_core/resources/raster_state.h"
#include "rex_renderer_core/resources/shader.h"
#include "rex_renderer_core/resources/vertex_buffer.h"


// new includes
#include "rex_renderer_core/gfx/texture_format.h"
#include "rex_renderer_core/gfx/index_buffer_format.h"
#include "rex_renderer_core/system/shader_platform.h"

#include "rex_renderer_core/resources/resource.h"
#include "rex_renderer_core/resources/render_target.h"
#include "rex_renderer_core/resources/texture_2d.h"
#include "rex_renderer_core/system/command_queue.h"
#include "rex_renderer_core/system/command_allocator.h"
#include "rex_renderer_core/system/swapchain.h"
#include "rex_renderer_core/resources/vertex_buffer.h"
#include "rex_renderer_core/resources/index_buffer.h"
#include "rex_renderer_core/resources/root_signature.h"
#include "rex_renderer_core/resources/pipeline_state.h"
#include "rex_renderer_core/resources/raster_state.h"
#include "rex_renderer_core/resources/constant_buffer.h"
#include "rex_renderer_core/resources/input_layout.h"
#include "rex_renderer_core/resources/upload_buffer.h"

namespace rex
{
  namespace gfx
  {
    class GpuEngine;
  }

  namespace gfx
  {
    struct Info;
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
      rsl::unique_ptr<VertexBuffer> create_vertex_buffer(s32 numVertices, s32 vertexSize);
      rsl::unique_ptr<IndexBuffer> create_index_buffer(s32 numIndices, IndexBufferFormat format);
      rsl::unique_ptr<RootSignature> create_root_signature(const RootSignatureDesc& desc);
      rsl::unique_ptr<PipelineState> create_pso(const PipelineStateDesc& desc);
      rsl::unique_ptr<Texture2D> create_texture2d(s32 width, s32 height, TextureFormat format, const void* data = nullptr);
      rsl::unique_ptr<ConstantBuffer> create_constant_buffer(rsl::memory_size size);
      rsl::unique_ptr<InputLayout> create_input_layout(const InputLayoutDesc& desc);
      rsl::unique_ptr<Shader> create_vertex_shader(rsl::string_view sourceCode);
      rsl::unique_ptr<Shader> create_pixel_shader(rsl::string_view sourceCode);
      rsl::unique_ptr<UploadBuffer> create_upload_buffer(rsl::memory_size size);

      // Return the render target pointing to the current swapchain's backbuffer
      RenderTarget* current_backbuffer_rt();
    }
  } // namespace gfx
} // namespace rex
