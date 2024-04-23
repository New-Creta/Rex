#include "rex_renderer_core/renderer.h"
#include "rex_renderer_core/renderer_backend.h"
#include "rex_renderer_core/resource_slot.h"
#include "rex_renderer_core/renderer_info.h"
#include "rex_engine/scenegraph/scene.h"

namespace rex
{
  namespace renderer
  {
    //-------------------------------------------------------------------------
    bool initialize(const OutputWindowUserData& userData)
    {
      return backend::initialize(userData);      
    }

    //-------------------------------------------------------------------------
    bool post_initialize()
    {
      return backend::post_initialize();
    }

    //-------------------------------------------------------------------------
    void shutdown()
    {
      backend::shutdown();
    }

    //-------------------------------------------------------------------------
    const Info& info()
    {
      return backend::info();
    }

    //-------------------------------------------------------------------------
    bool is_y_up()
    {
      return backend::is_y_up();
    }

    //-------------------------------------------------------------------------
    bool is_depth_0_to_1()
    {
      return backend::is_depth_0_to_1();
    }

    //-------------------------------------------------------------------------
    void draw_scene(Scene* scene)
    {
      backend::draw_scene(scene);
    }

    //-------------------------------------------------------------------------
    void draw_gui()
    {
      backend::draw_gui();
    }

    //-------------------------------------------------------------------------
    void present()
    {
      backend::present();
    }

    //-------------------------------------------------------------------------
    void flush()
    {
      backend::flush();
    }

    //-------------------------------------------------------------------------
    ResourceSlot copy_vertex_buffer(size_t numVertices, size_t vertexStride, const void* vertexBufferData)
    {
      return backend::copy_vertex_buffer(numVertices, vertexStride, vertexBufferData);
    }

    //-------------------------------------------------------------------------
    ResourceSlot copy_index_buffer(size_t numIndices, Format indexFormat, const void* indexBufferData)
    {
      return backend::copy_index_buffer(numIndices, indexFormat, indexBufferData);
    }

    //-------------------------------------------------------------------------
    ResourceSlot create_clear_state(CreateClearStateDesc&& desc)
    {
      return backend::create_clear_state(rsl::move(desc));
    }

    //-------------------------------------------------------------------------
    ResourceSlot create_raster_state(CreateRasterStateDesc&& desc)
    {
      return backend::create_raster_state(rsl::move(desc));
    }

    //-------------------------------------------------------------------------
    ResourceSlot create_texture(CreateTextureDesc&& desc) 
    {
      return backend::create_texture(rsl::move(desc));
    }

    //-------------------------------------------------------------------------
    ResourceSlot create_vertex_buffer(CreateVertexBufferDesc&& desc)
    {
      return backend::create_vertex_buffer(rsl::move(desc));
    }

    //-------------------------------------------------------------------------
    ResourceSlot create_index_buffer(CreateIndexBufferDesc&& desc)
    {
      return backend::create_index_buffer(rsl::move(desc));
    }

    //-------------------------------------------------------------------------
    ResourceSlot create_pipeline_state_object(CreatePipelineStateDesc&& desc)
    {
      return backend::create_pipeline_state_object(rsl::move(desc));
    }

    //-------------------------------------------------------------------------
    void release_resource(const ResourceSlot& resource)
    {
      backend::release_resource(resource);
    }

    //-------------------------------------------------------------------------
    void clear_texture(const ResourceSlot& clearStateTarget, const ResourceSlot& clearState)
    {
      backend::clear_texture(clearStateTarget, clearState);
    }

    //-------------------------------------------------------------------------
    void clear_depth_stencil_texture(const ResourceSlot& clearStateTarget, const ResourceSlot& clearState)
    {
      backend::clear_depth_stencil_texture(clearStateTarget, clearState);
    }

    //-------------------------------------------------------------------------
    void draw_geometry(u32 vertexCount, u32 instanceCount, u32 startVertex, u32 startInstance)
    {
      backend::draw_geometry(vertexCount, instanceCount, startVertex, startInstance);
    }

    //-------------------------------------------------------------------------
    void draw_geometry_indexed(u32 indexCount, u32 instanceCount, u32 startIndex, s32 baseVertex, u32 startInstance)
    {
      backend::draw_geometry_indexed(indexCount, instanceCount, startIndex, baseVertex, startInstance);
    }

    //-------------------------------------------------------------------------
    void set_render_target(const ResourceSlot& renderTarget)
    {
      backend::set_render_target(renderTarget);
    }

    //-------------------------------------------------------------------------
    void set_pipeline_state(const ResourceSlot& pipelineState)
    {
      backend::set_pipeline_state(pipelineState);
    }

    //-------------------------------------------------------------------------
    void set_graphics_dynamic_constant_buffer(u32 rootParameterIndex, rsl::memory_size sizeInBytes, const void* bufferData)
    {
      backend::set_graphics_dynamic_constant_buffer(rootParameterIndex, sizeInBytes, bufferData);
    }

    //-------------------------------------------------------------------------
    void set_graphics_32_bit_constants(u32 rootParameterIndex, u32 numConstants, const void* constants)
    {
      backend::set_graphics_32_bit_constants(rootParameterIndex, numConstants, constants);
    }

    //-------------------------------------------------------------------------
    void set_vertex_buffer(u32 slot, const ResourceSlot& vertexBuffer)
    {
      backend::set_vertex_buffer(slot, vertexBuffer);
    }

    //-------------------------------------------------------------------------
    void set_vertex_buffers(u32 slot, const rsl::vector<ResourceSlot>& vertexBuffers)
    {
      backend::set_vertex_buffers(slot, vertexBuffers);
    }

    //-------------------------------------------------------------------------
    void set_dynamic_vertex_buffer(u32 slot, size_t numVertices, rsl::memory_size vertexSize, const void* vertexBufferData)
    {
      backend::set_dynamic_vertex_buffer(slot, numVertices, vertexSize, vertexBufferData);
    }

    //-------------------------------------------------------------------------
    void set_index_buffer(const ResourceSlot& indexBuffer)
    {
      backend::set_index_buffer(indexBuffer);
    }

    //-------------------------------------------------------------------------
    void set_dynamic_index_buffer(size_t numIndicies, Format indexFormat, const void* indexBufferData)
    {
      backend::set_dynamic_index_buffer(numIndicies, indexFormat, indexBufferData);
    }

    //-------------------------------------------------------------------------
    void set_primitive_topology(PrimitiveTopology primitiveTopology)
    {
      backend::set_primitive_topology(primitiveTopology);
    }

    //-------------------------------------------------------------------------
    void set_viewport(const Viewport& vp)
    {
      backend::set_viewport(vp);
    }

    //-------------------------------------------------------------------------
    void set_viewports(const rsl::vector<Viewport>& viewports)
    {
      backend::set_viewports(viewports);
    }

    //-------------------------------------------------------------------------
    void set_scissor_rect(const ScissorRect& sr)
    {
      backend::set_scissor_rect(sr);
    }

    //-------------------------------------------------------------------------
    void set_scissor_rects(const rsl::vector<ScissorRect>& scissorRects)
    {
      backend::set_scissor_rects(scissorRects);
    }
  } // namespace renderer
} // namespace rex