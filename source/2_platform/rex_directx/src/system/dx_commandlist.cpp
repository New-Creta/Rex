#include "rex_directx/system/dx_commandlist.h"

#include "rex_engine/diagnostics/assert.h"
#include "rex_engine/engine/casting.h"

#include "rex_directx/system/dx_rhi.h"

namespace rex
{
  namespace rhi
  {
    CommandList::CommandList(const wrl::ComPtr<ID3D12GraphicsCommandList>& commandList)
      : m_cmd_list(commandList)
      , m_alloc(nullptr)
    {
      m_cmd_list->Close();
    }

    void CommandList::start_recording_commands(CommandAllocator* alloc, ID3D12PipelineState* pso)
    {
      REX_ASSERT_X(alloc != nullptr, "The allocator for a commandlist cannot be null");
      REX_ASSERT_X(m_alloc == nullptr, "There's already an allocator assigned to this commandlist");

      m_alloc = alloc;

      alloc->get()->Reset();
      m_cmd_list->Reset(m_alloc->get(), pso);
    }
    void CommandList::stop_recording_commands()
    {
      m_alloc = nullptr;
      m_cmd_list->Close();
      m_resource_state_tracker.update_parent();
      m_resource_state_tracker.clear();
    }

    void CommandList::set_viewport(const Viewport& vp)
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
    void CommandList::set_scissor_rect(const ScissorRect& rect)
    {
      RECT scissor_rect{};
      scissor_rect.top = (LONG)rect.top;
      scissor_rect.left = (LONG)rect.left;
      scissor_rect.bottom = (LONG)rect.bottom;
      scissor_rect.right = (LONG)rect.right;

      m_cmd_list->RSSetScissorRects(1, &scissor_rect);
    }
    void CommandList::transition_buffer(Resource2* resource, ResourceState state)
    {
      ResourceStateTransition transition = m_resource_state_tracker.track_resource_transition(resource, state);
      if (transition.before != transition.after)
      {
        D3D12_RESOURCE_STATES before_state = d3d::to_dx12(transition.before);
        D3D12_RESOURCE_STATES after_state = d3d::to_dx12(transition.after);
        D3D12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(resource->dx_object(), before_state, after_state);
        m_cmd_list->ResourceBarrier(1, &barrier);
      }
    }
    void CommandList::set_render_target(RenderTarget* renderTarget)
    {
      m_cmd_list->OMSetRenderTargets(1, &renderTarget->cpu_handle(), true, nullptr);
    }
    void CommandList::clear_render_target(RenderTarget* renderTarget, ClearStateResource* clearState)
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
    void CommandList::set_vertex_buffer(VertexBuffer* vb)
    {
      D3D12_VERTEX_BUFFER_VIEW view{};
      view.BufferLocation = vb->get()->GetGPUVirtualAddress();
      view.SizeInBytes = narrow_cast<s32>(vb->size());
      view.StrideInBytes = vb->stride();

      m_cmd_list->IASetVertexBuffers(0, 1, &view);
    }
    void CommandList::set_index_buffer(IndexBuffer* ib)
    {
      D3D12_INDEX_BUFFER_VIEW view{};
      view.BufferLocation = ib->get()->GetGPUVirtualAddress();
      view.Format = ib->format();
      view.SizeInBytes = narrow_cast<s32>(ib->size());

      m_cmd_list->IASetIndexBuffer(&view);
    }
    void CommandList::set_primitive_topology(renderer::PrimitiveTopology topology)
    {
      auto d3d_topology = rex::d3d::to_dx12(topology);
      m_cmd_list->IASetPrimitiveTopology(d3d_topology);
    }
    //void CommandList::set_blend_factor(BlendFactor* blendFactor)
    //{
    //  m_cmd_list->OMSetBlendFactor(blendFactor->as_float_array());
    //}
    void CommandList::set_root_signature(RootSignature* rootSignature)
    {
      m_cmd_list->SetGraphicsRootSignature(rootSignature->dx_object());
    }
    void CommandList::set_pipeline_state(PipelineState* pso)
    {
      m_cmd_list->SetPipelineState(pso->get());
    }

    

    void CommandList::draw_indexed(s32 indexCount, s32 startIndexLocation, s32 baseVertexLocation, s32 startInstanceLocation)
    {
      draw_indexed_instanced(indexCount, 1, startIndexLocation, baseVertexLocation, startInstanceLocation);
    }
    void CommandList::draw_indexed_instanced(s32 indexCountPerInstance, s32 instanceCount, s32 startIndexLocation, s32 baseVertexLocation, s32 startInstanceLocation)
    {
      m_cmd_list->DrawIndexedInstanced(indexCountPerInstance, instanceCount, startIndexLocation, baseVertexLocation, startInstanceLocation);
    }

    void CommandList::update_buffer(Resource2* buffer, void* data, rsl::memory_size size)
    {
      UploadBuffer* upload_buffer = rhi::global_upload_buffer();

      transition_buffer(buffer, ResourceState::CopyDest);
      upload_buffer->write(this, buffer, data, size);
    }
    void CommandList::update_texture(Resource2* texture, Resource2* updateBuffer, void* data, rsl::memory_size size)
    {
      // Copy memory into update buffer first

      // Copy memory into texture after
    }


    ID3D12GraphicsCommandList* CommandList::dx_object()
    {
      return m_cmd_list.Get();
    }

  }
}