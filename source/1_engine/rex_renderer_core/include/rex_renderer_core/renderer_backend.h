#pragma once

#include "rex_engine/engine/types.h"
#include "rex_std/bonus/memory/memory_size.h"

namespace rex
{ 
  class Scene;

  namespace renderer
  {
    struct Viewport;
    struct ScissorRect;

    struct Info;
    struct OutputWindowUserData;

    class ResourceSlot;

    enum class PrimitiveTopology;
    enum class Format;

    struct CreateClearStateDesc;
    struct CreateRasterStateDesc;
    struct CreateTextureDesc;
    struct CreateVertexBufferDesc;
    struct CreateIndexBufferDesc;
    struct CreateRootSignatureDesc;
    struct CreatePipelineStateDesc;

    namespace backend
    {
      bool initialize(const OutputWindowUserData& userData);
      bool post_initialize();
      void shutdown();

      // general accessors
      const Info& info();

      bool is_y_up();
      bool is_depth_0_to_1();

      /**
       * Draw scene
       */
      void draw_scene(Scene* scene);

      /**
       * Render the gui
       */
      void draw_gui();

      /**
      * End drawing the active frame
      */
      void end_frame();

      /**
       * Swap out for a new back buffer to draw to
       */
      void present();

      /**
       * Flush all command queues.
       */
      void flush();

      /**
       * Copy the contents to a vertex buffer in GPU memory.
       */
      ResourceSlot copy_vertex_buffer(size_t numVertices, size_t vertexStride, const void* vertexBufferData);

      /**
       * Copy the contents to a index buffer in GPU memory.
       */
      ResourceSlot copy_index_buffer(size_t numIndices, Format indexFormat, const void* indexBufferData);

      /**
       * Resource creation functions
       */
      ResourceSlot create_clear_state(CreateClearStateDesc&& clearStateParams);
      ResourceSlot create_raster_state(CreateRasterStateDesc&& rasterStateParams);
      ResourceSlot create_texture(CreateTextureDesc&& createTextureParams);
      ResourceSlot create_vertex_buffer(CreateVertexBufferDesc&& createBufferParams);
      ResourceSlot create_index_buffer(CreateIndexBufferDesc&& createBufferParams);
      ResourceSlot create_pipeline_state_object(CreatePipelineStateDesc&& createPipelineStateParams);

      /**
       * Manually release a resource
       */
      void release_resource(const ResourceSlot& slot);

      /**
       * Clear a texture.
       */
      void clear_texture(const ResourceSlot& texture, const ResourceSlot& clearState);

      /**
       * Clear depth/stencil texture.
       */
      void clear_depth_stencil_texture(const ResourceSlot& texture, const ResourceSlot& clearState);

      /**
       * Draw geometry.
       */
      void draw_geometry(u32 vertexCount, u32 instanceCount = 1, u32 startVertex = 0, u32 startInstance = 0);
      void draw_geometry_indexed(u32 indexCount, u32 instanceCount = 1, u32 startIndex = 0, s32 baseVertex = 0, u32 startInstance = 0);

      /**
       * Set a new rasterization state
       */
      void set_raster_state(const ResourceSlot& rasterState);

      /**
       * Set the render targets for the graphics rendering pipeline.
       */
      void set_render_target(const ResourceSlot& renderTarget);

      /**
       * Set the pipeline state object on the command list.
       */
      void set_pipeline_state(const ResourceSlot& pipelineState);

      /**
       * Set a dynamic constant buffer data to an inline descriptor in the root
       * signature.
       */
      void set_graphics_dynamic_constant_buffer(u32 rootParameterIndex, rsl::memory_size sizeInBytes, const void* bufferData);

      /**
       * Set a set of 32-bit constants on the graphics pipeline.
       */
      void set_graphics_32_bit_constants(u32 rootParameterIndex, u32 numConstants, const void* constants);

      /**
       * Set the vertex buffer to the rendering pipeline.
       */
      void set_vertex_buffer(u32 slot, const ResourceSlot& vertexBuffer);
      void set_vertex_buffers(u32 slot, const rsl::vector<ResourceSlot>& vertexBuffers);

      /**
       * Set dynamic vertex buffer data to the rendering pipeline.
       */
      void set_dynamic_vertex_buffer(u32 slot, size_t numVertices, rsl::memory_size vertexSize, const void* vertexBufferData);

      /**
       * Bind the index buffer to the rendering pipeline.
       */
      void set_index_buffer(const ResourceSlot& indexBuffer);

      /**
       * Bind dynamic index buffer data to the rendering pipeline.
       */
      void set_dynamic_index_buffer(size_t numIndicies, Format indexFormat, const void* indexBufferData);

      /**
       * Set the current primitive topology for the rendering pipeline.
       */
      void set_primitive_topology(PrimitiveTopology primitiveTopology);
      /**
       * Set viewports.
       */
      void set_viewport(const Viewport& viewport);
      void set_viewports(const rsl::vector<Viewport>& viewports);

      /**
       * Set scissor rects.
       */
      void set_scissor_rect(const ScissorRect& scissorRect);
      void set_scissor_rects(const rsl::vector<ScissorRect>& scissorRects);
    } // namespace backend
  }   // namespace renderer
} // namespace rex