#pragma once


#include "rex_directx/system/dx_view_heap.h"

#include "rex_engine/gfx/resources/constant_buffer.h"

namespace rex
{
  namespace gfx
  {
    class DxConstantBuffer : public ConstantBuffer
    {
    public:
      DxConstantBuffer(const wrl::ComPtr<ID3D12Resource>& resource, DxResourceView view, rsl::memory_size size);

      // Return the wrapped dx object
      ID3D12Resource* dx_object();
      // Return the view to the wrapped dx object      
      DxResourceView* view();

    private:
      wrl::ComPtr<ID3D12Resource> m_resource; // the actual constant buffer resource
      DxResourceView m_view; // A handle for both the cpu and gpu descriptor
    };
  } // namespace gfx
} // namespace rex