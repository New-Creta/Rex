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
    class ClearState;
    class PipelineState;
    class UploadBuffer;
    class Buffer;
    class ConstantBuffer;
    class VertexBuffer;
    class IndexBuffer;
    class UploadBuffer;
    class Texture2D;
    class Material;

    class RenderContext : public GraphicsContext
    {
    public:
      RenderContext(gfx::GraphicsEngine* owningEngine);

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
      // Set the render target of the context
      virtual void set_render_target(RenderTarget* renderTarget) = 0;
      // Clear the render target of the context
      virtual void clear_render_target(RenderTarget* renderTarget, ClearState* clearState) = 0;
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
      virtual void set_graphics_root_descriptor_table(s32 paramIdx, u64 id) = 0;
      virtual void set_graphics_root_descriptor_table(s32 paramIdx, ResourceView* startView) = 0;
      // Set the constant buffer of the context at a given index
      virtual void set_constant_buffer(s32 paramIdx, ConstantBuffer* cb) = 0;

      // Draw an indexed primitive
      virtual void draw_indexed(s32 indexCount, s32 startIndexLocation, s32 baseVertexLocation, s32 startInstanceLocation) = 0;
      // Draw instances of indexed primitive
      virtual void draw_indexed_instanced(s32 indexCountPerInstance, s32 instanceCount, s32 startIndexLocation, s32 baseVertexLocation, s32 startInstanceLocation) = 0;

      // Bind a texture to the context
      virtual void bind_texture(s32 rootParamIdx, Texture2D* texture) = 0;
      // Bind a material to the context
      virtual void bind_material(Material* material) = 0;

    protected:
      void type_specific_reset(const ContextResetData& resetData);
    };
  }
}