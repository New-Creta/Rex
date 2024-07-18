#pragma once


#include "rex_directx/system/dx_view_heap.h"

#include "rex_renderer_core/resources/constant_buffer.h"

namespace rex
{
  namespace gfx
  {
    class DxConstantBuffer : public ConstantBuffer
    {
    public:
      DxConstantBuffer(const wrl::ComPtr<ID3D12Resource>& resource, DxResourceView handle, rsl::memory_size size);

      ID3D12Resource* dx_object();

    private:
      wrl::ComPtr<ID3D12Resource> m_resource; // the actual constant buffer resource
      DxResourceView m_handle; // A handle for both the cpu and gpu descriptor
    };
  } // namespace gfx
} // namespace rex