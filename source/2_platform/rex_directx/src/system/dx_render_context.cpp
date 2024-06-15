#include "rex_directx/system/dx_render_context.h"

#include "rex_directx/utility/d3dx12.h"
#include "rex_directx/resources/dx_constant_buffer.h"
#include "rex_directx/resources/dx_vertex_buffer.h"
#include "rex_directx/resources/dx_index_buffer.h"
#include "rex_directx/resources/dx_render_target.h"
#include "rex_directx/resources/dx_root_signature.h"
#include "rex_directx/resources/dx_pipeline_state.h"
#include "rex_directx/resources/dx_texture_2d.h"
#include "rex_renderer_core/resources/clear_state.h"
#include "rex_engine/engine/casting.h"
#include "rex_directx/system/dx_command_allocator.h"
#include "rex_directx/system/dx_rhi.h"

namespace rex
{
  namespace rhi
  {
    DxRenderContext::DxRenderContext(gfx::GraphicsEngine* owningEngine, const wrl::ComPtr<ID3D12GraphicsCommandList> cmdList)
      : RenderContext(owningEngine)
      , m_cmd_list(cmdList)
    {
    }

    // Set the viewport of the context
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
    // Set the scissor rect of the context
    void DxRenderContext::set_scissor_rect(const ScissorRect& rect)
    {
      RECT scissor_rect{};
      scissor_rect.top = (LONG)rect.top;
      scissor_rect.left = (LONG)rect.left;
      scissor_rect.bottom = (LONG)rect.bottom;
      scissor_rect.right = (LONG)rect.right;

      m_cmd_list->RSSetScissorRects(1, &scissor_rect);
    }

    // Transition a constant buffer's resource state
    void DxRenderContext::transition_buffer(ConstantBuffer* resource, ResourceState state)
    {
      DxConstantBuffer* dx_constant_buffer = static_cast<DxConstantBuffer*>(resource);
      transition_buffer(resource, dx_constant_buffer->dx_object(), state);
    }
    // Transition a vertex buffer's resource state
    void DxRenderContext::transition_buffer(VertexBuffer* resource, ResourceState state)
    {
      DxVertexBuffer* dx_vertex_buffer = static_cast<DxVertexBuffer*>(resource);
      transition_buffer(resource, dx_vertex_buffer->dx_object(), state);
    }
    // Transition an index buffer's resource state
    void DxRenderContext::transition_buffer(IndexBuffer* resource, ResourceState state)
    {
      DxIndexBuffer* dx_index_buffer = static_cast<DxIndexBuffer*>(resource);
      transition_buffer(resource, dx_index_buffer->dx_object(), state);
    }
    // Transition a upload buffer's resource state
    void DxRenderContext::transition_buffer(UploadBuffer* resource, ResourceState state)
    {
      DxUploadBuffer* dx_upload_buffer = static_cast<DxUploadBuffer*>(resource);
      transition_buffer(resource, dx_upload_buffer->dx_object(), state);
    }
    // Transition a texture's resource state
    void DxRenderContext::transition_buffer(Texture2D* resource, ResourceState state)
    {
      DxTexture2D* dx_texture = static_cast<DxTexture2D*>(resource);
      transition_buffer(resource, dx_texture->dx_object(), state);
    }
    // Transition a render target's resource state
    void DxRenderContext::transition_buffer(RenderTarget* resource, ResourceState state)
    {
      DxRenderTarget* dx_rt = static_cast<DxRenderTarget*>(resource);
      transition_buffer(resource, dx_rt->dx_object(), state);
    }

