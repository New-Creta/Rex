#include "rex_directx/system/dx_copy_context.h"

#include "rex_directx/utility/dx_util.h"
#include "rex_directx/utility/d3dx12.h"

#include "rex_directx/resources/dx_upload_buffer.h"
#include "rex_directx/gfx/dx_copy_engine.h"

#include "rex_directx/resources/dx_constant_buffer.h"
#include "rex_directx/resources/dx_vertex_buffer.h"
#include "rex_directx/resources/dx_index_buffer.h"
#include "rex_directx/resources/dx_texture_2d.h"

namespace rex
{
  namespace rhi
  {
    DxCopyContext::DxCopyContext(gfx::GraphicsEngine* owningEngine, const wrl::ComPtr<ID3D12GraphicsCommandList> cmdList, CommandAllocator* alloc)
      : CopyContext(owningEngine)
      , m_cmd_list(cmdList)
    {
      reset(alloc);
    }

    ID3D12GraphicsCommandList* DxCopyContext::dx_cmdlist()
    {
      return m_cmd_list.Get();
    }

    void DxCopyContext::update_texture2d(rhi::Texture2D* texture, const void* data)
    {
      UploadBuffer* upload_buffer = api_engine()->lock_upload_buffer();

      transition_buffer(texture, ResourceState::CopyDest);

      s32 width = texture->width();
      s32 height = texture->height();
      renderer::TextureFormat format = texture->format();

      s32 write_offset = upload_buffer->prepare_for_new_texture_write(data, width, height, format);
      DxTexture2D* dx_texture = static_cast<DxTexture2D*>(texture);

      CD3DX12_TEXTURE_COPY_LOCATION dst_loc(dx_texture->dx_object(), 0);
      D3D12_PLACED_SUBRESOURCE_FOOTPRINT footprint{};
      footprint.Footprint.Format = d3d::to_dx12(format);
      footprint.Footprint.Width = width;
      footprint.Footprint.Height = height;
      footprint.Footprint.Depth = 1;
      footprint.Footprint.RowPitch = d3d::texture_pitch_size(width, format);
      footprint.Offset = write_offset;

      CD3DX12_TEXTURE_COPY_LOCATION src_loc(upload_buffer->dx_object(), footprint);
      m_cmd_list->CopyTextureRegion(&dst_loc, 0, 0, 0, &src_loc, nullptr);

      // Not allowed to transition to pixel shader resource in a copy command list
      //transition_buffer(texture, ResourceState::PixelShaderResource);

      api_engine()->unlock_upload_buffer();
    }

    void DxCopyContext::transition_buffer(ConstantBuffer* resource, ResourceState state)
    {
      DxConstantBuffer* dx_constant_buffer = static_cast<DxConstantBuffer*>(resource);
      transition_buffer(resource, dx_constant_buffer->dx_object(), state);
    }
    void DxCopyContext::transition_buffer(VertexBuffer* resource, ResourceState state)
    {
      DxVertexBuffer* dx_vertex_buffer = static_cast<DxVertexBuffer*>(resource);
      transition_buffer(resource, dx_vertex_buffer->dx_object(), state);
    }
    void DxCopyContext::transition_buffer(IndexBuffer* resource, ResourceState state)
    {
      DxIndexBuffer* dx_index_buffer = static_cast<DxIndexBuffer*>(resource);
      transition_buffer(resource, dx_index_buffer->dx_object(), state);
    }
    void DxCopyContext::transition_buffer(UploadBuffer* resource, ResourceState state)
    {
      Buffer* buff = (Buffer*)resource;
      transition_buffer(buff, resource->dx_object(), state);
    }
    void DxCopyContext::transition_buffer(Texture2D* resource, ResourceState state)
    {
      Buffer* buff = (Buffer*)resource;
      DxTexture2D* dx_texture = static_cast<DxTexture2D*>(resource);
      transition_buffer(buff, dx_texture->dx_object(), state);
    }

    void DxCopyContext::update_buffer(ConstantBuffer* buffer, const void* data, rsl::memory_size size, s32 offset)
    {
      DxConstantBuffer* dx_constant_buffer = static_cast<DxConstantBuffer*>(buffer);
      transition_buffer(buffer, ResourceState::CopyDest);
      update_buffer(buffer, dx_constant_buffer->dx_object(), data, size, offset);
    }
    void DxCopyContext::update_buffer(VertexBuffer* buffer, const void* data, rsl::memory_size size, s32 offset)
    {
      DxVertexBuffer* dx_vertex_buffer = static_cast<DxVertexBuffer*>(buffer);
      transition_buffer(buffer, ResourceState::CopyDest);
      update_buffer(buffer, dx_vertex_buffer->dx_object(), data, size, offset);
    }
    void DxCopyContext::update_buffer(IndexBuffer* buffer, const void* data, rsl::memory_size size, s32 offset)
    {
      DxIndexBuffer* dx_index_buffer = static_cast<DxIndexBuffer*>(buffer);
      transition_buffer(buffer, ResourceState::CopyDest);
      update_buffer(buffer, dx_index_buffer->dx_object(), data, size, offset);
    }

    DxCopyEngine* DxCopyContext::api_engine()
    {
      return static_cast<DxCopyEngine*>(owning_engine());
    }

    void DxCopyContext::transition_buffer(Buffer* resource, ID3D12Resource* d3d_resource, ResourceState state)
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
    void DxCopyContext::update_buffer(rhi::Buffer* buffer, ID3D12Resource* resource, const void* data, rsl::memory_size size, s32 offset)
    {
      UploadBuffer* upload_buffer = api_engine()->lock_upload_buffer();

      //transition_buffer(upload_buffer, ResourceState::CopySource);

      s32 write_offset = upload_buffer->prepare_for_new_buffer_write(data, size);
      m_cmd_list->CopyBufferRegion(resource, offset, upload_buffer->dx_object(), write_offset, size);

      api_engine()->unlock_upload_buffer();
    }
    void DxCopyContext::platform_reset()
    {
      DxCommandAllocator* dx_alloc = static_cast<DxCommandAllocator*>(allocator());

      REX_ASSERT_X(dx_alloc != nullptr, "The command allocator for a context cannot be null");

      dx_alloc->get()->Reset();
      m_cmd_list->Reset(dx_alloc->get(), nullptr);
    }
  }
}