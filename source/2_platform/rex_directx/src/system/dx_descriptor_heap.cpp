#include "rex_directx/system/dx_descriptor_heap.h"

#include "rex_directx/system/dx_resource.h"
#include "rex_engine/diagnostics/assert.h"
#include "rex_engine/engine/casting.h"

namespace rex
{
  namespace rhi
  {
    DescriptorHandle::DescriptorHandle(D3D12_CPU_DESCRIPTOR_HANDLE handle, D3D12_DESCRIPTOR_HEAP_TYPE type, s32 size)
        : m_handle(handle)
        , m_type(type)
        , m_size(size)
    {
    }

    DescriptorHandle& DescriptorHandle::operator++()
    {
      m_handle.Offset(1, m_size);
      return *this;
    }
    DescriptorHandle DescriptorHandle::operator++(int)
    {
      const CD3DX12_CPU_DESCRIPTOR_HANDLE handle(m_handle);
      m_handle.Offset(1, m_size);
      return DescriptorHandle(handle, m_type, m_size);
    }

    DescriptorHandle& DescriptorHandle::operator--()
    {
      m_handle.Offset(-1, m_size);
      return *this;
    }
    DescriptorHandle DescriptorHandle::operator--(int)
    {
      const CD3DX12_CPU_DESCRIPTOR_HANDLE handle(m_handle);
      m_handle.Offset(-1, m_size);
      return DescriptorHandle(handle, m_type, m_size);
    }

    DescriptorHandle DescriptorHandle::operator+(s32 offset) const
    {
      DescriptorHandle handle = *this;
      handle += offset;
      return handle;
    }
    DescriptorHandle& DescriptorHandle::operator+=(s32 offset)
    {
      m_handle.Offset(offset, m_size);
      return *this;
    }

    DescriptorHandle DescriptorHandle::operator-(s32 offset) const
    {
      DescriptorHandle handle = *this;
      handle -= offset;
      return handle;
    }
    DescriptorHandle& DescriptorHandle::operator-=(s32 offset)
    {
      m_handle.Offset(offset, m_size);
      return *this;
    }

    const CD3DX12_CPU_DESCRIPTOR_HANDLE& DescriptorHandle::get() const
    {
      return m_handle;
    }

    DescriptorHeap::DescriptorHeap(const wrl::ComPtr<ID3D12DescriptorHeap>& descHeap, const wrl::ComPtr<ID3D12Device1>& device)
        : m_descriptor_heap(descHeap)
        , m_device(device)
        , m_used_descriptors(0)
        , m_descriptor_size(0)
    {
      const D3D12_DESCRIPTOR_HEAP_DESC desc = m_descriptor_heap->GetDesc();
      m_desc_heap_type                = desc.Type;
      m_num_descriptors               = static_cast<s32>(desc.NumDescriptors);
      m_descriptor_size               = static_cast<s32>(m_device->GetDescriptorHandleIncrementSize(m_desc_heap_type)); // NOLINT(cppcoreguidelines-prefer-member-initializer)
    }

    DescriptorHandle DescriptorHeap::create_rtv(ID3D12Resource* resource)
    {
      REX_ASSERT_X(m_desc_heap_type == D3D12_DESCRIPTOR_HEAP_TYPE_RTV, "Trying to create a render target view from a descriptor heap that's not configured to create render target views");

      D3D12_RENDER_TARGET_VIEW_DESC rtv_desc {};
      rtv_desc.Format        = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
      rtv_desc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;

      DescriptorHandle rtv_handle = my_start_handle();
      rtv_handle += m_used_descriptors;
      m_device->CreateRenderTargetView(resource, &rtv_desc, rtv_handle.get());
      ++m_used_descriptors;

      return rtv_handle;
    }

    DescriptorHandle DescriptorHeap::create_dsv(ID3D12Resource* resource, DXGI_FORMAT format)
    {
      REX_ASSERT_X(m_desc_heap_type == D3D12_DESCRIPTOR_HEAP_TYPE_DSV, "Trying to create a depth stencil view from a descriptor heap that's not configured to create depth stencil views");

      D3D12_DEPTH_STENCIL_VIEW_DESC dsv_desc {};
      dsv_desc.Flags         = D3D12_DSV_FLAG_NONE;
      dsv_desc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
      dsv_desc.Format        = format;

      DescriptorHandle dsv_handle = my_start_handle();
      dsv_handle += m_used_descriptors;
      m_device->CreateDepthStencilView(resource, &dsv_desc, dsv_handle.get());
      ++m_used_descriptors;

      return dsv_handle;
    }

    DescriptorHandle DescriptorHeap::create_cbv(ID3D12Resource* resource, rsl::memory_size size)
    {
      REX_ASSERT_X(m_desc_heap_type == D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, "Trying to create a constant buffer view from a descriptor heap that's not configured to create constant buffers views");

      // Create a new constant buffer view given the offsetted GPU address and the size of the constant buffer in bytes
      D3D12_CONSTANT_BUFFER_VIEW_DESC cbv_desc;
      cbv_desc.BufferLocation = resource->GetGPUVirtualAddress();
      cbv_desc.SizeInBytes    = narrow_cast<s32>(size.size_in_bytes());

      DescriptorHandle cbv_handle = my_start_handle();
      cbv_handle += m_used_descriptors;
      m_device->CreateConstantBufferView(&cbv_desc, cbv_handle.get());
      ++m_used_descriptors;

      return cbv_handle;
    }

    D3D12_GPU_DESCRIPTOR_HANDLE DescriptorHeap::gpu_heap_start()
    {
      return m_descriptor_heap->GetGPUDescriptorHandleForHeapStart();
    }

    DescriptorHandle DescriptorHeap::my_start_handle()
    {
      return DescriptorHandle(m_descriptor_heap->GetCPUDescriptorHandleForHeapStart(), m_desc_heap_type, m_descriptor_size);
    }

  } // namespace rhi
} // namespace rex