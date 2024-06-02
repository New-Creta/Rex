#pragma once

#include "rex_renderer_core/system/resource_state_tracker.h"
#include "rex_renderer_core/rhi/command_type.h"
#include "rex_renderer_core/rhi/primitive_topology.h"
#include "rex_renderer_core/rhi/texture_format.h"

#include "rex_engine/engine/types.h"
#include "rex_std/bonus/memory.h"

namespace rex
{
  namespace rhi
  {
    class CommandAllocator;
    class PipelineState;
    class ClearState;
    class VertexBuffer;
    class IndexBuffer;
    class RootSignature;
    class ConstantBuffer;
    class UploadBuffer;
    class RenderTarget;
    class Texture2D;

    struct Viewport;
    struct ScissorRect;
    class Buffer;

    class CommandList
    {
    public:
      explicit CommandList(rhi::CommandType type, rhi::ResourceStateTracker* resourceStateTracker = nullptr);
      virtual ~CommandList();

      rhi::CommandType type() const;

      // Interface functions
      virtual void start_recording_commands(CommandAllocator* alloc, PipelineState* pso = nullptr) = 0;
      virtual void stop_recording_commands() = 0;

      virtual void set_viewport(const Viewport& vp) = 0;
      virtual void set_scissor_rect(const ScissorRect& rect) = 0;
      virtual void transition_buffer(Buffer* resource, ResourceState state) = 0;
      virtual void set_render_target(RenderTarget* renderTarget) = 0;
      virtual void clear_render_target(RenderTarget* renderTarget, ClearState* clearState) = 0;
      virtual void set_vertex_buffer(VertexBuffer* vb) = 0;
      virtual void set_index_buffer(IndexBuffer* ib) = 0;
      virtual void set_primitive_topology(renderer::PrimitiveTopology topology) = 0;
      virtual void set_blend_factor(const f32 blendFactor[4]) = 0;
      virtual void set_root_signature(RootSignature* rootSignature) = 0;
      virtual void set_pipeline_state(PipelineState* pso) = 0;
      virtual void set_graphics_root_descriptor_table(s32 paramIdx, u64 id) = 0;
      virtual void set_constant_buffer(s32 paramIdx, ConstantBuffer* cb) = 0;

      virtual void draw_indexed(s32 indexCount, s32 startIndexLocation, s32 baseVertexLocation, s32 startInstanceLocation) = 0;
      virtual void draw_indexed_instanced(s32 indexCountPerInstance, s32 instanceCount, s32 startIndexLocation, s32 baseVertexLocation, s32 startInstanceLocation) = 0;

      virtual void send_to_gpu() = 0;

      virtual void update_buffer(Buffer* buffer, UploadBuffer* updateBuffer, void* data, rsl::memory_size size, s32 dstOffset) = 0;
      virtual void update_texture2d(Texture2D* texture, UploadBuffer* updateBuffer, void* data, s32 width, s32 height, renderer::TextureFormat format) = 0;

    private:
      rhi::ResourceStateTracker* m_resource_state_tracker;
      rhi::CommandType m_type;
    };
  }
}
