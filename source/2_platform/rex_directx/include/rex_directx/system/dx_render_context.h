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
      DxRenderContext(gfx::GraphicsEngine* owningEngine, const wrl::ComPtr<ID3D12GraphicsCommandList> cmdList, CommandAllocator* alloc);
      ~DxRenderContext();

      void set_viewport(const Viewport& vp) override;
      void set_scissor_rect(const ScissorRect& rect) override;

      void transition_buffer(ConstantBuffer* resource, ResourceState state) override;
      void transition_buffer(VertexBuffer* resource, ResourceState state) override;
      void transition_buffer(IndexBuffer* resource, ResourceState state) override;
      void transition_buffer(UploadBuffer* resource, ResourceState state) override;
      void transition_buffer(Texture2D* resource, ResourceState state) override;
      void transition_buffer(RenderTarget* resource, ResourceState state) override;

      void set_render_target(RenderTarget* renderTarget) override;
      void clear_render_target(RenderTarget* renderTarget, ClearStateResource* clearState) override;
      void set_vertex_buffer(VertexBuffer* vb) override;
      void set_index_buffer(IndexBuffer* ib) override;
      void set_primitive_topology(renderer::PrimitiveTopology topology) override;
      void set_root_signature(RootSignature* rootSignature) override;
      void set_pipeline_state(PipelineState* pso) override;
      void set_graphics_root_descriptor_table(s32 paramIdx, UINT64 id) override;
      void set_constant_buffer(s32 paramIdx, ConstantBuffer* cb) override;
      void set_blend_factor(const f32 blendFactor[4]) override;

      void draw_indexed(s32 indexCount, s32 startIndexLocation, s32 baseVertexLocation, s32 startInstanceLocation) override;
      void draw_indexed_instanced(s32 indexCountPerInstance, s32 instanceCount, s32 startIndexLocation, s32 baseVertexLocation, s32 startInstanceLocation) override;

      void update_buffer(Buffer* buffer, UploadBuffer* updateBuffer, void* data, rsl::memory_size size, s32 dstOffset) override;
      void update_texture2d(Texture2D* texture, UploadBuffer* updateBuffer, void* data, s32 width, s32 height, renderer::TextureFormat format) override;

      void bind_texture(Texture2D* texture) override;

      ID3D12GraphicsCommandList* dx_cmdlist();

    protected:
      void platform_reset() override;

    private:
      void start_recording_commands();
      void transition_buffer(Resource* resource, ID3D12Resource* d3d_resource, ResourceState state);
      void bind_descriptor_heaps();
      
    private:
      wrl::ComPtr<ID3D12GraphicsCommandList> m_cmd_list;
    };
  }
}