#include "rex_directx/system/dx_render_context.h"

#include "rex_directx/utility/d3dx12.h"

namespace rex
{
  namespace rhi
  {
    DxRenderContext::DxRenderContext(const wrl::ComPtr<ID3D12GraphicsCommandList> cmdList, CommandAllocator* alloc)
      : RenderContext(alloc)
      , m_cmd_list(cmdList)
    {
      start_recording_commands();
    }

    DxRenderContext::~DxRenderContext()
    {
    }

    void DxRenderContext::set_viewport(const Viewport& vp)
    {
      D3D12_VIEWPORT d3d_viewport;
      d3d_viewport.TopLeftX = vp.top_left_x;
      d3d_viewport.TopLeftY = vp.top_left_y;
      d3d_viewport.Width = vp.width;
      d3d_viewport.Height = vp.height;
      d3d_viewport.MinDepth = vp.min_depth;
      d3d_viewport.MaxDepth = vp.max_depth;

      m_cmd_list->RSSetViewports(1, &d3d_viewport);
    }
    void DxRenderContext::set_scissor_rect(const ScissorRect& rect)
    {
      RECT scissor_rect{};
      scissor_rect.top = (LONG)rect.top;
      scissor_rect.left = (LONG)rect.left;
      scissor_rect.bottom = (LONG)rect.bottom;
      scissor_rect.right = (LONG)rect.right;

      m_cmd_list->RSSetScissorRects(1, &scissor_rect);
    }
    void DxRenderContext::transition_buffer(Buffer* resource, ResourceState state)
    {
      ResourceStateTransition transition = track_resource_transition(resource, state);
      if (transition.before != transition.after)
      {
        D3D12_RESOURCE_STATES before_state = d3d::to_dx12(transition.before);
        D3D12_RESOURCE_STATES after_state = d3d::to_dx12(transition.after);
        D3D12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(resource->dx_object(), before_state, after_state);
        m_cmd_list->ResourceBarrier(1, &barrier);
      }
    }
    void DxRenderContext::set_render_target(RenderTarget* renderTarget)
    {
      m_cmd_list->OMSetRenderTargets(1, &renderTarget->cpu_handle(), true, nullptr);
    }
    void DxRenderContext::clear_render_target(RenderTarget* renderTarget, ClearStateResource* clearState)
    {
      auto& clear_flags = clearState->get()->flags;
      if (clear_flags.has_state(renderer::ClearBits::ClearColorBuffer))
      {
        m_cmd_list->ClearRenderTargetView(renderTarget->cpu_handle(), clearState->get()->rgba.data(), 0, nullptr);
      }

      if (clear_flags.has_state(renderer::ClearBits::ClearDepthBuffer) || clear_flags.has_state(renderer::ClearBits::ClearStencilBuffer))
      {
        s32 d3d_clear_flags = 0;
        d3d_clear_flags |= clear_flags.has_state(renderer::ClearBits::ClearDepthBuffer) ? D3D12_CLEAR_FLAG_DEPTH : 0;
        d3d_clear_flags |= clear_flags.has_state(renderer::ClearBits::ClearStencilBuffer) ? D3D12_CLEAR_FLAG_STENCIL : 0;

        //DescriptorHandle dsv = internal::get()->swapchain->depth_stencil_view();
        //internal::get()->command_list->get()->ClearDepthStencilView(dsv.get(), (D3D12_CLEAR_FLAGS)d3d_clear_flags, clear_state->get()->depth, clear_state->get()->stencil, 0, nullptr);
      }
    }
    void DxRenderContext::set_vertex_buffer(VertexBuffer* vb)
    {
      D3D12_VERTEX_BUFFER_VIEW view{};
      view.BufferLocation = vb->dx_object()->GetGPUVirtualAddress();
      view.SizeInBytes = narrow_cast<s32>(vb->total_size());
      view.StrideInBytes = vb->stride();

      m_cmd_list->IASetVertexBuffers(0, 1, &view);
    }
    void DxRenderContext::set_index_buffer(IndexBuffer* ib)
    {
      D3D12_INDEX_BUFFER_VIEW view{};
      view.BufferLocation = ib->dx_object()->GetGPUVirtualAddress();
      view.Format = d3d::to_dx12(ib->format());
      view.SizeInBytes = narrow_cast<s32>(ib->total_size());

      m_cmd_list->IASetIndexBuffer(&view);
    }
    void DxRenderContext::set_primitive_topology(renderer::PrimitiveTopology topology)
    {
      auto d3d_topology = rex::d3d::to_dx12(topology);
      m_cmd_list->IASetPrimitiveTopology(d3d_topology);
    }

    void DxRenderContext::set_root_signature(RootSignature* rootSignature)
    {
      m_cmd_list->SetGraphicsRootSignature(rootSignature->dx_object());
    }
    void DxRenderContext::set_pipeline_state(PipelineState* pso)
    {
      m_cmd_list->SetPipelineState(pso->get());
    }
    void DxRenderContext::set_graphics_root_descriptor_table(s32 paramIdx, UINT64 id)
    {
      D3D12_GPU_DESCRIPTOR_HANDLE texture_handle = {};
      texture_handle.ptr = id;

      m_cmd_list->SetGraphicsRootDescriptorTable(paramIdx, texture_handle);
    }
    void DxRenderContext::set_constant_buffer(s32 paramIdx, ConstantBuffer* cb)
    {
      m_cmd_list->SetGraphicsRootConstantBufferView(paramIdx, cb->dx_object()->GetGPUVirtualAddress());
    }
    void DxRenderContext::set_blend_factor(const f32 blendFactor[4])
    {
      m_cmd_list->OMSetBlendFactor(blendFactor);
    }

    void DxRenderContext::draw_indexed(s32 indexCount, s32 startIndexLocation, s32 baseVertexLocation, s32 startInstanceLocation)
    {
      draw_indexed_instanced(indexCount, 1, startIndexLocation, baseVertexLocation, startInstanceLocation);
    }
    void DxRenderContext::draw_indexed_instanced(s32 indexCountPerInstance, s32 instanceCount, s32 startIndexLocation, s32 baseVertexLocation, s32 startInstanceLocation)
    {
      m_cmd_list->DrawIndexedInstanced(indexCountPerInstance, instanceCount, startIndexLocation, baseVertexLocation, startInstanceLocation);
    }

    void DxRenderContext::start_recording_commands()
    {
      CommandAllocator* alloc = resource_cast(allocator());

      REX_ASSERT_X(alloc != nullptr, "The command allocator for a context cannot be null");

      alloc->dx_object()->Reset();
      m_cmd_list->Reset(alloc->dx_object(), nullptr);

      bind_descriptor_heaps();
    }

    ID3D12GraphicsCommandList* DxRenderContext::dx_cmdlist()
    {
      return m_cmd_list.Get();
    }

  }
}