#include "rex_directx/system/directx_resource.h"

#include "rex_directx/resources/upload_buffer.h"
#include "rex_directx/d3dx12.h"

namespace rex
{
  namespace rhi
  {
    Resource::Resource(const wrl::ComPtr<ID3D12Resource>& resource, D3D12_RESOURCE_STATES startState, s32 size, DXGI_FORMAT format)
      : BaseResource(resource.Get(), make_new_hash())
      , m_resource(resource)
      , m_resource_state(startState)
      , m_size(size)
    {
      D3D12_RESOURCE_DESC desc = m_resource->GetDesc();
      m_width = desc.Width;
      m_height = desc.Height;
      m_format = format != DXGI_FORMAT_UNKNOWN
        ? format
        : desc.Format;
    }

    s32 Resource::width() const
    {
      return m_width;
    }
    s32 Resource::height() const
    {
      return m_height;
    }
    s32 Resource::size() const
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

    void Resource::write(ID3D12GraphicsCommandList* cmdList, UploadBuffer* uploadBuffer, s32 start, s32 size)
    {
      cmdList->CopyBufferRegion(m_resource.Get(), 0, uploadBuffer->get(), start, size);
    }

    void Resource::copy_to(ID3D12GraphicsCommandList* cmdList, ID3D12Resource* srcResource)
    {
      cmdList->CopyResource(m_resource.Get(), srcResource);
    }

  }
}