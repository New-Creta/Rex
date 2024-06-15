#pragma once

#include "rex_renderer_core/system/descriptor_heap.h"
#include "rex_directx/system/dx_descriptor_handle.h"
#include "rex_std/bonus/memory.h"

namespace rex
{
  namespace gfx
  {
    class Resource;

    class DxDescriptorHeap : public DescriptorHeap
    {
    public:
      DxDescriptorHeap(const wrl::ComPtr<ID3D12DescriptorHeap>& descHeap, const wrl::ComPtr<ID3D12Device1>& device);

      // Create a render target view and return a handle pointing to it
      DescriptorHandle create_rtv(ID3D12Resource* resource);
      // Create a depth stencil view and return a handle pointing to it
      DescriptorHandle create_dsv(ID3D12Resource* resource, DXGI_FORMAT format);
      // Create a constant buffer view and return a handle pointing to it
      DescriptorHandle create_cbv(ID3D12Resource* resource, rsl::memory_size size);
      // Create a shader resource view pointing to a texture and return a handle pointing to this view
      DescriptorHandle create_texture2d_srv(ID3D12Resource* resource);

      // Return the internal wrapped descriptor heap
      ID3D12DescriptorHeap* get();

      // Reset the descriptor heap
      // This will cause new descriptor to be allocated from the beginning of the heap
      // this does not destroy existing descriptors, 
      // it only repoints the allocating start back to the beginning of the heap
      void reset();

    private:
      // Return a handle pointing to a free bit of memory in the descriptor heap
      DescriptorHandle new_free_handle();
      // Return a handle pointing to the start of the descriptor heap
      DescriptorHandle my_start_handle();

    private:
      wrl::ComPtr<ID3D12DescriptorHeap> m_descriptor_heap;
      wrl::ComPtr<ID3D12Device1> m_device;
      D3D12_DESCRIPTOR_HEAP_TYPE m_desc_heap_type;
      s32 m_descriptor_size;
      s32 m_num_descriptors;
      s32 m_num_used_descriptors;
    };
  } // namespace gfx
} // namespace rex