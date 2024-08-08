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
#include "rex_renderer_core/resources/clear_state.h"
#include "rex_engine/engine/casting.h"
#include "rex_directx/system/dx_command_allocator.h"
#include "rex_directx/system/dx_rhi.h"
#include "rex_directx/utility/dx_util.h"

#include "WinPixEventRuntime/pix3.h"

#include "rex_renderer_core/gfx/graphics.h"

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

    // Set the render target of the context
    void DxRenderContext::set_render_target(RenderTarget* renderTarget)
    {
      DxRenderTarget* dx_render_target = d3d::to_dx12(renderTarget);
      m_cmd_list->OMSetRenderTargets(1, &dx_render_target->view().cpu_handle(), true, nullptr);
    }
    // Clear the render target of the context
    void DxRenderContext::clear_render_target(RenderTarget* renderTarget, ClearState* clearState)
    {
      auto& clear_flags = clearState->get()->flags;
      if (clear_flags.has_state(ClearBits::ClearColorBuffer))
      {
        DxRenderTarget* dx_render_target = d3d::to_dx12(renderTarget);
        m_cmd_list->ClearRenderTargetView(dx_render_target->view(), clearState->get()->rgba.data(), 0, nullptr);
      }

      if (clear_flags.has_state(ClearBits::ClearDepthBuffer) || clear_flags.has_state(ClearBits::ClearStencilBuffer))
      {
        s32 d3d_clear_flags = 0;
        d3d_clear_flags |= clear_flags.has_state(ClearBits::ClearDepthBuffer) ? D3D12_CLEAR_FLAG_DEPTH : 0;
        d3d_clear_flags |= clear_flags.has_state(ClearBits::ClearStencilBuffer) ? D3D12_CLEAR_FLAG_STENCIL : 0;

        //DxResourceView dsv = internal::get()->swapchain->depth_stencil_view();
        //internal::get()->command_list->get()->ClearDepthStencilView(dsv.get(), (D3D12_CLEAR_FLAGS)d3d_clear_flags, clear_state->get()->depth, clear_state->get()->stencil, 0, nullptr);
      }
    }
    // Set the vertex buffer of the context
    void DxRenderContext::set_vertex_buffer(VertexBuffer* vb)
    {
      DxVertexBuffer* dx_vb = d3d::to_dx12(vb);

      D3D12_VERTEX_BUFFER_VIEW view{};
      view.BufferLocation = dx_vb->dx_object()->GetGPUVirtualAddress();
      view.SizeInBytes = narrow_cast<s32>(vb->size().size_in_bytes());
      view.StrideInBytes = static_cast<UINT>(vb->vertex_size());

      m_cmd_list->IASetVertexBuffers(0, 1, &view);
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
    // Set the graphics root descriptor table of the context
    void DxRenderContext::set_graphics_root_descriptor_table(s32 paramIdx, UINT64 id)
    {
      D3D12_GPU_DESCRIPTOR_HANDLE texture_handle = {};
      texture_handle.ptr = id;

      m_cmd_list->SetGraphicsRootDescriptorTable(paramIdx, texture_handle);
    }
    void DxRenderContext::set_graphics_root_descriptor_table(s32 paramIdx, ResourceView* startView)
    {
      DxResourceView* dx_start_view = d3d::to_dx12(startView);
      m_cmd_list->SetGraphicsRootDescriptorTable(paramIdx, dx_start_view->gpu_handle());
    }
    // Set the constant buffer of the context at a given index
    void DxRenderContext::set_constant_buffer(s32 paramIdx, Resource* cb)
    {
      DxConstantBuffer* dx_cb = static_cast<DxConstantBuffer*>(cb);
      m_cmd_list->SetGraphicsRootConstantBufferView(paramIdx, dx_cb->dx_object()->GetGPUVirtualAddress());
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

    // Bind a texture to the context
    void DxRenderContext::bind_texture(s32 rootParamIdx, Texture2D* texture)
    {
      DxTexture2D* dx_texture = d3d::to_dx12(texture);
      m_cmd_list->SetGraphicsRootDescriptorTable(rootParamIdx, dx_texture->view()->gpu_handle());
    
    }
    // Bind a material to the context
    void DxRenderContext::bind_material(Material* material)
    {
      // The following is stored in a material and should be bound when a material gets bound

      // Primitive topology
      set_primitive_topology(material->primitive_topology());
            
      // Root signature
      set_root_signature(material->root_signature());
      
      // Blend settings
      set_blend_factor(material->blend_factor());

      // All textures and samplers descriptors for a material are expected to be continuously stored in memory
      // This means that the following needs to be true
      // Let's say we need to bind 5 textures, with the first texture being at address 0x0000 (start of the descriptor heap)
      // If a SRV descriptor is 128 bytes in memory, then the textures need to have the following layout in the descriptor heap
      // texture 0 - 0x0000 == 000
      // texture 1 - 0x0080 == 128
      // texture 2 - 0x0100 == 256
      // texture 3 - 0x0180 == 384
      // texture 4 - 0x0200 == 512

      // A root signature can have 2 kinds of parameters
      // 1. constants
      // Constants are NOT bound by materials, but are bound by the renderer instead
      // Constants themselves have different categories as well (per scene, per view, per object, per instance, ..)
      // I won't go into much detail here as it's explained in the renderer
      // 
      // 2. resources
      // Resources are bound by materials. These are mostly textures and samplers.
      // textures and samplers need to be put in descriptor tables in DirectX.
      // A material can have multiple descriptor tables as they're often tied to shader type
      // Therefore we need to loop over all the material's descriptor tables aka "tied resources"
      // and bind them, linking them to the root parameter idx they were originally created for

      // Textures
      // Samplers

      // Sort the materials based on their slot and register
      // Some might be in tables and they'll use the slot of those
      // This is to make sure that when we copy the descriptors in a heap
      // that's accessible by the GPU, all views are continious in memory
      // and are sorted in the way they're expected by the root signature

      const rsl::vector<ShaderResource>& shader_resources = material->shader_resources();
      auto copy_ctx = new_copy_ctx();
      for (const auto& shader_resource : shader_resources)
      {
        rsl::unique_ptr<ResourceView> start_handle;
        switch (shader_resource.type())
        {
        case ShaderResourceType::ConstantBuffer: start_handle = copy_ctx->copy_views(ViewHeapType::ConstantBufferView, shader_resource.views()); break;
        case ShaderResourceType::Texture:        start_handle = copy_ctx->copy_views(ViewHeapType::ShaderResourceView, shader_resource.views()); break;
        case ShaderResourceType::Sampler:        start_handle = copy_ctx->copy_views(ViewHeapType::Sampler, shader_resource.views());            break;
        }
        m_cmd_list->SetGraphicsRootDescriptorTable(shader_resource.slot(), d3d::to_dx12(start_handle.get())->gpu_handle());
      }
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

    // Bind material resources to the root signature parameter index provided
    void DxRenderContext::bind_material_resources(CopyContext* copyCtx, const rsl::vector<ResourceView*>& views, ViewHeapType type, s32 paramIdx)
    {
      if (!views.empty())
      {
        // Copy all view into a continious block of memory on the GPU
        rsl::unique_ptr<ResourceView> start_sampler_handle = copyCtx->copy_views(type, views);
        if (paramIdx != -1)
        {
          m_cmd_list->SetGraphicsRootDescriptorTable(paramIdx, d3d::to_dx12(start_sampler_handle.get())->gpu_handle());
        }
      }
    }

  }
}