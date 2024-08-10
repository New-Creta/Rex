#pragma once

#include "rex_renderer_core/system/view_heap.h"
#include "rex_directx/system/dx_resource_view.h"
#include "rex_std/bonus/memory.h"
#include "rex_directx/utility/dx_util.h"

namespace rex
{
  namespace gfx
  {
    class Resource;
    class IsShaderVisible;
    struct SamplerDesc;

    // CPU writeable/readable table based on shader visible flag
    // For more info, visit: https://learn.microsoft.com/en-us/windows/win32/direct3d12/non-shader-visible-descriptor-heaps?redirectedfrom=MSDN
    // Note: table is at the bottom of the page
    // +---------------------+---------------------+----------------------+
    // |      View Type      |    Shader Visible 	 |   Non-Shader visible |
    // +---------------------+---------------------+----------------------+
    // |    CBV, SRV, UAV    |         Write       |       Read/Write     |
    // +---------------------+---------------------+----------------------+
    // |       SAMPLER       |         Write       |       Read/Write     |
    // +---------------------+---------------------+----------------------+
    // |        RTV          |      Read/Write     |       Read/Write     |
    // +---------------------+---------------------+----------------------+
    // |        DSV          |      Read/Write     |       Read/Write     |
    // +---------------------+---------------------+----------------------+

    class DxViewHeap : public ViewHeap
    {
    public:
      DxViewHeap(const wrl::ComPtr<ID3D12DescriptorHeap>& viewHeap, const wrl::ComPtr<ID3D12Device1>& device, IsShaderVisible isShaderVisible);

      // Create a render target view and return a handle pointing to it
      DxResourceView create_rtv(ID3D12Resource* resource);
      // Create a depth stencil view and return a handle pointing to it
      DxResourceView create_dsv(ID3D12Resource* resource, DXGI_FORMAT format);
      // Create a constant buffer view and return a handle pointing to it
      DxResourceView create_cbv(ID3D12Resource* resource, rsl::memory_size size);
      // Create a shader resource view pointing to a texture and return a handle pointing to this view
      DxResourceView create_texture2d_srv(ID3D12Resource* resource);

      // Create a 2D texture sampler
      rsl::unique_ptr<DxSampler2D> create_sampler2d(const SamplerDesc& desc);

      // Copy the given views into this heap
      rsl::unique_ptr<ResourceView> copy_views(const rsl::vector<ResourceView*>& views) override;

      // Return the internal wrapped descriptor heap
      ID3D12DescriptorHeap* dx_object();

      // Reset the descriptor heap
      // This will cause new descriptor to be allocated from the beginning of the heap
      // this does not destroy existing descriptors, 
      // it only repoints the allocating start back to the beginning of the heap
      void clear() override;

    private:
      // Return a handle pointing to a free bit of memory in the descriptor heap
      DxResourceView new_free_handle(s32 numDescriptors = 1);
      // Return a handle pointing to the start of the descriptor heap
      DxResourceView my_start_handle();
      // Create a handle pointing to no resource
      void init_null_handle();

    private:
      wrl::ComPtr<ID3D12DescriptorHeap> m_view_heap;
      wrl::ComPtr<ID3D12Device1> m_device;
      D3D12_DESCRIPTOR_HEAP_TYPE m_view_heap_type;
      s32 m_view_size;
      s32 m_num_views;
      s32 m_num_used_views;
      bool m_is_shader_visible;
      DxResourceView m_null_view;
    };
  } // namespace gfx
} // namespace rex