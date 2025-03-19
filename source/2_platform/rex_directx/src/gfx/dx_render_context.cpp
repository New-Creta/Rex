#include "rex_directx/gfx/dx_render_context.h"

#include "rex_directx/utility/d3dx12.h"
#include "rex_directx/resources/dx_constant_buffer.h"
#include "rex_directx/resources/dx_vertex_buffer.h"
#include "rex_directx/resources/dx_index_buffer.h"
#include "rex_directx/resources/dx_render_target.h"
#include "rex_directx/resources/dx_root_signature.h"
#include "rex_directx/resources/dx_pipeline_state.h"
#include "rex_directx/resources/dx_texture_2d.h"
#include "rex_directx/resources/dx_sampler_2d.h"
#include "rex_directx/resources/dx_depth_stencil_buffer.h"
#include "rex_directx/resources/dx_unordered_access_buffer.h"
#include "rex_engine/gfx/resources/clear_state.h"
#include "rex_engine/engine/casting.h"
#include "rex_directx/system/dx_command_allocator.h"
#include "rex_directx/system/dx_gal.h"
#include "rex_directx/utility/dx_util.h"

#include "WinPixEventRuntime/pix3.h"

#include "rex_engine/gfx/graphics.h"

namespace rex
{
  namespace gfx
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
      DxConstantBuffer* dx_constant_buffer = d3d::to_dx12(resource);
      transition_buffer(resource, dx_constant_buffer->dx_object(), state);
    }
    // Transition a vertex buffer's resource state
    void DxRenderContext::transition_buffer(VertexBuffer* resource, ResourceState state)
    {
      DxVertexBuffer* dx_vertex_buffer = d3d::to_dx12(resource);
      transition_buffer(resource, dx_vertex_buffer->dx_object(), state);
    }
    // Transition an index buffer's resource state
    void DxRenderContext::transition_buffer(IndexBuffer* resource, ResourceState state)
    {
      DxIndexBuffer* dx_index_buffer = d3d::to_dx12(resource);
      transition_buffer(resource, dx_index_buffer->dx_object(), state);
    }
    // Transition an index buffer's resource state
    void DxRenderContext::transition_buffer(UnorderedAccessBuffer* resource, ResourceState state)
    {
      DxUnorderedAccessBuffer* dx_unordered_access_buffer = d3d::to_dx12(resource);
      transition_buffer(resource, dx_unordered_access_buffer->dx_object(), state);
    }
    // Transition a upload buffer's resource state
    void DxRenderContext::transition_buffer(UploadBuffer* resource, ResourceState state)
    {
      DxUploadBuffer* dx_upload_buffer = d3d::to_dx12(resource);
      transition_buffer(resource, dx_upload_buffer->dx_object(), state);
    }
    // Transition a texture's resource state
    void DxRenderContext::transition_buffer(Texture2D* resource, ResourceState state)
    {
      DxTexture2D* dx_texture = d3d::to_dx12(resource);
      transition_buffer(resource, dx_texture->dx_object(), state);
    }
    // Transition a render target's resource state
    void DxRenderContext::transition_buffer(RenderTarget* resource, ResourceState state)
    {
      DxRenderTarget* dx_rt = d3d::to_dx12(resource);
      transition_buffer(resource, dx_rt->dx_object(), state);
    }
    void DxRenderContext::transition_buffer(DepthStencilBuffer* resource, ResourceState state)
    {
      DxDepthStencilBuffer* dx_ds = d3d::to_dx12(resource);
      transition_buffer(resource, dx_ds->dx_object(), state);
    }

    // Set the render target of the context
    void DxRenderContext::set_render_target(RenderTarget* colorRenderTarget, DepthStencilBuffer* depthRenderTarget)
    {
      DxRenderTarget* dx_color_render_target = d3d::to_dx12(colorRenderTarget);
      if (depthRenderTarget)
      {
        DxResourceView* dx_depth_render_target = d3d::to_dx12(depthRenderTarget->resource_view());
        m_cmd_list->OMSetRenderTargets(1, &dx_color_render_target->view().cpu_handle(), true, &dx_depth_render_target->cpu_handle());
      }
      else
      {
			  m_cmd_list->OMSetRenderTargets(1, &dx_color_render_target->view().cpu_handle(), true, nullptr);
      }
    }
    // Clear the render target of the context
    void DxRenderContext::clear_render_target(RenderTarget* renderTarget, DepthStencilBuffer* depthRenderTarget)
    {
      REX_ASSERT_X(renderTarget, "Trying to clear a nullptr rendertarget");

			DxRenderTarget* dx_render_target = d3d::to_dx12(renderTarget);
			m_cmd_list->ClearRenderTargetView(dx_render_target->view(), renderTarget->clear_state().rgba.data(), 0, nullptr);

      if (depthRenderTarget)
      {
        s32 d3d_clear_flags = 0;
        d3d_clear_flags |= depthRenderTarget->clear_state().flags.has_state(ClearBits::ClearDepthBuffer) ? D3D12_CLEAR_FLAG_DEPTH : 0;
        d3d_clear_flags |= depthRenderTarget->clear_state().flags.has_state(ClearBits::ClearStencilBuffer) ? D3D12_CLEAR_FLAG_STENCIL : 0;

        DxResourceView* dsv = d3d::to_dx12(depthRenderTarget->resource_view());
        m_cmd_list->ClearDepthStencilView(dsv->cpu_handle(), (D3D12_CLEAR_FLAGS)d3d_clear_flags, depthRenderTarget->clear_state().depth, depthRenderTarget->clear_state().stencil, 0, nullptr);
      }
    }
    void DxRenderContext::clear_depth_stencil_target(DepthStencilBuffer* depthRenderTarget)
    {
      REX_ASSERT_X(depthRenderTarget, "Trying to clear a nullptr depth render target");
      s32 d3d_clear_flags = 0;
      d3d_clear_flags |= depthRenderTarget->clear_state().flags.has_state(ClearBits::ClearDepthBuffer) ? D3D12_CLEAR_FLAG_DEPTH : 0;
      d3d_clear_flags |= depthRenderTarget->clear_state().flags.has_state(ClearBits::ClearStencilBuffer) ? D3D12_CLEAR_FLAG_STENCIL : 0;

      DxResourceView* dsv = d3d::to_dx12(depthRenderTarget->resource_view());
      m_cmd_list->ClearDepthStencilView(dsv->cpu_handle(), (D3D12_CLEAR_FLAGS)d3d_clear_flags, depthRenderTarget->clear_state().depth, depthRenderTarget->clear_state().stencil, 0, nullptr);

    }
    // Set the vertex buffer of the context
    void DxRenderContext::set_vertex_buffer(VertexBuffer* vb, s32 startSlot)
    {
      DxVertexBuffer* dx_vb = d3d::to_dx12(vb);

      D3D12_VERTEX_BUFFER_VIEW view{};
      view.BufferLocation = dx_vb->dx_object()->GetGPUVirtualAddress();
      view.SizeInBytes = narrow_cast<s32>(vb->size().size_in_bytes());
      view.StrideInBytes = static_cast<UINT>(vb->vertex_size());

      m_cmd_list->IASetVertexBuffers(startSlot, 1, &view);
    }
    // Set the index buffer of the context
    void DxRenderContext::set_index_buffer(IndexBuffer* ib)
    {
      DxIndexBuffer* dx_ib = d3d::to_dx12(ib);

      D3D12_INDEX_BUFFER_VIEW view{};
      view.BufferLocation = dx_ib->dx_object()->GetGPUVirtualAddress();
      view.Format = d3d::to_dx12(ib->format());
      view.SizeInBytes = narrow_cast<s32>(ib->total_size());

      m_cmd_list->IASetIndexBuffer(&view);
    }
    // Set the primitive topology of the context
    void DxRenderContext::set_primitive_topology(PrimitiveTopology topology)
    {
      auto d3d_topology = d3d::to_dx12(topology);
      m_cmd_list->IASetPrimitiveTopology(d3d_topology);
    }

    // Set the root signature of the context
    void DxRenderContext::set_root_signature(RootSignature* rootSignature)
    {
      DxRootSignature* dx_root_signature = d3d::to_dx12(rootSignature);
      m_cmd_list->SetGraphicsRootSignature(dx_root_signature->dx_object());
    }
    // Set the pipeline state of the context
    void DxRenderContext::set_pipeline_state(PipelineState* pso)
    {
      DxPipelineState* dx_pso = d3d::to_dx12(pso);
      m_cmd_list->SetPipelineState(dx_pso->dx_object());
    }
    // Set a view table of the context
    void DxRenderContext::bind_view_table(s32 paramIdx, const ResourceView* startView)
    {
      const DxResourceView* dx_start_view = d3d::to_dx12(startView);
      m_cmd_list->SetGraphicsRootDescriptorTable(paramIdx, dx_start_view->gpu_handle());
    }
    // Bind a constant buffer to the context
    void DxRenderContext::bind_constant_buffer(s32 paramIdx, u64 gpuAddress)
    {
      m_cmd_list->SetGraphicsRootConstantBufferView(paramIdx, gpuAddress);
    }
    void DxRenderContext::bind_constant_buffer(s32 paramIdx, ConstantBuffer* constantBuffer)
    {
      m_cmd_list->SetGraphicsRootConstantBufferView(paramIdx, constantBuffer->gpu_address());
    }
    void DxRenderContext::bind_texture2d(s32 paramIdx, Texture2D* texture)
    {
    //  const ResourceView* gpu_handle = gpu_engine()->try_get_texture_gpu_handle(texture);

    //  // If the texture is not on the GPU yet, copy it there first
    //  if (gpu_handle == nullptr)
    //  {
    //    rsl::vector<const ResourceView*> views;
    //    views.push_back(texture->resource_view());
    //    auto copy_ctx = new_copy_ctx();
    //    auto start_handle = copy_ctx->copy_views(ViewHeapType::Texture2D, views);
    //    gpu_handle = start_handle.get();
				//gpu_engine()->notify_textures_presence_on_gpu(texture, rsl::move(start_handle));
    //  }

      rsl::vector<const ResourceView*> views;
      views.push_back(texture->resource_view());
      auto copy_ctx = new_copy_ctx();
      auto start_handle = copy_ctx->copy_views(ViewHeapType::Texture2D, views);

			// Textures need to be bind using a view table and cannot be bound directly
      bind_view_table(paramIdx, start_handle);
    }
    // Bind a shader resource to the context
    void DxRenderContext::bind_shader_resource(s32 paramIdx, u64 gpuAddress)
    {
      m_cmd_list->SetGraphicsRootShaderResourceView(paramIdx, gpuAddress);
    }
    // Bind a unordered access buffer to the context
    void DxRenderContext::bind_unordered_access_buffer(s32 paramIdx, u64 gpuAddress)
    {
      m_cmd_list->SetGraphicsRootUnorderedAccessView(paramIdx, gpuAddress);
    }

    // Set the blend factor of the context
    void DxRenderContext::set_blend_factor(const f32 blendFactor[4])
    {
      m_cmd_list->OMSetBlendFactor(blendFactor);
    }
    // Set the blend factor of the context
    void DxRenderContext::set_blend_factor(const BlendFactor& blendFactor)
    {
      set_blend_factor(blendFactor.data());
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

    // Return the wrapped directx commandlist
    ID3D12GraphicsCommandList* DxRenderContext::dx_cmdlist()
    {
      return m_cmd_list.Get();
    }

    // Reset the wrapped commandlist and its allocater
    void DxRenderContext::platform_reset(CommandAllocator* alloc, const ContextResetData& resetData)
    {
      DxCommandAllocator* dx_alloc = d3d::to_dx12(alloc);
      d3d::reset_cmdlist(m_cmd_list.Get(), dx_alloc, resetData);
    }

    // Profiling events
    void DxRenderContext::begin_profile_event(rsl::string_view eventName)
    {
      m_profile_event_name = eventName;
      if (m_profile_event_name.size())
      {
        ::PIXBeginEvent(m_cmd_list.Get(), 0, eventName.data());
      }
    }
    void DxRenderContext::end_profile_event()
    {
      if (m_profile_event_name.size())
      {
        ::PIXEndEvent(m_cmd_list.Get());
      }
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