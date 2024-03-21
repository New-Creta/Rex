#pragma once

#include "rex_directx/utility/directx_util.h"

namespace rex
{
  namespace rhi
  {
    class CommandList
    {
    public:
      CommandList(const wrl::ComPtr<ID3D12GraphicsCommandList>& commandList, const wrl::ComPtr<ID3D12CommandAllocator>& allocator);

      bool close();

    private:
      wrl::ComPtr<ID3D12GraphicsCommandList> m_command_list;
      wrl::ComPtr<ID3D12CommandAllocator> m_allocator;
    };
  }
}