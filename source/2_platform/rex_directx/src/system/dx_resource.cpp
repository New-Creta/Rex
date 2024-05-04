#include "rex_directx/system/dx_resource.h"

#include "rex_directx/resources/dx_upload_buffer.h"
#include "rex_directx/utility/d3dx12.h"

namespace rex
{
  namespace rhi
  {
    Resource::Resource(const wrl::ComPtr<ID3D12Resource>& resource, D3D12_RESOURCE_STATES startState, s64 size, DXGI_FORMAT format)
        : BaseResource(resource.Get(), make_new_hash())
        , m_resource(resource)
        , m_resource_state(startState)
        , m_size(size)
    {
      const D3D12_RESOURCE_DESC desc = m_resource->GetDesc();
      m_width                  = static_cast<s32>(desc.Width);
      m_height                 = static_cast<s32>(desc.Height);
      m_format                 = format != DXGI_FORMAT_UNKNOWN ? format : desc.Format;
    }

    Resource::Resource(const wrl::ComPtr<ID3D12Resource>& resource, D3D12_RESOURCE_STATES startState, s32 width, s32 height, DXGI_FORMAT format)
      : BaseResource(resource.Get(), make_new_hash())
      , m_resource(resource)
      , m_resource_state(startState)
      , m_width(width)
      , m_height(height)
      , m_format(format)
    {
      m_size = rex::d3d::texture_2d_size(format, width, height);
    }


    s32 Resource::width() const
    {
      return m_width;
    }
    s32 Resource::height() const
    {
      return m_height;
    }
    s64 Resource::size() const
    {
      return m_size;
    }
    DXGI_FORMAT Resource::format() const
    {
      return m_format;
    }
    ID3D12Resource* Resource::get() const
    {
      return m_resource.Get();
    }
    D3D12_RESOURCE_STATES Resource::resource_state() const
    {
      return m_resource_state;
    }

    void Resource::transition(ID3D12GraphicsCommandList* cmdList, D3D12_RESOURCE_STATES to)
    {
      CD3DX12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(m_resource.Get(), m_resource_state, to);
      cmdList->ResourceBarrier(1, &barrier);
      m_resource_state = to;
    }

    void Resource::write_buffer(ID3D12GraphicsCommandList* cmdList, UploadBuffer* uploadBuffer, s64 start, s64 size, s32 offset)
    {
      cmdList->CopyBufferRegion(m_resource.Get(), offset, uploadBuffer->get(), start, size);
    }

    void Resource::write_texture(ID3D12GraphicsCommandList* cmdList, UploadBuffer* uploadBuffer, s64 width, s64 height, s64 rowPitch, s32 offset)
    {
      CD3DX12_TEXTURE_COPY_LOCATION dst_loc(get(), 0);
      D3D12_PLACED_SUBRESOURCE_FOOTPRINT footprint{};
      footprint.Footprint.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
      footprint.Footprint.Width = width;
      footprint.Footprint.Height = height;
      footprint.Footprint.Depth = 1;
      footprint.Footprint.RowPitch = rowPitch;
      footprint.Offset = offset;

      CD3DX12_TEXTURE_COPY_LOCATION src_loc(uploadBuffer->get(), footprint);
      cmdList->CopyTextureRegion(&dst_loc, 0, 0, 0, &src_loc, nullptr);
    }

    void Resource::copy_to(ID3D12GraphicsCommandList* cmdList, ID3D12Resource* srcResource)
    {
      cmdList->CopyResource(m_resource.Get(), srcResource);
    }

  } // namespace rhi
} // namespace rex