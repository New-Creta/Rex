#pragma once

#include "rex_renderer_core/rhi/viewport.h"
#include "rex_renderer_core/rhi/scissor_rect.h"
#include "rex_renderer_core/rhi/resource_state.h"
#include "rex_renderer_core/rhi/primitive_topology.h"
#include "rex_renderer_core/rhi/texture_format.h"

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

#include "rex_renderer_core/rhi/graphics_context.h"

namespace rex
{
  namespace gfx
  {
    class GraphicsEngine;
  }

  namespace rhi
  {
    class ClearStateResource;
    class PipelineState;
    class UploadBuffer;
    class Buffer;
    class Texture2D;

    class RenderContext : public GraphicsContext
    {
    public:
      RenderContext(gfx::GraphicsEngine* owningEngine, rhi::CommandAllocator* alloc);
      RenderContext(const RenderContext&) = delete;
      RenderContext(RenderContext&&) = default;

      ~RenderContext() override;

      RenderContext& operator=(const RenderContext&) = delete;
      RenderContext& operator=(RenderContext&&) = default;

      // Implemented by Graphics API specific derived classes
      // ----------------------------------------------------------------
      virtual void set_viewport(const rhi::Viewport& vp) = 0;
      virtual void set_scissor_rect(const ScissorRect& rect) = 0;
      virtual void transition_buffer(rhi::ConstantBuffer* resource, ResourceState state) = 0;
      virtual void transition_buffer(rhi::VertexBuffer* resource, ResourceState state) = 0;
      virtual void transition_buffer(rhi::IndexBuffer* resource, ResourceState state) = 0;
      virtual void set_render_target(rhi::RenderTarget* renderTarget) = 0;
      virtual void clear_render_target(rhi::RenderTarget* renderTarget, rhi::ClearStateResource* clearState) = 0;
      virtual void set_vertex_buffer(rhi::VertexBuffer* vb) = 0;
      virtual void set_index_buffer(rhi::IndexBuffer* ib) = 0;
      virtual void set_primitive_topology(renderer::PrimitiveTopology topology) = 0;
      virtual void set_blend_factor(const f32 blendFactor[4]) = 0;
      virtual void set_root_signature(rhi::RootSignature* rootSignature) = 0;
      virtual void set_pipeline_state(rhi::PipelineState* pso) = 0;
      virtual void set_graphics_root_descriptor_table(s32 paramIdx, u64 id) = 0;
      virtual void set_constant_buffer(s32 paramIdx, rhi::ConstantBuffer* cb) = 0;

      virtual void draw_indexed(s32 indexCount, s32 startIndexLocation, s32 baseVertexLocation, s32 startInstanceLocation) = 0;
      virtual void draw_indexed_instanced(s32 indexCountPerInstance, s32 instanceCount, s32 startIndexLocation, s32 baseVertexLocation, s32 startInstanceLocation) = 0;

      virtual void update_buffer(Buffer* buffer, UploadBuffer* updateBuffer, void* data, rsl::memory_size size, s32 dstOffset) = 0;
      virtual void update_texture2d(Texture2D* texture, UploadBuffer* updateBuffer, void* data, s32 width, s32 height, renderer::TextureFormat format) = 0;

    protected:
      CommandAllocator* allocator();

    private:
      gfx::GraphicsEngine* m_owning_engine;
      CommandAllocator* m_cmd_allocator;
    };
  }
}