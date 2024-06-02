//#pragma once
//
//#include "rex_directx/utility/dx_util.h"
//
//#include "rex_directx/system/dx_command_allocator.h"
//
//#include "rex_renderer_core/rhi/viewport.h"
//#include "rex_renderer_core/rhi/scissor_rect.h"
//#include "rex_renderer_core/rhi/primitive_topology.h"
//#include "rex_renderer_core/rhi/resource_state.h"
//#include "rex_renderer_core/rhi/command_type.h"
//#include "rex_renderer_core/resources/clear_state.h"
//
//#include "rex_directx/resources/dx_clear_state_resource.h"
//
//#include "rex_directx/system/dx_resource.h"
//#include "rex_directx/system/dx_blend_factor.h"
//#include "rex_directx/system/dx_resource_state_tracker.h"
//
//namespace rex
//{
//  namespace rhi
//  {
//    class CommandList
//    {
//    public:
//      CommandList(const wrl::ComPtr<ID3D12GraphicsCommandList>& commandList, ResourceStateTracker* parentResourceStateTracker = nullptr);
//
//      void start_recording_commands(CommandAllocator* alloc, ID3D12PipelineState* pso = nullptr);
//      void stop_recording_commands();
//
//      void set_viewport(const Viewport& vp);
//      void set_scissor_rect(const ScissorRect& rect);
//      void transition_buffer(Resource2* resource, ResourceState state);
//      void set_render_target(RenderTarget* renderTarget);
//      void clear_render_target(RenderTarget* renderTarget, ClearStateResource* clearState);
//      void set_vertex_buffer(VertexBuffer* vb);
//      void set_index_buffer(IndexBuffer* ib);
//      void set_primitive_topology(renderer::PrimitiveTopology topology);
//      void set_root_signature(RootSignature* rootSignature);
//      void set_pipeline_state(PipelineState* pso);
//      void set_graphics_root_descriptor_table(s32 paramIdx, UINT64 id);
//      void set_constant_buffer(s32 paramIdx, ConstantBuffer* cb);
//      void set_blend_factor(const f32 blendFactor[4]);
//
//      void draw_indexed(s32 indexCount, s32 startIndexLocation, s32 baseVertexLocation, s32 startInstanceLocation);
//      void draw_indexed_instanced(s32 indexCountPerInstance, s32 instanceCount, s32 startIndexLocation, s32 baseVertexLocation, s32 startInstanceLocation);
//
//      void send_to_gpu();
//
//      void update_buffer(Resource2* buffer, void* data, rsl::memory_size size, s32 dstOffset);
//      void update_texture(Resource2* texture, UploadBuffer* updateBuffer, void* data, s32 width, s32 height, renderer::TextureFormat format);
//
//      ID3D12GraphicsCommandList* dx_object();
//
//      CommandType type() const;
//
//    private:
//      wrl::ComPtr<ID3D12GraphicsCommandList> m_cmd_list;
//      CommandAllocator* m_alloc;
//      ResourceStateTracker m_resource_state_tracker;
//    };
//  }
//}