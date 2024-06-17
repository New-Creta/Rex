#include "rex_directx/system/dx_command_allocator.h"

namespace rex
{
  namespace gfx
  {
    DxCommandAllocator::DxCommandAllocator(const wrl::ComPtr<ID3D12CommandAllocator>& alloc)
      : m_alloc(alloc)
    {}

    ID3D12CommandAllocator* DxCommandAllocator::dx_object()
    {
      return m_alloc.Get();
    }

  }
}