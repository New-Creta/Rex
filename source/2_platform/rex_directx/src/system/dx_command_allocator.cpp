#include "rex_directx/system/dx_command_allocator.h"

namespace rex
{
  namespace rhi
  {
    CommandAllocator::CommandAllocator(const wrl::ComPtr<ID3D12CommandAllocator>& alloc)
      : m_alloc(alloc)
    {}

    ID3D12CommandAllocator* CommandAllocator::get()
    {
      return m_alloc.Get();
    }

  }
}