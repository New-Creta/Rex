#include "rex_directx/system/directx_descriptor_heap.h"

#include "rex_directx/d3dx12.h"

#include "rex_directx/system/directx_resource.h"

#include "rex_engine/diagnostics/assert.h"

namespace rex
{
  namespace rhi
  {
    DescriptorHeap::DescriptorHeap(const wrl::ComPtr<ID3D12DescriptorHeap>& descHeap, const wrl::ComPtr<ID3D12Device>& device)
      : m_descriptor_heap(descHeap)
      , m_device(device)
      , m_used_descriptors(0)
    {
      D3D12_DESCRIPTOR_HEAP_DESC desc = m_descriptor_heap->GetDesc();
      m_desc_heap_type = desc.Type;
      m_num_descriptors = desc.NumDescriptors;
      m_descriptor_size = m_device->GetDescriptorHandleIncrementSize(m_desc_heap_type);
    }

    void DescriptorHeap::create_rtv(const Resource* resource)
    {
      CD3DX12_CPU_DESCRIPTOR_HANDLE rtv_handle(m_descriptor_heap->GetCPUDescriptorHandleForHeapStart());

      D3D12_RENDER_TARGET_VIEW_DESC rtv_desc{};
      rtv_desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
      rtv_desc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;

      rtv_handle.Offset(m_used_descriptors, m_descriptor_size);
      m_device->CreateRenderTargetView(resource->get(), &rtv_desc, rtv_handle);
      ++m_used_descriptors;
    }

    void DescriptorHeap::create_dsv(const Resource* resource)
    {
      CD3DX12_CPU_DESCRIPTOR_HANDLE dsv_handle(m_descriptor_heap->GetCPUDescriptorHandleForHeapStart());

      D3D12_DEPTH_STENCIL_VIEW_DESC dsv_desc{};
      dsv_desc.Flags = D3D12_DSV_FLAG_NONE;
      dsv_desc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
      dsv_desc.Format = resource->format();

      dsv_handle.Offset(m_used_descriptors, m_descriptor_size);
      m_device->CreateDepthStencilView(resource->get(), &dsv_desc, dsv_handle);
      ++m_used_descriptors;
    }

  }
}