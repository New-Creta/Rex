#pragma once

#include "rex_directx/directx_util.h"

namespace rex
{
  namespace renderer
  {
    namespace backend
    {
      ID3D12Device* device();
      ID3D12GraphicsCommandList* command_list();
      ID3D12CommandQueue* command_queue();
      DXGI_FORMAT rtv_format();
      ID3D12DescriptorHeap* cbv_srv_heap();
      D3D12_CPU_DESCRIPTOR_HANDLE cbv_srv_uav_cpu_desc_handle_for_heap_start();
      D3D12_GPU_DESCRIPTOR_HANDLE cbv_srv_uav_gpu_desc_handle_for_heap_start();
      ID3D12Fence* fence();
      ID3D12Resource* back_buffer();
      ID3D12CommandAllocator* frame_command_allocator();
      D3D12_CPU_DESCRIPTOR_HANDLE back_buffer_cpu_desc_handle_for_heap_start();
    }
  } // namespace renderer
} // namespace rex