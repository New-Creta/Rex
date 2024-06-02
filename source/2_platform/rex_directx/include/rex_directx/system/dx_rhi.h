#pragma once

#include "rex_renderer_core/rhi/rhi.h"

#include "rex_directx/dxgi/includes.h"
#include "rex_directx/dxgi/factory.h"
#include "rex_directx/utility/dx_util.h"
#include "rex_renderer_core/resource_management/resource_slot.h"
#include "rex_renderer_core/system/gpu_description.h"
#include "rex_renderer_core/resource_management/resource_pool.h"

#include "rex_directx/system/dx_device.h"
#include "rex_directx/system/dx_debug_interface.h"
#include "rex_directx/system/dx_resource_heap.h"
#include "rex_directx/system/dx_descriptor_heap.h"
#include "rex_directx/system/dx_commandlist.h"
#include "rex_directx/system/dx_command_queue.h"
#include "rex_directx/system/dx_swapchain.h"
//#include "rex_directx/system/dx_pipeline_library.h"
//#include "rex_directx/system/dx_texture_2d.h"
#include "rex_directx/system/dx_resource.h"
#include "rex_directx/resources/dx_upload_buffer.h"
//#include "rex_directx/system/dx_constant_buffer.h"

//#include "rex_directx/system/dx_vertex_buffer.h"
//#include "rex_directx/system/dx_index_buffer.h"
//#include "rex_directx/system/dx_root_signature.h"
//#include "rex_directx/system/dx_pipeline_state.h"




#include "rex_std/string_view.h"
#include "rex_engine/engine/defines.h"
#include "rex_engine/engine/types.h"

struct ID3D12GraphicsCommandList;

namespace rex
{
  namespace rhi
  {
    REX_STATIC_WARNING("The following func doesn't belong here and is more a utility/debug functionality");
    // Useful function that assigns a name to a resource.
    // This is useful for debugging
    template<typename TResourceType>
    void set_debug_name_for(TResourceType* resource, rsl::string_view name)
    {
#ifdef REX_ENABLE_DEBUG_RESOURCE_NAMES
      resource->SetPrivateData(WKPDID_D3DDebugObjectName, name.length(), name.data());
#else
      REX_UNUSED_PARAM(resource);
      REX_UNUSED_PARAM(name);
#endif
    }


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

    //rsl::unique_ptr<CommandList> create_commandlist(ResourceStateTracker* resourceStateTracker = nullptr);
    //rsl::unique_ptr<CommandQueue> create_command_queue();
    //rsl::unique_ptr<Swapchain> create_swapchain(s32 bufferCount, void* primaryDisplayHandle);
    //rsl::unique_ptr<CommandAllocator> create_command_allocator();
    //rsl::unique_ptr<RenderTarget> create_render_target_from_backbuffer(Resource2* resource);
    //rsl::unique_ptr<VertexBuffer> create_vertex_buffer(s32 numVertices, s32 vertexSize);
    //rsl::unique_ptr<IndexBuffer> create_index_buffer(s32 numIndices, renderer::IndexBufferFormat format);
    //rsl::unique_ptr<RootSignature> create_root_signature(const RootSignatureDesc& desc);
    //rsl::unique_ptr<PipelineState> create_pso(const rex::rhi::PipelineStateDesc& desc);
    //rsl::unique_ptr<Texture2D> create_texture2d(s32 width, s32 height, renderer::TextureFormat format);
    //rsl::unique_ptr<RasterStateResource> create_raster_state(const RasterStateDesc& desc);
    //rsl::unique_ptr<ConstantBuffer> create_constant_buffer(rsl::memory_size size);
    //rsl::unique_ptr<InputLayout> create_input_layout(const InputLayoutDesc& desc);
    //rsl::unique_ptr<VertexShader> create_vertex_shader(rsl::string_view sourceCode);
    //rsl::unique_ptr<PixelShader> create_pixel_shader(rsl::string_view sourceCode);
    //rsl::unique_ptr<UploadBuffer> create_upload_buffer(rsl::memory_size size);

    namespace api
    {
      wrl::ComPtr<ID3DBlob> compile_shader(const CompileShaderDesc& desc);
      wrl::ComPtr<ID3D12GraphicsCommandList> create_commandlist(rhi::CommandAllocator* alloc, rhi::CommandType type);
      rsl::unique_ptr<Texture2D> create_texture2d(const wrl::ComPtr<ID3D12Resource>& resource);
    }

    DescriptorHeap* cbv_uav_srv_desc_heap();
    UploadBuffer* global_upload_buffer();
    //ID3D12Device1* get_device();

