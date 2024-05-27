#pragma once

#include "rex_renderer_core/rhi/render_context.h"

#include "rex_directx/utility/dx_util.h"

namespace rex
{
  namespace rhi
  {
    class DxRenderContext : public RenderContext
    {
    public:
      DxRenderContext(const wrl::ComPtr<ID3D12GraphicsCommandList> cmdList, CommandAllocator* alloc);
      ~DxRenderContext();

      void set_viewport(const Viewport& vp);
      void set_scissor_rect(const ScissorRect& rect);
      void transition_buffer(Buffer* resource, ResourceState state);
      void set_render_target(RenderTarget* renderTarget);
      void clear_render_target(RenderTarget* renderTarget, ClearStateResource* clearState);
      void set_vertex_buffer(VertexBuffer* vb);
      void set_index_buffer(IndexBuffer* ib);
      void set_primitive_topology(renderer::PrimitiveTopology topology);
      void set_root_signature(RootSignature* rootSignature);
      void set_pipeline_state(PipelineState* pso);
      void set_graphics_root_descriptor_table(s32 paramIdx, UINT64 id);
      void set_constant_buffer(s32 paramIdx, ConstantBuffer* cb);
      void set_blend_factor(const f32 blendFactor[4]);

      void draw_indexed(s32 indexCount, s32 startIndexLocation, s32 baseVertexLocation, s32 startInstanceLocation);
      void draw_indexed_instanced(s32 indexCountPerInstance, s32 instanceCount, s32 startIndexLocation, s32 baseVertexLocation, s32 startInstanceLocation);

      ID3D12GraphicsCommandList* dx_cmdlist();

    private:
      void start_recording_commands();
      
    private:
      wrl::ComPtr<ID3D12GraphicsCommandList> m_cmd_list;
    };
  }
}