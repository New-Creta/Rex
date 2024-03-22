#pragma once

#include "rex_renderer_core/resource_management/resource_slot.h"

#include "rex_renderer_core/resources/buffer_desc.h"
#include "rex_renderer_core/resources/compile_shader.h"
#include "rex_renderer_core/resources/clear_state.h"
#include "rex_renderer_core/resources/input_layout.h"
#include "rex_renderer_core/resources/link_shader.h"
#include "rex_renderer_core/resources/load_shader.h"
#include "rex_renderer_core/resources/pipeline_state.h"
#include "rex_renderer_core/resources/raster_state.h"

namespace rex
{
  namespace rhi
  {
    // Initializes the render hardware infrastructure
    // Creates the dxgi factory, d3d device, command buffers, heaps and swapchain
    // After this, the rhi is setup to start creating resources (textures, shaders, vertex buffers, ..)
    class RenderHardwareInfrastructure* init(const renderer::OutputWindowUserData& userData);

    // shutdown the internal rhi, all reference to the rhi are invalidated from here on out
    void shutdown();

    // command line interface
    void reset_command_list();
    void flush_command_queue();
    void exec_command_list();

    // shader API
    // Compile a shader into binary code
    ResourceSlot compile_shader(const CompileShaderDesc& desc);
    // Link a vertex and pixel shader together with a root signature.
    ResourceSlot link_shader(const LinkShaderDesc& desc);
    // Create a shader object from precompiled binary code
    ResourceSlot load_shader(const LoadShaderDesc& desc);

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
    ResourceSlot create_vertex_buffer(const BufferDesc& desc);
    // An index buffer is a buffer holding indices of 1 or more objects
    ResourceSlot create_index_buffer(const BufferDesc& desc);
    // A constant buffer is a buffer holding data that's accessible to a shader
    // This can hold data like ints, floats, vectors and matrices
    ResourceSlot create_consant_buffer(const BufferDesc& desc);
    // A pipeline state object defines a state for the graphics pipeline.
    // It holds the input layout, root signature, shaders, raster state, blend state ..
    // needed for a draw call.
    ResourceSlot create_pso(const PipelineStateDesc& desc);
  }
}