#pragma once

#include "rex_renderer_core/gfx/viewport.h"
#include "rex_renderer_core/gfx/scissor_rect.h"
#include "rex_renderer_core/gfx/resource_state.h"
#include "rex_renderer_core/gfx/primitive_topology.h"
#include "rex_renderer_core/gfx/texture_format.h"

#include "rex_renderer_core/resources/buffer.h"
#include "rex_renderer_core/resources/render_target.h"
#include "rex_renderer_core/resources/clear_state.h"
#include "rex_renderer_core/resources/vertex_buffer.h"
#include "rex_renderer_core/resources/index_buffer.h"
#include "rex_renderer_core/resources/root_signature.h"
#include "rex_renderer_core/resources/pipeline_state.h"
#include "rex_renderer_core/resources/constant_buffer.h"
#include "rex_renderer_core/resources/clear_state.h"

#include "rex_renderer_core/system/command_allocator.h"

#include "rex_renderer_core/gfx/graphics_context.h"

namespace rex
{
  namespace gfx
  {
    class GraphicsEngine;
  }

  namespace gfx
  {
    class PipelineState;
    class UploadBuffer;
    class Buffer;
    class ConstantBuffer;
    class VertexBuffer;
    class IndexBuffer;
    class UploadBuffer;
    class Texture2D;
    class Material;
    class DepthStencilBuffer;

    class RenderContext : public GraphicsContext
    {
    public:
      RenderContext(gfx::GraphicsEngine* owningEngine);

      // Calls the graphics API set_render_target function with a null depth stencil buffer
      void set_render_target(RenderTarget* colorRenderTarget);
      // Calls the graphics API clear_render_target function with a null depth stencil buffer
      void clear_render_target(RenderTarget* renderTarget);

      // Implemented by Graphics API specific derived classes
      // ----------------------------------------------------------------
      // Set the viewport of the context
      virtual void set_viewport(const Viewport& vp) = 0;
      // Set the scissor rect of the context
      virtual void set_scissor_rect(const ScissorRect& rect) = 0;
      // Transition a constant buffer's resource state
      virtual void transition_buffer(ConstantBuffer* resource, ResourceState state) = 0;
      // Transition a vertex buffer's resource state
      virtual void transition_buffer(VertexBuffer* resource, ResourceState state) = 0;
      // Transition an index buffer's resource state
      virtual void transition_buffer(IndexBuffer* resource, ResourceState state) = 0;
      // Transition a upload buffer's resource state
      virtual void transition_buffer(UploadBuffer* resource, ResourceState state) = 0;
      // Transition a texture's resource state
      virtual void transition_buffer(Texture2D* resource, ResourceState state) = 0;
      // Transition a render target's resource state
      virtual void transition_buffer(RenderTarget* resource, ResourceState state) = 0;
      // Transition a depth stencil buffer's resource state
      virtual void transition_buffer(DepthStencilBuffer* resource, ResourceState state) = 0;
      // Set the render target of the context
      virtual void set_render_target(RenderTarget* colorRenderTarget, DepthStencilBuffer* depthRenderTarget) = 0;
      // Clear the render target of the context
      virtual void clear_render_target(RenderTarget* renderTarget, DepthStencilBuffer* depthRenderTarget) = 0;
      // Clears the depth stencil target of the context
      virtual void clear_depth_stencil_target(DepthStencilBuffer* depthRenderTarget) = 0;
      // Set the vertex buffer of the context
      virtual void set_vertex_buffer(VertexBuffer* vb) = 0;
      // Set the index buffer of the context
      virtual void set_index_buffer(IndexBuffer* ib) = 0;
      // Set the primitive topology of the context
      virtual void set_primitive_topology(PrimitiveTopology topology) = 0;
      // Set the blend factor of the context
      virtual void set_blend_factor(const f32 blendFactor[4]) = 0;
      // Set the blend factor of the context
      virtual void set_blend_factor(const BlendFactor& blendFactor) = 0;
      // Set the root signature of the context
      virtual void set_root_signature(RootSignature* rootSignature) = 0;
      // Set the pipeline state of the context
      virtual void set_pipeline_state(PipelineState* pso) = 0;
      // Set the graphics root descriptor table of the context
      virtual void bind_view_table(s32 paramIdx, ResourceView* startView) = 0;
      // Bind a constant buffer to the pipeline, tied to a param slot
      virtual void bind_constant_buffer(s32 paramIdx, u64 gpuAddress) = 0;
      virtual void bind_constant_buffer(s32 paramIdx, ConstantBuffer* constantBuffer) = 0;
      // Bind a shader resource to the pipeline, tied to a param slot
      virtual void bind_shader_resource(s32 paramIdx, u64 gpuAddress) = 0;
      // Bind an unordered access to the pipeline, tied to a param slot
      virtual void bind_unordered_access_buffer(s32 paramIdx, u64 gpuAddress) = 0;

      // Draw an indexed primitive
      virtual void draw_indexed(s32 indexCount, s32 startIndexLocation, s32 baseVertexLocation, s32 startInstanceLocation) = 0;
      // Draw instances of indexed primitive
      virtual void draw_indexed_instanced(s32 indexCountPerInstance, s32 instanceCount, s32 startIndexLocation, s32 baseVertexLocation, s32 startInstanceLocation) = 0;

      // Bind a material to the context
      // Returns true if the pgo has changed
      virtual bool bind_material(Material* material) = 0;

    protected:
      void type_specific_reset(const ContextResetData& resetData);
    };
  }
}