#pragma once

#include "rex_renderer_core/rendering/primitive_topology.h"
#include "rex_renderer_core/rendering/renderer_output_window_user_data.h"
#include "rex_renderer_core/resource_management/resource_slot.h"
#include "rex_renderer_core/resources/clear_state.h"
#include "rex_renderer_core/resources/compile_shader.h"
#include "rex_renderer_core/resources/constant_buffer.h"
#include "rex_renderer_core/resources/index_buffer.h"
#include "rex_renderer_core/resources/input_layout.h"
#include "rex_renderer_core/resources/link_shader.h"
#include "rex_renderer_core/resources/mesh.h"
#include "rex_renderer_core/resources/pipeline_state.h"
#include "rex_renderer_core/resources/raster_state.h"
#include "rex_renderer_core/resources/shader.h"
#include "rex_renderer_core/resources/vertex_buffer.h"

namespace rex
{
  namespace rhi
  {
    struct Info
    {
      rsl::small_stack_string api_version;
      rsl::small_stack_string shader_version;
      rsl::small_stack_string adaptor;
      rsl::small_stack_string vendor;
    };

    // An object that resets the commandslist and executes it on closing
    class CommandsFrame
    {
    public:
      CommandsFrame();

      CommandsFrame(const CommandsFrame&) = delete;
      CommandsFrame(CommandsFrame&&) = delete;
      ~CommandsFrame();

      CommandsFrame& operator=(const CommandsFrame&) = delete;
      CommandsFrame& operator=(CommandsFrame&&) = delete;
    };

    // Return basic info about the graphics hardware of the current machine
    const Info& info();

    // Initializes the render hardware infrastructure
    // Creates the dxgi factory, d3d device, command buffers, heaps and swapchain
    // After this, the rhi is setup to start creating resources (textures, shaders, vertex buffers, ..)
    bool init(const renderer::OutputWindowUserData& userData);

    // shutdown the internal rhi, all reference to the rhi are invalidated from here on out
    void shutdown();

    // User might want to create render objects during initialization phase
    // therefore we can start and end these phases so the rhi
    // can prepare for these things
    void prepare_user_initialization();
    void finish_user_initialization();

    // Specifying draw area

    // Specify the viewport where we'll be drawing in
    void set_viewport(const Viewport& viewport);

    // Speicfy the scissor rect outside of which we clip drawing
    void set_scissor_rect(const ScissorRect& rect);

    // command line interface
    void reset_command_list(const ResourceSlot& psoSlot);
    void flush_command_queue();
    void exec_command_list();

    // We dealing with resource creation, we need to do this smart as we don't want to have the same object twice in memory.
    // Eg. if an object is requested for rendering, and an equivalent vertex buffer or index buffer is already in memory,
    // we shouldn't put it in memory again, instead we should use the existing memory for this job

    // This can be performed by hashing the vertex buffer and index buffer descriptors
    // and looking into our existing resource pool if such a hash is already exists tied to a resource.
    // If it does, reuse the existing data for this render item
    // If it doesn't already exist, we create a new vertex buffer and/or index buffer
    // and queue for uploading to the gpu.

    // The same strategy can be used for constant buffers, shaders, textures, ..
    // The constant buffer that's unique per object won't (or shouldn't) have any pre existing data
    // Other constant buffers that are shared between objects, can already sit in memory and therefore
    // Don't need to get added again

    // shader API
    // Compile a shader into binary code
    ResourceSlot compile_shader(const CompileShaderDesc& desc);
    // Link a vertex and pixel shader together with a root signature.
    ResourceSlot link_shader(const LinkShaderDesc& desc);
    // Create a shader object from precompiled binary code
    ResourceSlot load_shader(const ShaderDesc& desc);

    // A clear state is just a struct holding different values to clear a buffer
    // Flags control which part of the buffer (color, depths or stencil) should be cleared
    ResourceSlot create_clear_state(const ClearStateDesc& desc);
    // A raster state holds rasterization settings
    // settings like cull mode, fill mode, depth bias, normal orientation, ..
    // are all included in the raster state
    ResourceSlot create_raster_state(const RasterStateDesc& desc);
    // An input layout determines the format of vertices
    // It determines where a shader can find the position, normal, color
    // of a vertex.
    ResourceSlot create_input_layout(const InputLayoutDesc& desc);
    // A vertex buffer is a buffer holding vertices of 1 or more objects
    ResourceSlot create_vertex_buffer(const VertexBufferDesc& desc);
    // A vertex buffer is a buffer holding vertices of 1 or more objects
    ResourceSlot create_vertex_buffer(s32 totalSize, s32 vertexSize);
    // An index buffer is a buffer holding indices of 1 or more objects
    ResourceSlot create_index_buffer(const IndexBufferDesc& desc);
    // An index buffer is a buffer holding indices of 1 or more objects
    ResourceSlot create_index_buffer(s32 totalSize, renderer::IndexBufferFormat format);
    // A constant buffer is a buffer holding data that's accessible to a shader
    // This can hold data like ints, floats, vectors and matrices
    ResourceSlot create_constant_buffer(const ConstantBufferDesc& desc);
    // A constant buffer is a buffer holding data that's accessible to a shader
    // This can hold data like ints, floats, vectors and matrices
    ResourceSlot create_constant_buffer(rsl::memory_size size);
    // A pipeline state object defines a state for the graphics pipeline.
    // It holds the input layout, root signature, shaders, raster state, blend state ..
    // needed for a draw call.
    ResourceSlot create_pso(const PipelineStateDesc& desc);
    
    // bind the render target of the backbuffer to the pipeline
    void bind_backbuffer_rendertarget();

    // Bind the vertex buffer specified at the resource slot to the pipeline
    void set_vertex_buffer(const rhi::ResourceSlot& vb);
    // Bind the index buffer specified at the resource slot to the pipeline
    void set_index_buffer(const rhi::ResourceSlot& ib);
    // Bind the constant buffer to the pipeline at the specified index
    void set_constant_buffer(s32 idx, const rhi::ResourceSlot& cb, struct ID3D12GraphicsCommandList* cmdList = nullptr);
    // Specify the primitive topology of the pipeline
    void set_primitive_topology(renderer::PrimitiveTopology topology);
    // Bind the shader specified at the resource slot to the pipeline
    void set_shader(const rhi::ResourceSlot& slot);
    // Set the pipeline of the pipeline
    void set_pso(const rhi::ResourceSlot& slot);

    // Draw to the screen, using indexed primitives
    void draw_indexed(s32 instanceCount, s32 startInstance, s32 indexCount, s32 startIndex, s32 baseVertexLoc);

    // Reset the upload buffer offset. Should be called at the start of the frame (or after every upload has finished)
    // So new data can be queued for uploading
    void reset_upload_buffer();

    // Update a buffer with new data.
    // Data is always written at the start of the destination resource
    void update_buffer(const ResourceSlot& slot, const void* data, s64 size, struct ID3D12GraphicsCommandList* cmdList = nullptr);

  } // namespace rhi
} // namespace rex