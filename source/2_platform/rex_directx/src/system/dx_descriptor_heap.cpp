#include "rex_directx/system/dx_descriptor_heap.h"


#include "rex_engine/diagnostics/assert.h"
#include "rex_engine/engine/casting.h"
#include "rex_engine/memory/pointer_math.h"

namespace rex
{
  namespace gfx
  {
    DxDescriptorHeap::DxDescriptorHeap(const wrl::ComPtr<ID3D12DescriptorHeap>& descHeap, const wrl::ComPtr<ID3D12Device1>& device)
        : m_descriptor_heap(descHeap)
        , m_device(device)
        , m_num_used_descriptors(0)
        , m_descriptor_size(0)
    {
      const D3D12_DESCRIPTOR_HEAP_DESC desc = m_descriptor_heap->GetDesc();
      m_desc_heap_type                = desc.Type;
      m_num_descriptors               = static_cast<s32>(desc.NumDescriptors);
      m_descriptor_size               = static_cast<s32>(m_device->GetDescriptorHandleIncrementSize(m_desc_heap_type)); // NOLINT(cppcoreguidelines-prefer-member-initializer)
    }

    // Create a render target view and return a handle pointing to it
    DescriptorHandle DxDescriptorHeap::create_rtv(ID3D12Resource* resource)
    {
      REX_ASSERT_X(m_desc_heap_type == D3D12_DESCRIPTOR_HEAP_TYPE_RTV, "Trying to create a render target view from a descriptor heap that's not configured to create render target views");

      D3D12_RENDER_TARGET_VIEW_DESC rtv_desc {};
      rtv_desc.Format        = DXGI_FORMAT_R8G8B8A8_UNORM;
      rtv_desc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;

      DescriptorHandle rtv_handle = new_free_handle();
      m_device->CreateRenderTargetView(resource, &rtv_desc, rtv_handle);

      return rtv_handle;
    }
    // Create a depth stencil view and return a handle pointing to it
    DescriptorHandle DxDescriptorHeap::create_dsv(ID3D12Resource* resource, DXGI_FORMAT format)
    {
      REX_ASSERT_X(m_desc_heap_type == D3D12_DESCRIPTOR_HEAP_TYPE_DSV, "Trying to create a depth stencil view from a descriptor heap that's not configured to create depth stencil views");

      D3D12_DEPTH_STENCIL_VIEW_DESC dsv_desc {};
      dsv_desc.Flags         = D3D12_DSV_FLAG_NONE;
      dsv_desc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
      dsv_desc.Format        = format;

      DescriptorHandle dsv_handle = new_free_handle();
      m_device->CreateDepthStencilView(resource, &dsv_desc, dsv_handle);

      return dsv_handle;
    }
    // Create a constant buffer view and return a handle pointing to it
    DescriptorHandle DxDescriptorHeap::create_cbv(ID3D12Resource* resource, rsl::memory_size size)
    {
      REX_ASSERT_X(m_desc_heap_type == D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, "Trying to create a constant buffer view from a descriptor heap that's not configured to create constant buffers views");

      // Create a new constant buffer view given the offsetted GPU address and the size of the constant buffer in bytes
      D3D12_CONSTANT_BUFFER_VIEW_DESC cbv_desc;
      cbv_desc.BufferLocation = resource->GetGPUVirtualAddress();
      cbv_desc.SizeInBytes = size.size_in_bytes();

      DescriptorHandle cbv_handle = new_free_handle();
      m_device->CreateConstantBufferView(&cbv_desc, cbv_handle);

      return cbv_handle;
    }
    // Create a shader resource view pointing to a texture and return a handle pointing to this view
    DescriptorHandle DxDescriptorHeap::create_texture2d_srv(ID3D12Resource* resource)
    {
      REX_ASSERT_X(m_desc_heap_type == D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, "Trying to create a constant buffer view from a descriptor heap that's not configured to create constant buffers views");

      D3D12_RESOURCE_DESC resource_desc = resource->GetDesc();
      D3D12_SHADER_RESOURCE_VIEW_DESC desc{};
      desc.Format = resource_desc.Format;
      desc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
      desc.Texture2D.MipLevels = 1;
      desc.Texture2D.MostDetailedMip = 0;
      desc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;

      DescriptorHandle desc_handle = new_free_handle();
      m_device->CreateShaderResourceView(resource, &desc, desc_handle);
      return desc_handle;
    }

    // Return the internal wrapped descriptor heap
    ID3D12DescriptorHeap* DxDescriptorHeap::get()
    {
      return m_descriptor_heap.Get();
    }

    // Reset the descriptor heap
    // This will cause new descriptor to be allocated from the beginning of the heap
    // this does not destroy existing descriptors, 
    // it only repoints the allocating start back to the beginning of the heap
    void DxDescriptorHeap::reset()
    {
      m_num_used_descriptors = 0;
    }

    // Return a handle pointing to the start of the descriptor heap
    DescriptorHandle DxDescriptorHeap::new_free_handle()
    {
      DescriptorHandle handle = my_start_handle();
      handle += m_num_used_descriptors;
      ++m_num_used_descriptors;
      return handle;
    }
    // Return a handle pointing to a free bit of memory in the descriptor heap
    DescriptorHandle DxDescriptorHeap::my_start_handle()
    {
      return DescriptorHandle(m_descriptor_heap->GetCPUDescriptorHandleForHeapStart(), m_descriptor_heap->GetGPUDescriptorHandleForHeapStart(), m_desc_heap_type, m_descriptor_size);
    }

  } // namespace gfx
} // namespace rex