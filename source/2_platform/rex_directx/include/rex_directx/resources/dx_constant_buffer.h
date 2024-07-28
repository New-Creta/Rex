#pragma once


#include "rex_directx/system/dx_view_heap.h"

#include "rex_renderer_core/resources/constant_buffer.h"

#include "rex_std/vector.h"

namespace rex
{
  namespace gfx
  {
    class DxConstantBuffer : public ConstantBuffer
    {
    public:
      DxConstantBuffer(const wrl::ComPtr<ID3D12Resource>& resource, DxResourceView view, rsl::memory_size size);
      DxConstantBuffer(const wrl::ComPtr<ID3D12Resource>& resource, rsl::vector<DxResourceView>&& views, rsl::memory_size size);

      ID3D12Resource* dx_object();

    private:
      wrl::ComPtr<ID3D12Resource> m_resource; // the actual constant buffer resource
      rsl::vector<DxResourceView> m_views; // A handle for both the cpu and gpu descriptor
    };
  } // namespace gfx
} // namespace rex