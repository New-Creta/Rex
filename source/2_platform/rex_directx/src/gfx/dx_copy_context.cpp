#include "rex_directx/gfx/dx_copy_context.h"

#include "rex_directx/utility/dx_util.h"
#include "rex_directx/utility/d3dx12.h"

#include "rex_directx/resources/dx_upload_buffer.h"
#include "rex_directx/system/dx_copy_engine.h"

#include "rex_directx/resources/dx_constant_buffer.h"
#include "rex_directx/resources/dx_vertex_buffer.h"
#include "rex_directx/resources/dx_index_buffer.h"
#include "rex_directx/resources/dx_texture_2d.h"
#include "rex_directx/resources/dx_pipeline_state.h"

#include "WinPixEventRuntime/pix3.h"

namespace rex
{
  namespace gfx
  {
    DEFINE_LOG_CATEGORY(LogDxCopyContext);

    DxCopyContext::DxCopyContext(gfx::GraphicsEngine* owningEngine, const wrl::ComPtr<ID3D12GraphicsCommandList> cmdList)
      : CopyContext(owningEngine)
      , m_cmd_list(cmdList)
    {
    }

    // Return the wrapped directx commandlist object
    ID3D12GraphicsCommandList* DxCopyContext::dx_cmdlist()
    {
      return m_cmd_list.Get();
    }

    // Transition a constant buffer to a new resource state
    void DxCopyContext::transition_buffer(ConstantBuffer* resource, ResourceState state)
    {
      DxConstantBuffer* dx_constant_buffer = d3d::to_dx12(resource);
      transition_buffer(resource, dx_constant_buffer->dx_object(), state);
    }
    // Transition a vertex buffer to a new resource state
    void DxCopyContext::transition_buffer(VertexBuffer* resource, ResourceState state)
    {
      DxVertexBuffer* dx_vertex_buffer = d3d::to_dx12(resource);
      transition_buffer(resource, dx_vertex_buffer->dx_object(), state);
    }
    // Transition a index buffer to a new resource state
    void DxCopyContext::transition_buffer(IndexBuffer* resource, ResourceState state)
    {
      DxIndexBuffer* dx_index_buffer = d3d::to_dx12(resource);
      transition_buffer(resource, dx_index_buffer->dx_object(), state);
    }
    // Transition a upload buffer to a new resource state
    void DxCopyContext::transition_buffer(UploadBuffer* resource, ResourceState state)
    {
      DxUploadBuffer* dx_upload_buffer = d3d::to_dx12(resource);
      transition_buffer(dx_upload_buffer, dx_upload_buffer->dx_object(), state);
    }
    // Transition a texture to a new resource state
    void DxCopyContext::transition_buffer(Texture2D* resource, ResourceState state)
    {
      DxTexture2D* dx_texture = d3d::to_dx12(resource);
      transition_buffer(resource, dx_texture->dx_object(), state);
    }

    // Update a constant buffer's data on the gpu
    void DxCopyContext::update_buffer(ConstantBuffer* buffer, const void* data, rsl::memory_size size, s32 offset)
    {
      DxConstantBuffer* dx_constant_buffer = d3d::to_dx12(buffer);
      transition_buffer(buffer, ResourceState::CopyDest);
      update_buffer(dx_constant_buffer->dx_object(), data, size, offset);
    }
    // Update a vertex buffer's data on the gpu
    void DxCopyContext::update_buffer(VertexBuffer* buffer, const void* data, rsl::memory_size size, s32 offset)
    {
      DxVertexBuffer* dx_vertex_buffer = d3d::to_dx12(buffer);
      transition_buffer(buffer, ResourceState::CopyDest);
      update_buffer(dx_vertex_buffer->dx_object(), data, size, offset);
    }
    // Update a index buffer's data on the gpu
    void DxCopyContext::update_buffer(IndexBuffer* buffer, const void* data, rsl::memory_size size, s32 offset)
    {
      DxIndexBuffer* dx_index_buffer = d3d::to_dx12(buffer);
      transition_buffer(buffer, ResourceState::CopyDest);
      update_buffer(dx_index_buffer->dx_object(), data, size, offset);
    }
    // Update a texture's data on the gpu
    void DxCopyContext::update_texture2d(Texture2D* texture, const void* data)
    {
      UploadBufferLock upload_buffer_lock = api_engine()->lock_upload_buffer();

      transition_buffer(texture, ResourceState::CopyDest);

      s32 width = texture->width();
      s32 height = texture->height();
      TextureFormat format = texture->format();

      s64 write_offset = upload_buffer_lock.upload_buffer()->write_texture_data_from_cpu(data, width, height, format);
      DxTexture2D* dx_texture = d3d::to_dx12(texture);

      CD3DX12_TEXTURE_COPY_LOCATION dst_loc(dx_texture->dx_object(), 0);
      D3D12_PLACED_SUBRESOURCE_FOOTPRINT footprint{};
      footprint.Footprint.Format = d3d::to_dx12(format);
      footprint.Footprint.Width = width;
      footprint.Footprint.Height = height;
      footprint.Footprint.Depth = 1;
      footprint.Footprint.RowPitch = d3d::texture_pitch_size(width, format);
      footprint.Offset = write_offset;

      CD3DX12_TEXTURE_COPY_LOCATION src_loc(upload_buffer_lock.upload_buffer()->dx_object(), footprint);
      m_cmd_list->CopyTextureRegion(&dst_loc, 0, 0, 0, &src_loc, nullptr);
    }

    // Reset this context by resetting the commandlist and its allocator
    // Also bind the descriptor heap
    void DxCopyContext::platform_reset(CommandAllocator* alloc, const ContextResetData& resetData)
    {
      DxCommandAllocator* dx_alloc = d3d::to_dx12(alloc);
      d3d::reset_cmdlist(m_cmd_list.Get(), dx_alloc, resetData);
    }
    
    // Return the graphics engine casted into the directx class
    DxCopyEngine* DxCopyContext::api_engine()
    {
      return static_cast<DxCopyEngine*>(owning_engine());
    }

    // Transition a buffer object into a new resource state
    void DxCopyContext::transition_buffer(Resource* resource, ID3D12Resource* d3d_resource, ResourceState state)
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
    // Update a buffer on the gpu
    void DxCopyContext::update_buffer(ID3D12Resource* resource, const void* data, rsl::memory_size size, s32 offset)
    {
      UploadBufferLock upload_buffer_lock = api_engine()->lock_upload_buffer();

      s64 write_offset = upload_buffer_lock.upload_buffer()->write_buffer_data_from_cpu(data, size);
      m_cmd_list->CopyBufferRegion(resource, offset, upload_buffer_lock.upload_buffer()->dx_object(), write_offset, size);
    }

    // Profiling events
    void DxCopyContext::begin_profile_event(rsl::string_view eventName)
    {
      ::PIXBeginEvent(m_cmd_list.Get(), 0, eventName.data());
    }
    void DxCopyContext::end_profile_event()
    {
      ::PIXEndEvent(m_cmd_list.Get());
    }
  }
}