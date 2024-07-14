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
      if (clear_flags.has_state(ClearBits::ClearColorBuffer))
      {
        DxRenderTarget* dx_render_target = static_cast<DxRenderTarget*>(renderTarget);
        m_cmd_list->ClearRenderTargetView(dx_render_target->handle(), clearState->get()->rgba.data(), 0, nullptr);
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
      DxVertexBuffer* dx_vb = static_cast<DxVertexBuffer*>(vb);

      D3D12_VERTEX_BUFFER_VIEW view{};
      view.BufferLocation = dx_vb->dx_object()->GetGPUVirtualAddress();
      view.SizeInBytes = narrow_cast<s32>(vb->size().size_in_bytes());
      view.StrideInBytes = static_cast<UINT>(vb->vertex_size());

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
    void DxRenderContext::set_primitive_topology(PrimitiveTopology topology)
    {
      auto d3d_topology = d3d::to_dx12(topology);
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
      m_cmd_list->SetPipelineState(dx_pso->dx_object());
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
      DxTexture2D* dx_texture = static_cast<DxTexture2D*>(texture);
      m_cmd_list->SetGraphicsRootDescriptorTable(rootParamIdx, dx_texture->gpu_handle());
    
    }
    // Bind a material to the context
    void DxRenderContext::bind_material(Material* material)
    {
      // The following is stored in a material and should be bound when a material gets bound

      // Primitive topology
      set_primitive_topology(material->primitive_topology());
      
      // Pipeline state
      set_pipeline_state(material->pso());
      
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
      bind_resources_for_shader(material, ShaderType::Vertex);
      bind_resources_for_shader(material, ShaderType::Pixel);
    }

    // Return the wrapped directx commandlist
    ID3D12GraphicsCommandList* DxRenderContext::dx_cmdlist()
    {
      return m_cmd_list.Get();
    }

    // Reset the wrapped commandlist and its allocater
    void DxRenderContext::platform_reset(CommandAllocator* alloc, const ContextResetData& resetData)
    {
      start_recording_commands(alloc);

      rsl::array<ID3D12DescriptorHeap*, 2> d3d_desc_heaps{};
      d3d_desc_heaps[0] = d3d::to_dx12(resetData.shader_visible_srv_desc_heap)->dx_object();
      d3d_desc_heaps[1] = d3d::to_dx12(resetData.shader_visible_sampler_desc_heap)->dx_object();
      m_cmd_list->SetDescriptorHeaps(d3d_desc_heaps.size(), d3d_desc_heaps.data());
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

    // Open the commandlist for recording of gpu commands
    void DxRenderContext::start_recording_commands(CommandAllocator* alloc)
    {
      DxCommandAllocator* dx_alloc = static_cast<DxCommandAllocator*>(alloc);

      REX_ASSERT_X(dx_alloc != nullptr, "The command allocator for a context cannot be null");

      dx_alloc->dx_object()->Reset();
      m_cmd_list->Reset(dx_alloc->dx_object(), nullptr);
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
    void DxRenderContext::bind_resources_for_shader(Material* material, ShaderType type)
    {
      // 1. Split textures and samplers of material based on shader visibility
      ShaderResources shader_resources = material->resources_for_shader(type);

      // 2. Copy textures and samplers into descriptor heap that's visible in shaders, make sure they're sorted based on shader register
      rsl::vector<ResourceView*> texture_handles;
			// Sort the textures based on their shader register
			rsl::sort(shader_resources.textures.begin(), shader_resources.textures.end(),
				[](const TextureMaterialParameter* lhs, const TextureMaterialParameter* rhs)
				{
					return lhs->shader_register() < rhs->shader_register();
				});

			// copy the texture gpu handles into a separate array
			texture_handles.resize(shader_resources.textures.size());
			rsl::transform(shader_resources.textures.cbegin(), shader_resources.textures.cend(), texture_handles.begin(),
				[](TextureMaterialParameter* texture)
				{
					DxTexture2D* dx_texture = static_cast<DxTexture2D*>(texture->texture());
					return dx_texture->handle();
				});

      rsl::vector<ResourceView*> sampler_handles;
			// Sort the samplers based on their shader register
			rsl::sort(shader_resources.samplers.begin(), shader_resources.samplers.end(),
				[](const SamplerMaterialParameter* lhs, const SamplerMaterialParameter* rhs)
				{
					return lhs->shader_register() < rhs->shader_register();
				});

			// copy the sampler gpu handles into a separate array
			sampler_handles.resize(shader_resources.samplers.size());
			rsl::transform(shader_resources.samplers.cbegin(), shader_resources.samplers.cend(), sampler_handles.begin(),
				[](SamplerMaterialParameter* sampler)
				{
					DxSampler2D* dx_sampler = d3d::to_dx12(sampler->sampler());
					return dx_sampler->gpu_handle();
				});

      // Copy the earlier cached gpu handle descriptors into the shader visible heap
      auto copy_ctx = new_copy_ctx();
      if (!texture_handles.empty())
      {
				rsl::unique_ptr<ResourceView> start_texture_handle = copy_ctx->copy_descriptors(copy_ctx->shader_visible_srv_heap(), texture_handles);
				// 3. Bind the descriptor table based on the descriptors in this shader visible descriptor heap
				if (shader_resources.textures_root_param_idx != -1)
				{
					m_cmd_list->SetGraphicsRootDescriptorTable(shader_resources.textures_root_param_idx, d3d::to_dx12(start_texture_handle.get())->gpu_handle());
				}
      }

      if (!sampler_handles.empty())
      {
        rsl::unique_ptr<ResourceView> start_sampler_handle = copy_ctx->copy_descriptors(copy_ctx->shader_visible_sampler_heap(), sampler_handles);
        if (shader_resources.samplers_root_param_idx != -1)
        {
          m_cmd_list->SetGraphicsRootDescriptorTable(shader_resources.samplers_root_param_idx, d3d::to_dx12(start_sampler_handle.get())->gpu_handle());
        }
      }
    }

  }
}