    // Set the render target of the context
    void DxRenderContext::set_render_target(RenderTarget* renderTarget)
    {
      DxRenderTarget* dx_render_target = static_cast<DxRenderTarget*>(renderTarget);
      m_cmd_list->OMSetRenderTargets(1, &dx_render_target->handle().cpu_handle(), true, nullptr);
    }
    // Clear the render target of the context
    void DxRenderContext::clear_render_target(RenderTarget* renderTarget, ClearState* clearState)
    {
      auto& clear_flags = clearState->get()->flags;
      if (clear_flags.has_state(renderer::ClearBits::ClearColorBuffer))
      {
        DxRenderTarget* dx_render_target = static_cast<DxRenderTarget*>(renderTarget);
        m_cmd_list->ClearRenderTargetView(dx_render_target->handle(), clearState->get()->rgba.data(), 0, nullptr);
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
    // Set the vertex buffer of the context
    void DxRenderContext::set_vertex_buffer(VertexBuffer* vb)
    {
      DxVertexBuffer* dx_vb = static_cast<DxVertexBuffer*>(vb);

      D3D12_VERTEX_BUFFER_VIEW view{};
      view.BufferLocation = dx_vb->dx_object()->GetGPUVirtualAddress();
      view.SizeInBytes = narrow_cast<s32>(vb->size().size_in_bytes());
      view.StrideInBytes = vb->vertex_size();

      m_cmd_list->IASetVertexBuffers(0, 1, &view);
    }
    // Set the index buffer of the context
    void DxRenderContext::set_index_buffer(IndexBuffer* ib)
    {
      DxIndexBuffer* dx_ib = static_cast<DxIndexBuffer*>(ib);

      D3D12_INDEX_BUFFER_VIEW view{};
      view.BufferLocation = dx_ib->dx_object()->GetGPUVirtualAddress();
      view.Format = d3d::to_dx12(ib->format());
      view.SizeInBytes = narrow_cast<s32>(ib->total_size());

      m_cmd_list->IASetIndexBuffer(&view);
    }
    // Set the primitive topology of the context
    void DxRenderContext::set_primitive_topology(renderer::PrimitiveTopology topology)
    {
      auto d3d_topology = rex::d3d::to_dx12(topology);
      m_cmd_list->IASetPrimitiveTopology(d3d_topology);
    }

    // Set the root signature of the context
    void DxRenderContext::set_root_signature(RootSignature* rootSignature)
    {
      DxRootSignature* dx_root_signature = static_cast<DxRootSignature*>(rootSignature);
      m_cmd_list->SetGraphicsRootSignature(dx_root_signature->dx_object());
    }
    // Set the pipeline state of the context
    void DxRenderContext::set_pipeline_state(PipelineState* pso)
    {
      DxPipelineState* dx_pso = static_cast<DxPipelineState*>(pso);
      m_cmd_list->SetPipelineState(dx_pso->get());
    }
    // Set the graphics root descriptor table of the context
    void DxRenderContext::set_graphics_root_descriptor_table(s32 paramIdx, UINT64 id)
    {
      D3D12_GPU_DESCRIPTOR_HANDLE texture_handle = {};
      texture_handle.ptr = id;

      m_cmd_list->SetGraphicsRootDescriptorTable(paramIdx, texture_handle);
    }
    // Set the constant buffer of the context at a given index
    void DxRenderContext::set_constant_buffer(s32 paramIdx, ConstantBuffer* cb)
    {
      DxConstantBuffer* dx_cb = static_cast<DxConstantBuffer*>(cb);
      m_cmd_list->SetGraphicsRootConstantBufferView(paramIdx, dx_cb->dx_object()->GetGPUVirtualAddress());
    }
    // Set the blend factor of the context
    void DxRenderContext::set_blend_factor(const f32 blendFactor[4])
    {
      m_cmd_list->OMSetBlendFactor(blendFactor);
    }

    // Draw an indexed primitive
    void DxRenderContext::draw_indexed(s32 indexCount, s32 startIndexLocation, s32 baseVertexLocation, s32 startInstanceLocation)
    {
      draw_indexed_instanced(indexCount, 1, startIndexLocation, baseVertexLocation, startInstanceLocation);
    }
    // Draw instances of indexed primitive
    void DxRenderContext::draw_indexed_instanced(s32 indexCountPerInstance, s32 instanceCount, s32 startIndexLocation, s32 baseVertexLocation, s32 startInstanceLocation)
    {
      m_cmd_list->DrawIndexedInstanced(indexCountPerInstance, instanceCount, startIndexLocation, baseVertexLocation, startInstanceLocation);
    }

    // Bind a texture to the context
    void DxRenderContext::bind_texture(s32 rootParamIdx, Texture2D* texture)
    {
      DxTexture2D* dx_texture = static_cast<DxTexture2D*>(texture);
      m_cmd_list->SetGraphicsRootDescriptorTable(rootParamIdx, dx_texture->gpu_handle());
    
    }

    // Return the wrapped directx commandlist
    ID3D12GraphicsCommandList* DxRenderContext::dx_cmdlist()
    {
      return m_cmd_list.Get();
    }

    // Reset the wrapped commandlist and its allocater
    void DxRenderContext::platform_reset(rhi::CommandAllocator* alloc, DescriptorHeap* descHeap)
    {
      start_recording_commands(alloc, descHeap);
    }

    // Open the commandlist for recording of gpu commands
    void DxRenderContext::start_recording_commands(rhi::CommandAllocator* alloc, DescriptorHeap* descHeap)
    {
      DxCommandAllocator* dx_alloc = static_cast<DxCommandAllocator*>(alloc);

      REX_ASSERT_X(dx_alloc != nullptr, "The command allocator for a context cannot be null");

      dx_alloc->get()->Reset();
      m_cmd_list->Reset(dx_alloc->get(), nullptr);
      ID3D12DescriptorHeap* d3d_desc_heap = d3d::to_dx12(descHeap)->get();
      m_cmd_list->SetDescriptorHeaps(1, &d3d_desc_heap);
    }
    // Transition a buffer into a new resource state
    void DxRenderContext::transition_buffer(Resource* resource, ID3D12Resource* d3d_resource, ResourceState state)
    {
      ResourceStateTransition transition = track_resource_transition(resource, state);
      if (transition.before != transition.after)
      {
        D3D12_RESOURCE_STATES before_state = d3d::to_dx12(transition.before);
        D3D12_RESOURCE_STATES after_state = d3d::to_dx12(transition.after);
        D3D12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(d3d_resource, before_state, after_state);
        m_cmd_list->ResourceBarrier(1, &barrier);
      }
    }

  }
}