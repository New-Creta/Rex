#include "rex_directx/system/directx_resource.h"

#include "rex_directx/resources/upload_buffer.h"
#include "rex_directx/d3dx12.h"

namespace rex
{
  namespace rhi
  {
    Resource::Resource(ResourceHash hash, const wrl::ComPtr<ID3D12Resource>& resource, D3D12_RESOURCE_STATES startState)
      : BaseResource(resource.Get(), hash)
      , m_resource(resource)
      , m_resource_state(startState)
    {
      D3D12_RESOURCE_DESC desc = m_resource->GetDesc();
      m_width = desc.Width;
      m_height = desc.Height;
      m_format = desc.Format;
    }

    s32 Resource::width() const
    {
      return m_width;
    }
    s32 Resource::height() const
    {
      return m_height;
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

    void Resource::write(ID3D12GraphicsCommandList* cmdList, UploadBuffer* uploadBuffer, const void* data, s32 size)
    {
      D3D12_SUBRESOURCE_DATA sub_resource_data = {};
      sub_resource_data.pData = data;
      sub_resource_data.RowPitch = size;
      sub_resource_data.SlicePitch = sub_resource_data.RowPitch;

      cmdList->CopyResource(m_resource.Get(), uploadBuffer->get());

      UpdateSubresources<1>(cmdList, m_resource.Get(), uploadBuffer->get(), 0, 0, 1, &sub_resource_data);
    }

    void Resource::copy_to(ID3D12GraphicsCommandList* cmdList, ID3D12Resource* srcResource)
    {
      cmdList->CopyResource(m_resource.Get(), srcResource);
    }

  }
}