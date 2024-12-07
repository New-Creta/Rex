#pragma once

#include "rex_engine/gfx/system/render_context.h"

#include "rex_engine/gfx/resources/constant_buffer.h"
#include "rex_engine/gfx/resources/vertex_buffer.h"
#include "rex_engine/gfx/resources/index_buffer.h"

#include "rex_directx/utility/dx_util.h"

namespace rex
{
  namespace gfx
  {
    class Material;
    class CopyContext;

    class DxRenderContext : public RenderContext
    {
    public:
      DxRenderContext(gfx::GraphicsEngine* owningEngine, const wrl::ComPtr<ID3D12GraphicsCommandList> cmdList);

      // Set the viewport of the context
      void set_viewport(const Viewport& vp) override;
      // Set the scissor rect of the context
      void set_scissor_rect(const ScissorRect& rect) override;

      // Transition a constant buffer's resource state
      void transition_buffer(ConstantBuffer* resource, ResourceState state) override;
      // Transition a vertex buffer's resource state
      void transition_buffer(VertexBuffer* resource, ResourceState state) override;
      // Transition an index buffer's resource state
      void transition_buffer(IndexBuffer* resource, ResourceState state) override;
      // Transition a upload buffer's resource state
      void transition_buffer(UploadBuffer* resource, ResourceState state) override;
      // Transition a texture's resource state
      void transition_buffer(Texture2D* resource, ResourceState state) override;
      // Transition a render target's resource state
      void transition_buffer(RenderTarget* resource, ResourceState state) override;
      void transition_buffer(DepthStencilBuffer* resource, ResourceState state) override;

      // Set the render target of the context
      void set_render_target(RenderTarget* colorRenderTarget, DepthStencilBuffer* depthRenderTarget) override;
      // Clear the render target of the context
      void clear_render_target(RenderTarget* renderTarget, DepthStencilBuffer* depthRenderTarget) override;
      // Clears the depth stencil target of the context
      void clear_depth_stencil_target(DepthStencilBuffer* depthRenderTarget) override;
      // Set the vertex buffer of the context
      void set_vertex_buffer(VertexBuffer* vb, s32 startSlot) override;
      // Set the index buffer of the context
      void set_index_buffer(IndexBuffer* ib) override;
      // Set the primitive topology of the context
      void set_primitive_topology(PrimitiveTopology topology) override;
      // Set the root signature of the context
      void set_root_signature(RootSignature* rootSignature) override;
      // Set the pipeline state of the context
      void set_pipeline_state(PipelineState* pso) override;
      // Set the graphics root descriptor table of the context
      void bind_view_table(s32 paramIdx, ResourceView* startCb) override;
      // Set the constant buffer of the context at a given index
      void bind_constant_buffer(s32 paramIdx, u64 gpuAddress) override;
      void bind_constant_buffer(s32 paramIdx, ConstantBuffer* constantBuffer) override;
      void bind_shader_resource(s32 paramIdx, u64 gpuAddress) override;
      void bind_unordered_access_buffer(s32 paramIdx, u64 gpuAddress) override;

      // Set the blend factor of the context
      void set_blend_factor(const f32 blendFactor[4]) override;
      // Set the blend factor of the context
      void set_blend_factor(const BlendFactor& blendFactor) override;

      // Draw an indexed primitive
      void draw_indexed(s32 indexCount, s32 startIndexLocation, s32 baseVertexLocation, s32 startInstanceLocation) override;
      // Draw instances of indexed primitive
      void draw_indexed_instanced(s32 indexCountPerInstance, s32 instanceCount, s32 startIndexLocation, s32 baseVertexLocation, s32 startInstanceLocation) override;

      // Return the wrapped directx commandlist
      ID3D12GraphicsCommandList* dx_cmdlist();

    protected:
      // Reset the wrapped commandlist and its allocater
      void platform_reset(CommandAllocator* alloc, const ContextResetData& data) override;

      // profiling events
      void begin_profile_event(rsl::string_view eventName) override;
      void end_profile_event() override;

    private:
      // Transition a buffer into a new resource state
      void transition_buffer(Resource* resource, ID3D12Resource* d3d_resource, ResourceState state);
      
    private:
      wrl::ComPtr<ID3D12GraphicsCommandList> m_cmd_list;
      rsl::string_view m_profile_event_name;
    };
  }
}