    void execute_command_list(CommandList* cmdList);
    void wait_for_gpu(CommandType type);

    // Resource Creation
    // -----------------------------------------------------------------------
    // A clear state is just a struct holding different values to clear a buffer
    // Flags control which part of the buffer (color, depths or stencil) should be cleared
    //ResourceSlot create_clear_state(const ClearStateDesc& desc);

    // A raster state holds rasterization settings
    // settings like cull mode, fill mode, depth bias, normal orientation, ..
    // are all included in the raster state
    //ResourceSlot create_raster_state(const RasterStateDesc& desc);

    // An input layout determines the format of vertices
    // It determines where a shader can find the position, normal, color
    // of a vertex.
    //ResourceSlot create_input_layout(const InputLayoutDesc& desc);

    // A vertex buffer is a buffer holding vertices of 1 or more objects
    //ResourceSlot create_vertex_buffer(const VertexBufferDesc& desc);

    // A vertex buffer is a buffer holding vertices of 1 or more objects
    //ResourceSlot create_vertex_buffer(s32 totalSize, s32 vertexSize);

    // An index buffer is a buffer holding indices of 1 or more objects
    //ResourceSlot create_index_buffer(const IndexBufferDesc& desc);

    // An index buffer is a buffer holding indices of 1 or more objects
    //ResourceSlot create_index_buffer(s32 totalSize, renderer::IndexBufferFormat format);

    // A constant buffer is a buffer holding data that's accessible to a shader
    // This can hold data like ints, floats, vectors and matrices
    //ResourceSlot create_constant_buffer(const ConstantBufferDesc& desc);

    // A constant buffer is a buffer holding data that's accessible to a shader
    // This can hold data like ints, floats, vectors and matrices
    //ResourceSlot create_constant_buffer(rsl::memory_size size);

    // A pipeline state object defines a state for the graphics pipeline.
    // It holds the input layout, root signature, shaders, raster state, blend state ..
    // needed for a draw call.
    //ResourceSlot create_pso(const PipelineStateDesc& desc);

    
    // Shader API
    // -----------------------------------------------------------------------
    // Compile a shader into binary code
    //ResourceSlot compile_shader(const CompileShaderDesc& desc);
    // Link a vertex and pixel shader together with a root signature.
    //ResourceSlot link_shader(const LinkShaderDesc& desc);
    // Create a shader object from precompiled binary code
    //ResourceSlot load_shader(const ShaderDesc& desc);

    // Graphics Pipeline
    // -----------------------------------------------------------------------
    // Specify the viewport where we'll be drawing in
    //void set_viewport(CommandList* cmdList, const Viewport& viewport);
    // Speicfy the scissor rect outside of which we clip drawing
    //void set_scissor_rect(CommandList* cmdList, const ScissorRect& rect);

    // Transition the back buffer into a specified state
    //void transition_backbuffer(CommandList* cmdList, D3D12_RESOURCE_STATES state);
    // Clear the back buffer using a clear state
    //void clear_backbuffer(CommandList* cmdList, const ResourceSlot& clearState);

    // Bind the vertex buffer specified at the resource slot to the pipeline
    //void set_vertex_buffer(CommandList* cmdList, const ResourceSlot& vb);
    // Bind the index buffer specified at the resource slot to the pipeline
    //void set_index_buffer(CommandList* cmdList, const ResourceSlot& ib);
    // Bind the constant buffer to the pipeline at the specified index
    //void set_constant_buffer(CommandList* cmdList, s32 idx, const ResourceSlot& cb);
    // Specify the primitive topology of the pipeline
    //void set_primitive_topology(CommandList* cmdList, renderer::PrimitiveTopology topology);
    // Specify the blend factor of the pipeline
    //void set_blend_factor(CommandList* cmdList, const f32 blendFactor[4]);
    // Bind the shader specified at the resource slot to the pipeline
    //void set_shader(CommandList* cmdList, const ResourceSlot& slot);
    // Set the pipeline of the pipeline
    //void set_pso(CommandList* cmdList, const ResourceSlot& slot);

    // Draw to the screen, using indexed instances
    //void draw_indexed(s32 instanceCount, s32 startInstance, s32 indexCount, s32 startIndex, s32 baseVertexLoc);
    // Draw to the screen, using indexed instances of primitives
    //void draw_indexed_instanced(CommandList* cmdList, s32 indexCountPerInstance, s32 instanceCount, s32 startIndexLocation, s32 baseVertexLocation, s32 startInstanceLocation);
  }
}