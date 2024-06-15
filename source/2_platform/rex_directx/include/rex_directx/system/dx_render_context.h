#pragma once

#include "rex_renderer_core/rhi/render_context.h"

#include "rex_renderer_core/resources/constant_buffer.h"
#include "rex_renderer_core/resources/vertex_buffer.h"
#include "rex_renderer_core/resources/index_buffer.h"

#include "rex_directx/utility/dx_util.h"

namespace rex
{
  namespace rhi
  {
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

      // Set the render target of the context
      void set_render_target(RenderTarget* renderTarget) override;
      // Clear the render target of the context
      void clear_render_target(RenderTarget* renderTarget, ClearState* clearState) override;
      // Set the vertex buffer of the context
      void set_vertex_buffer(VertexBuffer* vb) override;
      // Set the index buffer of the context
      void set_index_buffer(IndexBuffer* ib) override;
      // Set the primitive topology of the context
      void set_primitive_topology(renderer::PrimitiveTopology topology) override;
      // Set the root signature of the context
      void set_root_signature(RootSignature* rootSignature) override;
      // Set the pipeline state of the context
      void set_pipeline_state(PipelineState* pso) override;
      // Set the graphics root descriptor table of the context
      void set_graphics_root_descriptor_table(s32 paramIdx, UINT64 id) override;
      // Set the constant buffer of the context at a given index
      void set_constant_buffer(s32 paramIdx, ConstantBuffer* cb) override;
      // Set the blend factor of the context
      void set_blend_factor(const f32 blendFactor[4]) override;

      // Draw an indexed primitive
      void draw_indexed(s32 indexCount, s32 startIndexLocation, s32 baseVertexLocation, s32 startInstanceLocation) override;
      // Draw instances of indexed primitive
      void draw_indexed_instanced(s32 indexCountPerInstance, s32 instanceCount, s32 startIndexLocation, s32 baseVertexLocation, s32 startInstanceLocation) override;

      // Bind a texture to the context
      void bind_texture(s32 rootParamIdx, Texture2D* texture) override;

      // Return the wrapped directx commandlist
      ID3D12GraphicsCommandList* dx_cmdlist();

    protected:
      // Reset the wrapped commandlist and its allocater
      void platform_reset(rhi::CommandAllocator* alloc, DescriptorHeap* descHeap) override;

    private:
      // Open the commandlist for recording of gpu commands
      void start_recording_commands(rhi::CommandAllocator* alloc, DescriptorHeap* descHeap);
      // Transition a buffer into a new resource state
      void transition_buffer(Resource* resource, ID3D12Resource* d3d_resource, ResourceState state);
      
    private:
      wrl::ComPtr<ID3D12GraphicsCommandList> m_cmd_list;
    };
  }
}