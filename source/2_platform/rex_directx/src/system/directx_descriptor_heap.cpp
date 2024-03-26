#include "rex_directx/system/directx_descriptor_heap.h"

#include "rex_directx/d3dx12.h"

#include "rex_directx/system/directx_resource.h"

#include "rex_engine/diagnostics/assert.h"

namespace rex
{
  namespace rhi
  {
    DescriptorHeap::DescriptorHeap(const wrl::ComPtr<ID3D12DescriptorHeap>& descHeap, const wrl::ComPtr<ID3D12Device1>& device)
      : m_descriptor_heap(descHeap)
      , m_device(device)
      , m_used_descriptors(0)
    {
      D3D12_DESCRIPTOR_HEAP_DESC desc = m_descriptor_heap->GetDesc();
      m_desc_heap_type = desc.Type;
      m_num_descriptors = desc.NumDescriptors;
      m_descriptor_size = m_device->GetDescriptorHandleIncrementSize(m_desc_heap_type);
    }

    D3D12_CPU_DESCRIPTOR_HANDLE DescriptorHeap::create_rtv(ID3D12Resource* resource)
    {
      CD3DX12_CPU_DESCRIPTOR_HANDLE rtv_handle(m_descriptor_heap->GetCPUDescriptorHandleForHeapStart());

      D3D12_RENDER_TARGET_VIEW_DESC rtv_desc{};
      rtv_desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
      rtv_desc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;

      rtv_handle.Offset(m_used_descriptors, m_descriptor_size);
      m_device->CreateRenderTargetView(resource, &rtv_desc, rtv_handle);
      ++m_used_descriptors;

      return rtv_handle;
    }

    D3D12_CPU_DESCRIPTOR_HANDLE DescriptorHeap::create_dsv(ID3D12Resource* resource, DXGI_FORMAT format)
    {
      CD3DX12_CPU_DESCRIPTOR_HANDLE dsv_handle(m_descriptor_heap->GetCPUDescriptorHandleForHeapStart());

      D3D12_DEPTH_STENCIL_VIEW_DESC dsv_desc{};
      dsv_desc.Flags = D3D12_DSV_FLAG_NONE;
      dsv_desc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
      dsv_desc.Format = format;

      dsv_handle.Offset(m_used_descriptors, m_descriptor_size);
      m_device->CreateDepthStencilView(resource, &dsv_desc, dsv_handle);
      ++m_used_descriptors;

      return dsv_handle;
    }

    D3D12_CPU_DESCRIPTOR_HANDLE DescriptorHeap::create_cbv(ID3D12Resource* resource, s32 size)
    {
      CD3DX12_CPU_DESCRIPTOR_HANDLE cbv_handle(m_descriptor_heap->GetCPUDescriptorHandleForHeapStart());

      // Create a new constant buffer view given the offsetted GPU address and the size of the constant buffer in bytes
      D3D12_CONSTANT_BUFFER_VIEW_DESC cbv_desc;
      cbv_desc.BufferLocation = resource->GetGPUVirtualAddress();
      cbv_desc.SizeInBytes = size;

      cbv_handle.Offset(m_used_descriptors, m_descriptor_size);
      m_device->CreateConstantBufferView(&cbv_desc, cbv_handle);
      ++m_used_descriptors;

      return cbv_handle;
    }

    D3D12_GPU_DESCRIPTOR_HANDLE DescriptorHeap::gpu_heap_start()
    {
      return m_descriptor_heap->GetGPUDescriptorHandleForHeapStart();
    }

  }
}