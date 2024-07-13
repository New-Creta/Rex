#pragma once

#include "rex_directx/utility/d3dx12.h"
#include "rex_directx/utility/dx_util.h"
#include "rex_engine/engine/types.h"

#include "rex_renderer_core/system/resource_view.h"

namespace rex
{
  namespace gfx
  {
    // A wrapper around both the cpu and gpu descriptor handle of a resource
    // Both are accessible through this object
    // For convenience this object also converts into one of these handles implicitely
    class DxResourceView : public ResourceView
    {
    public:
      DxResourceView() = default;
      DxResourceView(D3D12_CPU_DESCRIPTOR_HANDLE handle, D3D12_GPU_DESCRIPTOR_HANDLE handleGpu, D3D12_DESCRIPTOR_HEAP_TYPE type, s32 size);

      // Increment the internal handle value by 1
      // It'll hold the address of the next descriptor
      DxResourceView& operator++();
      // Create a copy of this handle
      // Increment its internal handle value by 1
      // It'll hold the address of the next descriptor
      DxResourceView operator++(int);

      // Decrement the internal handle value by 1
      // It'll hold the address of the next descriptor
      DxResourceView& operator--();
      // Create a copy of this handle
      // Decrement its internal handle value by 1
      // It'll hold the address of the next descriptor
      DxResourceView operator--(int);

      // Create a copy of this handle
      // Increment its internal handle value by a given offset
      DxResourceView operator+(s32 offset) const;
      // Increment the internal handle value by a given offset
      DxResourceView& operator+=(s32 offset);

      // Create a copy of this handle
      // Decrement its internal handle value by a given offset
      DxResourceView operator-(s32 offset) const;
      // Decrement the internal handle value by a given offset
      DxResourceView& operator-=(s32 offset);

      // Return the descriptor handle pointing to a cpu accessible address
      operator D3D12_CPU_DESCRIPTOR_HANDLE() const;
      // Return the descriptor handle pointing to a gpu accessible address
      operator D3D12_GPU_DESCRIPTOR_HANDLE() const;

      // Return the descriptor handle pointing to a cpu accessible address
      const CD3DX12_CPU_DESCRIPTOR_HANDLE& cpu_handle() const;
      // Return the descriptor handle pointing to a gpu accessible address
      const D3D12_GPU_DESCRIPTOR_HANDLE& gpu_handle() const;

    private:
      CD3DX12_CPU_DESCRIPTOR_HANDLE m_cpu_handle; // The descriptor handle of the cpu addressible address
      CD3DX12_GPU_DESCRIPTOR_HANDLE m_gpu_handle; // The descriptor handle of the gpu addressible address
      D3D12_DESCRIPTOR_HEAP_TYPE m_type; // The typeo of the descriptor it points to
      s32 m_size; // The size of the descriptor it points to
    };
  }
}