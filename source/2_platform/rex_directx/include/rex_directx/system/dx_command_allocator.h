#pragma once

#include "rex_directx/utility/dx_util.h"

namespace rex
{
  namespace rhi
  {
    class CommandAllocator
    {
    public:
      CommandAllocator(const wrl::ComPtr<ID3D12CommandAllocator>& alloc);

      ID3D12CommandAllocator* get();

    private:
      wrl::ComPtr<ID3D12CommandAllocator> m_alloc;
    };
  }
}