#include "rex_directx/system/directx_commandlist.h"

#include "rex_directx/diagnostics/directx_call.h"

namespace rex
{
  namespace rhi
  {
    CommandList::CommandList(const wrl::ComPtr<ID3D12GraphicsCommandList>& commandList, const wrl::ComPtr<ID3D12CommandAllocator>& allocator)
      : m_command_list(commandList)
      , m_allocator(allocator)
    {}

    bool CommandList::close()
    {
      return DX_SUCCESS(m_command_list->Close());
    }

  }
}