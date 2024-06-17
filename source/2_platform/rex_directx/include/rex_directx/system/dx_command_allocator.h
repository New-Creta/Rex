#pragma once

#include "rex_directx/utility/dx_util.h"
#include "rex_renderer_core/system/command_allocator.h"

namespace rex
{
  namespace gfx
  {
    class DxCommandAllocator : public CommandAllocator
    {
    public:
      DxCommandAllocator(const wrl::ComPtr<ID3D12CommandAllocator>& alloc);

      ID3D12CommandAllocator* dx_object();

    private:
      wrl::ComPtr<ID3D12CommandAllocator> m_alloc;
    };
  }
}